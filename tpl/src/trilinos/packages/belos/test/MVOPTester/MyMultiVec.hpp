/*
//@HEADER
// ************************************************************************
//
//                 Belos: Block Linear Solvers Package
//                  Copyright 2004 Sandia Corporation
//
// Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY SANDIA CORPORATION "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SANDIA CORPORATION OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Michael A. Heroux (maherou@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#ifndef MY_MULTIVECTOR_HPP
#define MY_MULTIVECTOR_HPP

#include "BelosConfigDefs.hpp"
#include "BelosMultiVec.hpp"

//! Simple example of a user's defined Belos::MultiVec class.
/*! 
 * This is a simple, single processor example of user's defined
 * MultiVec-derived class. The class is templated with ScalarType;
 * possible choices are, for example, "float", "double", or
 * "std::complex<double>".
 *
 * \author Oscar Chinallato (ETHZ/ICOS) and Marzio Sala (ETHZ/COLAB) 
 *
 * \date Last modified on 01-Nov-05
 */
template <class ScalarType>
class MyMultiVec : public Belos::MultiVec<ScalarType>
{
public:

  //! Constructor for a \c NumberVecs vectors of length \c Length.
  MyMultiVec(const int Length, const int NumberVecs) :
    Length_(Length),
    NumberVecs_(NumberVecs)
  {
    Check();

    data_.resize(NumberVecs);
    ownership_.resize(NumberVecs);
    
    // Allocates memory to store the vectors.
    for (int v = 0 ; v < NumberVecs_ ; ++v)
    {
      data_[v] = new ScalarType[Length];
      ownership_[v] = true;
    }

    // Initializes all elements to zero.
    MvInit(0.0);
  }
  
  //! Constructor with already allocated memory
  MyMultiVec(const int Length, const std::vector<ScalarType*>& rhs) :
    Length_(Length),
    NumberVecs_(rhs.size())
  {
    Check();

    data_.resize(NumberVecs_);
    ownership_.resize(NumberVecs_);
    
    // Copies pointers from input array, set ownership so that
    // this memory is not free'd in the destructor
    for (int v = 0 ; v < NumberVecs_ ; ++v)
    {
      data_[v] = rhs[v];
      ownership_[v] = false;
    }
  }
  
  //! Copy constructor, performs a deep copy.
  MyMultiVec(const MyMultiVec& rhs) :
    Length_(rhs.GetVecLength()),
    NumberVecs_(rhs.NumberVecs_)
  {
    Check();

    data_.resize(NumberVecs_);
    ownership_.resize(NumberVecs_);
    
    for (int v = 0 ; v < NumberVecs_ ; ++v)
    {
      data_[v] = new ScalarType[Length_];
      ownership_[v] = true;
    }
    
    for (int v = 0 ; v < NumberVecs_ ; ++v)
    {
      for (int i = 0 ; i < Length_ ; ++i)
        (*this)(i, v) = rhs(i, v);
    }
  }
  
  //! Destructor
  ~MyMultiVec()
  {
    for (int v = 0 ; v < NumberVecs_ ; ++v)
      if (ownership_[v]) 
        delete[] data_[v];
  }
  
  //! Returns a clone of the current std::vector.
  MyMultiVec* Clone(const int NumberVecs) const
  {
    // FIXME
    MyMultiVec* tmp = new MyMultiVec(Length_, NumberVecs);
    
    //   for (int v = 0 ; v < NumberVecs ; ++v)
    //         for (int i = 0 ; i < Length_ ; ++i)
    //           (*tmp)(i, v) = (*this)(i, v);
    
    return(tmp);
  }
  
  // Returns a clone of the corrent multi-std::vector.
  MyMultiVec* CloneCopy() const
  {
    return(new MyMultiVec(*this));
  }
  
  //! Returns a clone copy of specified vectors.
  MyMultiVec* CloneCopy(const std::vector< int > &index) const
  {
    int size = index.size();
    MyMultiVec* tmp = new MyMultiVec(Length_, size);
    
    for (unsigned int v = 0 ; v < index.size() ; ++v) {
      for (int i = 0 ; i < Length_ ; ++i) {
        (*tmp)(i, v) = (*this)(i, index[v]);
      }
    }
    
    return(tmp);
  }
  
  //! Returns a view of current std::vector (shallow copy)
  MyMultiVec* CloneViewNonConst(const std::vector< int > &index) 
  {
    int size = index.size();
    std::vector<ScalarType*> values(size);
    
    for (unsigned int v = 0 ; v < index.size() ; ++v)
      values[v] = data_[index[v]];
    
    return(new MyMultiVec(Length_, values));
  }
  
  //! Returns a view of current std::vector (shallow copy), const version.
  const MyMultiVec* CloneView(const std::vector< int > &index) const
  {
    int size = index.size();
    std::vector<ScalarType*> values(size);
    
    for (unsigned int v = 0 ; v < index.size() ; ++v)
      values[v] = data_[index[v]];
    
    return(new MyMultiVec(Length_, values));
  }
  
  int GetVecLength () const
  {
    return(Length_);
  }
  
  int GetNumberVecs () const
  {
    return(NumberVecs_);
  }
  
  // Update *this with alpha * A * B + beta * (*this). 
  void MvTimesMatAddMv (const ScalarType alpha, const Belos::MultiVec<ScalarType> &A, 
                        const Teuchos::SerialDenseMatrix<int, ScalarType> &B, 
                        const ScalarType beta)
  {
    
    assert (Length_ == A.GetVecLength());
    assert (B.numRows() == A.GetNumberVecs());
    assert (B.numCols() <= NumberVecs_);

    MyMultiVec* MyA;
    MyA = dynamic_cast<MyMultiVec*>(&const_cast<Belos::MultiVec<ScalarType> &>(A)); 
    assert(MyA!=NULL);

    if ((*this)[0] == (*MyA)[0]) {
      // If this == A, then need additional storage ...
      // This situation is a bit peculiar but it may be required by
      // certain algorithms.
      
      std::vector<ScalarType> tmp(NumberVecs_);

      for (int i = 0 ; i < Length_ ; ++i) {
        for (int v = 0; v < A.GetNumberVecs() ; ++v) {
          tmp[v] = (*MyA)(i, v);
        }

        for (int v = 0 ; v < B.numCols() ; ++v) {
          (*this)(i, v) *= beta; 
          ScalarType res = Teuchos::ScalarTraits<ScalarType>::zero();

          for (int j = 0 ; j < A.GetNumberVecs() ; ++j) {
            res +=  tmp[j] * B(j, v);
          }

          (*this)(i, v) += alpha * res;
        }
      }
    }
    else {
      for (int i = 0 ; i < Length_ ; ++i) {
        for (int v = 0 ; v < B.numCols() ; ++v) {
          (*this)(i, v) *= beta; 
          ScalarType res = 0.0;
          for (int j = 0 ; j < A.GetNumberVecs() ; ++j) {
            res +=  (*MyA)(i, j) * B(j, v);
          }

          (*this)(i, v) += alpha * res;
        }
      }
    }
  }
  
  // Replace *this with alpha * A + beta * B. 
  void MvAddMv (const ScalarType alpha, const Belos::MultiVec<ScalarType>& A, 
                const ScalarType beta,  const Belos::MultiVec<ScalarType>& B)
  {
    MyMultiVec* MyA;
    MyA = dynamic_cast<MyMultiVec*>(&const_cast<Belos::MultiVec<ScalarType> &>(A)); 
    assert (MyA != 0);
    
    MyMultiVec* MyB;
    MyB = dynamic_cast<MyMultiVec*>(&const_cast<Belos::MultiVec<ScalarType> &>(B)); 
    assert (MyB != 0);
    
    assert (NumberVecs_ == A.GetNumberVecs());
    assert (NumberVecs_ == B.GetNumberVecs());
    
    assert (Length_ == A.GetVecLength());
    assert (Length_ == B.GetVecLength());
    
    for (int v = 0 ; v < NumberVecs_ ; ++v) {
      for (int i = 0 ; i < Length_ ; ++i) {
        (*this)(i, v) = alpha * (*MyA)(i, v) + beta * (*MyB)(i, v);
      }
    }
  }
  
  // Replace each element of the vectors in *this with (*this)*alpha.
  void MvScale (const ScalarType alpha)
  {
    for (int v = 0 ; v < NumberVecs_ ; ++v) {
      for (int i = 0 ; i < Length_ ; ++i) {
        (*this)(i, v) *= alpha;
      }
    }
  }
  
  // Replace each element of the vectors in *this with (*this)*alpha[i].
  void MvScale (const std::vector<ScalarType>& alpha)
  {
    assert((int)alpha.size() >= NumberVecs_);
    for (int v = 0 ; v < NumberVecs_ ; ++v) {
      for (int i = 0 ; i < Length_ ; ++i) {
        (*this)(i, v) *= alpha[v];
      }
    }
  }
  
  // Compute a dense matrix B through the matrix-matrix multiply alpha * A^H * (*this). 
  void MvTransMv (const ScalarType alpha, const Belos::MultiVec<ScalarType>& A, 
                  Teuchos::SerialDenseMatrix< int, ScalarType >& B) const
  {
    MyMultiVec* MyA;
    MyA = dynamic_cast<MyMultiVec*>(&const_cast<Belos::MultiVec<ScalarType> &>(A)); 
    assert (MyA != 0);
    
    assert (A.GetVecLength() == Length_);
    assert (NumberVecs_ <= B.numCols());
    assert (A.GetNumberVecs() <= B.numRows());
    
      for (int v = 0 ; v < A.GetNumberVecs() ; ++v) {
        for (int w = 0 ; w < NumberVecs_ ; ++w) {
          ScalarType value = 0.0;
          for (int i = 0 ; i < Length_ ; ++i) {
            value += Teuchos::ScalarTraits<ScalarType>::conjugate((*MyA)(i, v)) * (*this)(i, w);
          }
          B(v, w) = alpha * value;
        }
      }
  }
  
  
  // Compute a std::vector b where the components are the individual dot-products, i.e.b[i] = A[i]^H*this[i] where A[i] is the i-th column of A. 
  void MvDot (const Belos::MultiVec<ScalarType>& A, std::vector<ScalarType>& b) const
  {
    MyMultiVec* MyA;
    MyA = dynamic_cast<MyMultiVec*>(&const_cast<Belos::MultiVec<ScalarType> &>(A)); 
    assert (MyA != 0);
    
    assert (NumberVecs_ <= (int)b.size());
    assert (NumberVecs_ == A.GetNumberVecs());
    assert (Length_ == A.GetVecLength());
    
      for (int v = 0 ; v < NumberVecs_ ; ++v) {
        ScalarType value = 0.0;
        for (int i = 0 ; i < Length_ ; ++i) {
          value += (*this)(i, v) * Teuchos::ScalarTraits<ScalarType>::conjugate((*MyA)(i, v));
        }
        b[v] = value;
      }
  }  
  
  
  void MvNorm ( std::vector<typename Teuchos::ScalarTraits<ScalarType>::magnitudeType>& normvec,
		Belos::NormType type = Belos::TwoNorm ) const 
  {
    assert (NumberVecs_ <= (int)normvec.size());

    typedef typename Teuchos::ScalarTraits<ScalarType>::magnitudeType MagnitudeType;
    
    for (int v = 0 ; v < NumberVecs_ ; ++v) {
      MagnitudeType value = Teuchos::ScalarTraits<MagnitudeType>::zero();
      for (int i = 0 ; i < Length_ ; ++i) {
        MagnitudeType val = Teuchos::ScalarTraits<ScalarType>::magnitude((*this)(i, v));
        value += val * val;
      }
      normvec[v] = Teuchos::ScalarTraits<MagnitudeType>::squareroot(value);
    }
  }
  
  // Copy the vectors in A to a set of vectors in *this. The numvecs vectors in 
  // A are copied to a subset of vectors in *this indicated by the indices given 
  // in index.
  // FIXME: not so clear what the size of A and index.size() are...
  void SetBlock (const Belos::MultiVec<ScalarType>& A, 
                 const std::vector<int> &index)
  {
    MyMultiVec* MyA;
    MyA = dynamic_cast<MyMultiVec*>(&const_cast<Belos::MultiVec<ScalarType> &>(A)); 
    assert (MyA != 0);
    
    assert (A.GetNumberVecs() >= (int)index.size());
    assert (A.GetVecLength() == Length_);
    
    for (unsigned int v = 0 ; v < index.size() ; ++v) {
      for (int i = 0 ; i < Length_ ; ++i) {
        (*this)(i, index[v])  = (*MyA)(i, v);
      }
    }
  }
  
  // Fill the vectors in *this with random numbers.
  void MvRandom ()
  {
    for (int v = 0 ; v < NumberVecs_ ; ++v) {
      for (int i = 0 ; i < Length_ ; ++i) {
        (*this)(i, v) = Teuchos::ScalarTraits<ScalarType>::random();
      }
    }
  }
  
  // Replace each element of the vectors in *this with alpha.
  void MvInit (const ScalarType alpha)
  {
    for (int v = 0 ; v < NumberVecs_ ; ++v) {
      for (int i = 0 ; i < Length_ ; ++i) {
        (*this)(i, v) = alpha;
      }
    }
  }
  
  void MvPrint (std::ostream &os) const
  {
    os << "Object MyMultiVec" << std::endl;
    os << "Number of rows = " << Length_ << std::endl;
    os << "Number of vecs = " << NumberVecs_ << std::endl;
    
    for (int i = 0 ; i < Length_ ; ++i)
      {
        for (int v = 0 ; v < NumberVecs_ ; ++v)
          os << (*this)(i, v) << " ";
        os << std::endl;
      }
  }
  
  inline ScalarType& operator()(const int i, const int j)
  {
    if (j < 0 || j >= NumberVecs_) throw(-1);
    if (i < 0 || i >= Length_) throw(-2);
    // 
    return(data_[j][i]);
  }
  
  inline const ScalarType& operator()(const int i, const int j) const
  {
    if (j < 0 || j >= NumberVecs_) throw(-1);
    if (i < 0 || i >= Length_) throw(-2);
    // 
    return(data_[j][i]);
  }
  
  ScalarType* operator[](int v)
  {
    if (v < 0 || v >= NumberVecs_) throw(-1);
    return(data_[v]);
  }
  
  ScalarType* operator[](int v) const
  {
    return(data_[v]);
  }
  
private:
  void Check()
  {
    if (Length_ <= 0)
      throw("Length must be positive");

    if (NumberVecs_ <= 0)
      throw("Number of vectors must be positive");
  }

  //! Length of the vectors
  const int Length_;
  //! Number of multi-vectors
  const int NumberVecs_;
  //! Pointers to the storage of the vectors.
  std::vector<ScalarType*> data_;
  //! If \c true, then this object owns the vectors and must free them in dtor.
  std::vector<bool> ownership_;

};


#endif // MY_MULTIVECTOR_HPP