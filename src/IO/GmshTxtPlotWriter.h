//******************************************************************************
/*!
  \file      src/IO/GmshTxtPlotWriter.h
  \author    J. Bakosi
  \date      Sun 01 Sep 2013 02:19:16 PM MDT
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     GmshTxtPlotWriter class declaration
  \details   GmshTxtPlotWriter class declaration
*/
//******************************************************************************
#ifndef GmshTxtPlotWriter_h
#define GmshTxtPlotWriter_h

#include <string>

#include <Writer.h>

namespace quinoa {

//! GmshTxtPlotWriter : Writer
class GmshTxtPlotWriter : public Writer {

  public:
    //! Constructor: Acquire plot file handle
    explicit GmshTxtPlotWriter(const std::string& filename) :
      Writer(filename) {}

    //! Destructor: Release plot file handle
    ~GmshTxtPlotWriter() noexcept override = default;

  private:
    //! Don't permit copy constructor
    GmshTxtPlotWriter(const GmshTxtPlotWriter&) = delete;
    //! Don't permit copy assigment
    GmshTxtPlotWriter& operator=(const GmshTxtPlotWriter&) = delete;
    //! Don't permit move constructor
    GmshTxtPlotWriter(GmshTxtPlotWriter&&) = delete;
    //! Don't permit move assigment
    GmshTxtPlotWriter& operator=(GmshTxtPlotWriter&&) = delete;
};

} // namespace quinoa

#endif // GmshTxtPlotWriter_h
