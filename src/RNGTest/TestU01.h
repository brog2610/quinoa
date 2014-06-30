//******************************************************************************
/*!
  \file      src/RNGTest/TestU01.h
  \author    J. Bakosi
  \date      Mon 30 Jun 2014 06:41:44 AM MDT
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     TestU01 statistical tests
  \details   TestU01 statistical tests
*/
//******************************************************************************
#ifndef TestU01_h
#define TestU01_h

#include <charm++.h>

#include <testu01.decl.h>
#include <TestU01Props.h>

namespace rngtest {

//! TestU01 statistical test used polymorphically with StatTest
template< class TestU01Props >
class TestU01 : public CBase_TestU01< TestU01Props > {

  public:
    using Props = TestU01Props;
    using Proxy = CProxy_TestU01< TestU01Props >;

    //! Constructor
    explicit TestU01( const TestU01Props& props ) : m_props( props ) {}

    //! Run test then evaluate it
    void run() { m_props.proxy().evaluate( m_props.run() ); }

    //! Query and contribute number of results/test, i.e., p-values
    void npval() { m_props.proxy().npval( m_props.npval() ); }

    //! Query and contribute test name(s)
    void names() { m_props.proxy().names( m_props.names() ); }

//     //! RNG enum accessor
//     const tk::ctr::RNGType& rng() const { return m_id; }
// 
//     //! RNG id accessor
//     std::size_t id() const { return 0; }
// 
//     //! Query whether test is failed
//     bool fail( std::size_t p ) const {
//       if ( (m_pvals[p] <= gofw_Suspectp) || (m_pvals[p] >= 1.0-gofw_Suspectp) )
//         return true;
//       else
//         return false;
//     }
// 
//     //! Return number of failed tests
//     std::size_t nfail() const {
//       std::size_t f = 0;
//       for (std::size_t p = 0; p<m_npval; ++p) if (fail(p)) ++f;
//       return f;
//     }

  private:
    TestU01Props m_props;               //!< TestU01 test properties
};

} // rngtest::

#define CK_TEMPLATES_ONLY
#include <testu01.def.h>
#undef CK_TEMPLATES_ONLY

#endif // TestU01_h
