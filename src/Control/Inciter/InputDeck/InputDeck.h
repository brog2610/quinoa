//******************************************************************************
/*!
  \file      src/Control/Inciter/InputDeck/InputDeck.h
  \author    J. Bakosi
  \date      Mon 23 Feb 2015 08:58:02 AM MST
  \copyright 2012-2015, Jozsef Bakosi.
  \brief     Inciter's input deck definition
  \details   This file defines the heterogeneous stack that is used for storing
     the data from user input during the control file parsing of the
     computational shock hydrodynamics tool, Inciter.
*/
//******************************************************************************
#ifndef InciterInputDeck_h
#define InciterInputDeck_h

#include <limits>

#include <boost/mpl/set.hpp>
#include <boost/mpl/for_each.hpp>

#include <Control.h>
#include <Inciter/CmdLine/CmdLine.h>
#include <Inciter/Components.h>

namespace inciter {
namespace ctr {

//! \brief InputDeck : Control< specialized to Inciter >, see Types.h,
//! \details The stack is a tagged tuple, a hierarchical heterogeneous data
//!    structure where all parsed information is stored.
//! \see Base/TaggedTuple.h
//! \see Control/Inciter/Types.h
//! \author J. Bakosi
class InputDeck :
  public tk::Control< // tag           type
                      tag::title,      kw::title::info::expect::type,
                      tag::cmd,        CmdLine,
                      tag::error,      std::vector< std::string > > {

  public:
    //! \brief Inciter input deck keywords
    //! \details Since there are more than 20 and boost::mpl only allows maxium
    //!   20 items in a set by default (and I don't want to mess with
    //!   preprocessor-generated boost::mpl headers), the whole set is broken up
    //!   into several sets each containing 20 keywords.
    //! \see tk::grm::use and its documentation
    using keywords = boost::mpl::set< kw::title >;
                                     
    //! \brief Constructor: set defaults
    //! \details Anything not set here is initialized by the compiler using the
    //!   default constructor for the corresponding type.
    //! \author J. Bakosi
    InputDeck() {
      // Initialize help: fill own keywords
      const auto& ctrinfoFill = tk::ctr::Info( get< tag::cmd, tag::ctrinfo >() );
      boost::mpl::for_each< keywords >( ctrinfoFill );
    }

    /** @name Pack/Unpack: Serialize InputDeck object for Charm++ */
    ///@{
    //! \brief Pack/Unpack serialize member function
    //! \param[inout] p Charm++'s PUP::er serializer object reference
    //! \author J. Bakosi
    void pup( PUP::er& p ) {
      tk::Control< tag::title,      kw::title::info::expect::type,
                   tag::cmd,        CmdLine,
                   tag::error,      std::vector< std::string > >::pup(p);
    }
    //! \brief Pack/Unpack serialize operator|
    //! \param[inout] p Charm++'s PUP::er serializer object reference
    //! \param[inout] i InputDeck object reference
    //! \author J. Bakosi
    friend void operator|( PUP::er& p, InputDeck& i ) { i.pup(p); }
    //@}
};

} // ctr::
} // inciter::

#endif // InciterInputDeck_h
