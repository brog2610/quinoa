//******************************************************************************
/*!
  \file      src/Base/Base.h
  \author    J. Bakosi
  \date      Thu Nov 14 10:35:16 2013
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Collection of essentials
  \details   Collection of essentials
*/
//******************************************************************************
#ifndef Base_h
#define Base_h

#include <QuinoaPrint.h>
#include <RNGTestPrint.h>
#include <Paradigm.h>
#include <Control.h>
#include <Timer.h>
#include <Driver.h>

namespace quinoa {

//! Base: collection of essentials
struct Base {
  QuinoaPrint& print;
  tk::Paradigm& paradigm;
  ctr::InputDeck& control;
  tk::Timer& timer;
  tk::RNGFactory& rng;

  //! Initializer constructor
  Base(QuinoaPrint& Print,
       tk::Paradigm& Paradigm,
       ctr::InputDeck& Control,
       tk::Timer& Timer,
       tk::RNGFactory& Rng) : print(Print),
                              paradigm(Paradigm),
                              control(Control),
                              timer(Timer),
                              rng(Rng) {}
};

} // quinoa::

namespace rngtest {

//! Base: collection of essentials
struct Base {
  RNGTestPrint& print;
  tk::Paradigm& paradigm;
  ctr::InputDeck& control;
  tk::Timer& timer;
  tk::RNGFactory& rng;

  //! Initializer constructor
  Base(RNGTestPrint& Print,
       tk::Paradigm& Paradigm,
       ctr::InputDeck& Control,
       tk::Timer& Timer,
       tk::RNGFactory& Rng) : print(Print),
                              paradigm(Paradigm),
                              control(Control),
                              timer(Timer),
                              rng(Rng) {}
};

} // rngtest::

#endif // Base_h
