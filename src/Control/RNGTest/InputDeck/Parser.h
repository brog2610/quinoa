//******************************************************************************
/*!
  \file      src/Control/RNGTest/InputDeck/Parser.h
  \author    J. Bakosi
  \date      Fri Oct 18 12:22:58 2013
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Random number generator test suite input deck parser
  \details   Random number generator test suite input deck parser
*/
//******************************************************************************
#ifndef RNGTestInputDeckParser_h
#define RNGTestInputDeckParser_h

#include <FileParser.h>
#include <RNGTest/CmdLine/CmdLine.h>
#include <RNGTest/InputDeck/InputDeck.h>

namespace rngtest {

//! InputDeckParser : FileParser
class InputDeckParser : public tk::FileParser {

  public:
    //! Constructor
    explicit InputDeckParser(const tk::Print& print,
                             const std::unique_ptr< ctr::CmdLine >& cmdline,
                             std::unique_ptr< ctr::InputDeck >& inputdeck);

    //! Destructor
    ~InputDeckParser() noexcept override = default;

  private:
    //! Don't permit copy constructor
    InputDeckParser(const InputDeckParser&) = delete;
    //! Don't permit copy assigment
    InputDeckParser& operator=(const InputDeckParser&) = delete;
    //! Don't permit move constructor
    InputDeckParser(InputDeckParser&&) = delete;
    //! Don't permit move assigment
    InputDeckParser& operator=(InputDeckParser&&) = delete;
};

} // namespace rngtest

#endif // RNGTestInputDeckParser_h
