//******************************************************************************
/*!
  \file      src/Main/QuinoaPrint.h
  \author    J. Bakosi
  \date      Mon 22 Sep 2014 12:43:20 PM MDT
  \copyright 2012-2014, Jozsef Bakosi.
  \brief     Quinoa's printer
  \details   Quinoa's printer
*/
//******************************************************************************
#ifndef QuinoaPrint_h
#define QuinoaPrint_h

#include <algorithm>

#include <boost/algorithm/string/replace.hpp>

#include <RNGPrint.h>
#include <DiffEq.h>
#include <Quinoa/Types.h>
#include <Quinoa/Options/DiffEq.h>
#include <Quinoa/InputDeck/InputDeck.h>

namespace quinoa {

extern ctr::InputDeck g_inputdeck_defaults;
extern ctr::InputDeck g_inputdeck;

//! QuinoaPrint : RNGPrint
class QuinoaPrint : public tk::RNGPrint {

  public:
    //! Constructor
    explicit QuinoaPrint( std::ostream& str = std::clog,
                          std::ostream& qstr = std::cout ) :
      RNGPrint( str, qstr ) {}

    //! Print control option: 'group : option' only if differs from its default
    template< typename Option, typename... tags >
    void Section() const {
      if (g_inputdeck.get< tags... >() !=
            g_inputdeck_defaults.get< tags... >() ) {
        Option opt;
        auto& group = opt.group();
        auto& value = opt.name( g_inputdeck.get< tags... >() );
        m_stream << m_section_title_value_fmt % m_section_indent
                                              % m_section_bullet
                                              % group
                                              % value;
        m_stream << m_section_underline_fmt
                    % m_section_indent
                    % std::string( m_section_indent.size() + 3 +
                                   group.size() + value.size(), '-' );
      }
    }

    //! Print item: 'name : value' only if differs from its default
    template<typename... tags>
    void Item(const std::string& name) const {
      if (g_inputdeck.get< tags... >() !=
            g_inputdeck_defaults.get< tags... >() )
        m_stream << m_item_name_value_fmt
                    % m_item_indent % name % g_inputdeck.get<tags...>();
    }

    //! Print control option: 'group : option' only if differs from its default
    template<typename Option, typename... tags>
    void Item() const {
      if (g_inputdeck.get< tags... >() !=
            g_inputdeck_defaults.get< tags... >() ) {
        Option opt;
        m_stream << m_item_name_value_fmt
                    % m_item_indent % opt.group()
                    % opt.name( g_inputdeck.get< tags... >() );
      }
    }

    //! Print list: name: option names with policies
    template< class Factory >
    void eqlist( const std::string& title,
                 const Factory& factory,
                 std::size_t ntypes ) const {
      if (!factory.empty()) {
        section( title );
        item( "Unique equation types", ntypes );
        item( "With all policy combinations", factory.size() );
        raw( '\n' );
        raw( m_item_indent + "Legend: equation name : "
                             "i: initialization policy, " +
                             "c: coefficients policy\n\n" );
        std::string oldeqname;
        for (const auto& f : factory) {
          std::vector< std::string > entries = DiffEqPolicyNames( f.first );
          std::stringstream ss;
          for (const auto& e : entries) ss << e << " ";
          const auto eqname = DiffEqName( f.first );
          if ( oldeqname != eqname )
            m_stream << m_item_name_value_fmt % m_item_indent % eqname % ss.str();
          else
            m_stream << m_item_name_value_fmt % m_item_indent % "" % ss.str();
          oldeqname = eqname;
        }
      }
    }

    //! Print time integration header
    void inthead( const std::string& title, const std::string& name,
                  const std::string& legend, const std::string& head ) const;

    //! Print statistics and PDFs
    void statistics( const std::string& title ) const;

    //! Print configuration of a stack of differential equations
    void diffeqs( const std::string& title,
      const std::vector< std::vector< std::pair< std::string, std::string > > >&
        info ) const;

  private:
    //! Return differential equation name
    template< class Key >
    std::string DiffEqName ( const Key& key ) const
    { return ctr::DiffEq().name( key.template get< tag::diffeq >() ); }

    //! Extract differential equation policy names from diff eq key
    template< class Key >
    std::vector< std::string > DiffEqPolicyNames( const Key& key ) const {
      std::vector< std::string > names;
      names.emplace_back(
        std::string("i:") +
        ctr::InitPolicy().name( key.template get< tag::initpolicy >() ) + ',' );
      names.emplace_back(
        std::string("c:") +
        ctr::CoeffPolicy().name( key.template get< tag::coeffpolicy >() ) );
      return names;
    }

    //! Echo statistics container contents if differs from default applying op
    void stats( const std::string& msg, std::function< std::ostream& (
      std::ostream&, const std::vector< ctr::Term >& ) > op ) const;

    //! Echo pdfs container contents if differs from default applying op
    void pdfs( const std::string& msg,
               std::function<
                 std::ostream& ( std::ostream&,
                                 const std::vector< ctr::Term >&,
                                 const std::vector< tk::real >&,
                                 const std::string&,
                                 const std::vector< tk::real >& ) > op ) const;
};

} // quinoa::

#endif // QuinoaPrint_h
