//******************************************************************************
/*!
  \file      src/LinearAlgebra/SparseMatrix.h
  \author    J. Bakosi
  \date      Wed 17 Oct 2012 07:30:50 PM MDT
  \copyright Copyright 2005-2012, Jozsef Bakosi, All rights reserved.
  \brief     Sparse matrix declaration
  \details   Sparse matrix base class declaration
*/
//******************************************************************************
#ifndef SparseMatrix_h
#define SparseMatrix_h

#include <string>

using namespace std;

#include <QuinoaTypes.h>
#include <Memory.h>

namespace Quinoa {

//! Sparse matrix base class
class SparseMatrix {

  protected:
    //! Constructor
    SparseMatrix(string name, Int size, Int dof) :
      m_name(name), m_size(size), m_rsize(size*dof), m_dof(dof) {}

    //! Destructor
    virtual ~SparseMatrix() = 0;

    Memory *m_memory;  //!< Local copy of the memory store pointer
    string m_name;     //!< Name of the sparse matrix instance
    Int m_size;        //!< Size of matrix: (dof x size) x (dof x size)
    Int m_rsize;       //!< Width of matrix: dof x size
    Int m_dof;         //!< Number of degrees of freedom
    Int m_nnz;         //!< Total number of nonzeros

  private:
    //! Don't permit copy constructor
    SparseMatrix(const SparseMatrix&) = delete;
    //! Don't permit copy assigment
    SparseMatrix& operator=(const SparseMatrix&) = delete;
    //! Don't permit move constructor
    SparseMatrix(SparseMatrix&&) = delete;
    //! Don't permit move assigment
    SparseMatrix& operator=(SparseMatrix&&) = delete;
};

} // namespace Quinoa

#endif // SparseMatrix_h
