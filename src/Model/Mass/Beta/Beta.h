//******************************************************************************
/*!
  \file      src/Model/Mass/Beta/Beta.h
  \author    J. Bakosi
  \date      Fri May 24 13:38:45 2013
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Beta mass model
  \details   Beta mass model
*/
//******************************************************************************
#ifndef Beta_h
#define Beta_h

#include <Mass.h>

namespace Quinoa {

class Memory;
class Paradigm;
class JPDF;

//! Beta : Mass<Beta> child for CRTP
//! See: http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern
class Beta : public Mass<Beta> {

  public:
    //! Constructor
    explicit Beta(Memory* const memory,
                  Paradigm* const paradigm,
                  Control* const control,
                  real* const particles) :
      Mass<Beta>(memory, paradigm, control, particles),
      m_At(control->get<control::AT>()) {
      // ErrChk on m_At
    }

    //! Destructor
    virtual ~Beta() noexcept = default;

    //! Return mass model identification
    control::MassType id() noexcept { return control::BETA; }

    //! Initialize particles
    void init();

    //! Advance particles
    void advance(const real& dt);

    //! Estimate joint scalar PDF
    void jpdf(JPDF& jpdf);

  private:
    //! Don't permit copy constructor
    Beta(const Beta&) = delete;
    //! Don't permit copy assigment
    Beta& operator=(const Beta&) = delete;
    //! Don't permit move constructor
    Beta(Beta&&) = delete;
    //! Don't permit move assigment
    Beta& operator=(Beta&&) = delete;

    const real m_At;            //!< Atwood-number
};

} // namespace Quinoa

#endif // Beta_h
