//******************************************************************************
/*!
  \file      src/Model/Mix/Mix.h
  \author    J. Bakosi
  \date      Sat 23 Feb 2013 09:07:48 AM MST
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Mix model base
  \details   Mix mode lbase
*/
//******************************************************************************
#ifndef Mix_h
#define Mix_h

#include <string>

#include <QuinoaTypes.h>
#include <Model.h>
#include <JPDF.h>

namespace Quinoa {

using namespace std;

//! Mix model base
class Mix : public Model {

  public:
    //! Constructor
    Mix(Memory* const memory,
        Paradigm* const paradigm,
        Control* const control,
        const string& name);

    //! Destructor
    virtual ~Mix() {}

    //! Interface for initializing particles
    virtual void init() = 0;

    //! Interface for advancing particles in mix model
    virtual void advance(const real dt) = 0;

    //! Interface for echo information on mix model
    virtual void echo() = 0;

    //! Interface for estimate joint scalar PDF
    virtual void jpdf(JPDF& jpdf) = 0;

  protected:
    const int m_nscalar;            //!< Number of mixing scalars
    const int m_npar;               //!< Number of particles

  private:
    //! Don't permit copy constructor
    Mix(const Mix&) = delete;
    //! Don't permit copy assigment
    Mix& operator=(const Mix&) = delete;
    //! Don't permit move constructor
    Mix(Mix&&) = delete;
    //! Don't permit move assigment
    Mix& operator=(Mix&&) = delete;
};

} // namespace Quinoa

#endif // Mix_h
