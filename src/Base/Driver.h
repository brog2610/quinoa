//******************************************************************************
/*!
  \file      src/Base/Driver.h
  \author    J. Bakosi
  \date      Wed 07 Nov 2012 08:58:45 PM MST
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Driver base class declaration
  \details   Driver base class declaration
*/
//******************************************************************************
#ifndef Driver_h
#define Driver_h

#include <Memory.h>

namespace Quinoa {

//! Driver base class
class Driver {

  public:
    //! Constructor
    Driver(Memory* memory) : m_memory(memory) {}

    //! Destructor
    ~Driver();

    //! Setup
    void setup(int argc, char* argv[]);

    //! Solve
    void solve();

    //! Finalize (either at normal exit, or due to exception)
    void finalize();

  private:
    //! Don't permit copy constructor
    Driver(const Driver&) = delete;
    //! Don't permit assigment constructor
    Driver& operator=(const Driver&) = delete;
    //! Don't permit move constructor
    Driver(Driver&&) = delete;
    //! Don't permit move assignment
    Driver& operator=(Driver&&) = delete;

    //! Pointer to Memory object
    Memory* m_memory;
};

} // namespace Quinoa

#endif // Driver_h
