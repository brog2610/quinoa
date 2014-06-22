//******************************************************************************
/*!
  \file      src/RNGTest/Crush.h
  \author    J. Bakosi
  \date      Mon 16 Jun 2014 11:20:07 AM MDT
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Crush battery
  \details   Crush battery
*/
//******************************************************************************
#ifndef Crush_h
#define Crush_h

#include <Option.h>
#include <Battery.h>
#include <StatTest.h>
#include <testu01suite.decl.h>

namespace rngtest {

//! Crush
class Crush {

  public:
    //! Return string identifying test suite name
    const std::string& name() const { return
      tk::Option< ctr::Battery >().name( rngtest::ctr::BatteryType::CRUSH );
    }

    //! Add statistical tests to battery
    void addTests( std::vector< std::function< StatTest() > >& tests,
                   tk::ctr::RNGType rng,
                   CProxy_TestU01Suite& proxy );
};

} // rngtest::

#endif // Crush_h
