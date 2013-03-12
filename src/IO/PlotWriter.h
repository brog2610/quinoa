//******************************************************************************
/*!
  \file      src/IO/PlotWriter.h
  \author    J. Bakosi
  \date      Mon 11 Mar 2013 06:59:07 PM MDT
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     PlotWriter base class declaration
  \details   PlotWriter base class declaration
*/
//******************************************************************************
#ifndef PlotWriter_h
#define PlotWriter_h

#include <string>
#include <fstream>

#include <QuinoaTypes.h>

using namespace std;

namespace Quinoa {

//! PlotWriter base class
class PlotWriter {

  public:
    //! Constructor: Acquire plot file handle
    PlotWriter(const string& filename);

    //! Destructor: Release plot file handle
    virtual ~PlotWriter();

  protected:
    //! Interface for plot write
    virtual void write(const int it, const real t) = 0;

    const string m_filename;    //!< Plot file name
    ofstream m_outPlot;         //!< Plot file output stream

  private:
    //! Don't permit copy constructor
    PlotWriter(const PlotWriter&) = delete;
    //! Don't permit copy assigment
    PlotWriter& operator=(const PlotWriter&) = delete;
    //! Don't permit move constructor
    PlotWriter(PlotWriter&&) = delete;
    //! Don't permit move assigment
    PlotWriter& operator=(PlotWriter&&) = delete;
};

} // namespace Quinoa

#endif // PlotWriter_h
