// @HEADER
//
// ***********************************************************************
//
//                MueLu: A package for multigrid based preconditioning
//                                      Copyright 2012 Sandia Corporation
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
// Questions? Contact
//                                        Jonathan Hu           (jhu@sandia.gov)
//                                        Andrey Prokopenko (aprokop@sandia.gov)
//                                        Ray Tuminaro          (rstumin@sandia.gov)
//
// ***********************************************************************
//
// @HEADER

#ifndef MUELU_MATLABUTILS_DECL_HPP
#define MUELU_MATLABUTILS_DECL_HPP

#include <string>
#include <complex>
#include <Teuchos_ParameterList.hpp>
#include <Teuchos_RCP.hpp>
#include <Teuchos_DefaultComm.hpp>
#include "MueLu_ConfigDefs.hpp"

#if !defined(HAVE_MUELU_MATLAB) || !defined(HAVE_MUELU_EPETRA) || !defined(HAVE_MUELU_TPETRA)
#error "Muemex types require MATLAB, Epetra and Tpetra."
#else
#include "mex.h"
#include "MueLu_Hierarchy_decl.hpp"
#include "MueLu_Aggregates_decl.hpp"
#include "MueLu_AmalgamationInfo_decl.hpp"
#include "Epetra_MultiVector.h"
#include "Epetra_CrsMatrix.h"
#include "Tpetra_CrsMatrix_decl.hpp"
#include "Xpetra_EpetraCrsMatrix.hpp"

namespace MueLu {
//Useful global typedefs for MueMex

enum MUEMEX_TYPE
  {
    INT,
    DOUBLE,
    STRING,
    COMPLEX,
    XPETRA_ORDINAL_VECTOR,
    TPETRA_MULTIVECTOR_DOUBLE,
    TPETRA_MULTIVECTOR_COMPLEX,
    TPETRA_MATRIX_DOUBLE,
    TPETRA_MATRIX_COMPLEX,
    XPETRA_MATRIX_DOUBLE,
    XPETRA_MATRIX_COMPLEX,
    XPETRA_MULTIVECTOR_DOUBLE,
    XPETRA_MULTIVECTOR_COMPLEX,
    EPETRA_CRSMATRIX,
    EPETRA_MULTIVECTOR,
    AGGREGATES,
    AMALGAMATION_INFO
  };

typedef Kokkos::Compat::KokkosDeviceWrapperNode<Kokkos::Serial, Kokkos::HostSpace> mm_node_t;
typedef Tpetra::Vector<>::local_ordinal_type mm_LocalOrd;
typedef Tpetra::Vector<>::global_ordinal_type mm_GlobalOrd;
typedef Tpetra::Map<> muemex_map_type;
typedef Tpetra::CrsMatrix<double, mm_LocalOrd, mm_GlobalOrd, mm_node_t> Tpetra_CrsMatrix_double;
typedef std::complex<double> complex_t;
typedef Tpetra::CrsMatrix<complex_t, mm_LocalOrd, mm_GlobalOrd, mm_node_t> Tpetra_CrsMatrix_complex;
typedef Xpetra::Vector<mm_LocalOrd, mm_LocalOrd, mm_GlobalOrd, mm_node_t> Xpetra_ordinal_vector;
typedef Xpetra::Matrix<double, mm_LocalOrd, mm_GlobalOrd, mm_node_t> Xpetra_Matrix_double;
typedef Xpetra::Matrix<complex_t, mm_LocalOrd, mm_GlobalOrd, mm_node_t> Xpetra_Matrix_complex;
typedef Xpetra::MultiVector<double, mm_LocalOrd, mm_GlobalOrd, mm_node_t> Xpetra_MultiVector_double;
typedef Xpetra::MultiVector<complex_t, mm_LocalOrd, mm_GlobalOrd, mm_node_t> Xpetra_MultiVector_complex;
typedef MueLu::Hierarchy<double, mm_LocalOrd, mm_GlobalOrd, mm_node_t> Hierarchy_double;
typedef MueLu::Hierarchy<complex_t, mm_LocalOrd, mm_GlobalOrd, mm_node_t> Hierarchy_complex;
typedef MueLu::Aggregates<> MAggregates;
typedef MueLu::AmalgamationInfo<> MAmalInfo;

/* Static utility functions */
template<typename Scalar>
Teuchos::RCP<Tpetra::MultiVector<Scalar, mm_LocalOrd, mm_GlobalOrd, mm_node_t>> loadTpetraMV(const mxArray* mxa);
//create a sparse array in Matlab
template<typename Scalar>
mxArray* createMatlabSparse(int numRows, int numCols, int nnz);
//create an ordinal (int32) vector in Matlab
mxArray* createMatlabLOVector(Teuchos::RCP<Xpetra_ordinal_vector>& vec);
//copy a sparse Xpetra matrix (double or complex) to Matlab
template<typename Scalar>
mxArray* saveMatrixToMatlab(Teuchos::RCP<Xpetra::Matrix<Scalar, mm_LocalOrd, mm_GlobalOrd, mm_node_t>>& mat);
template<typename Scalar>
mxArray* createMatlabMultiVector(int numRows, int numCols);
template<typename Scalar>
mxArray* saveTpetraMV(Teuchos::RCP<Tpetra::MultiVector<Scalar, mm_LocalOrd, mm_GlobalOrd, mm_node_t>>& mv);
template<typename Scalar>
mxArray* saveMultiVectorToMatlab(Teuchos::RCP<Xpetra::MultiVector<Scalar, mm_LocalOrd, mm_GlobalOrd, mm_node_t>>& mv);
Teuchos::RCP<Xpetra::MultiVector<double, mm_LocalOrd, mm_GlobalOrd, mm_node_t>> loadXpetraMVDouble(const mxArray* mxa);
Teuchos::RCP<Xpetra::MultiVector<complex_t, mm_LocalOrd, mm_GlobalOrd, mm_node_t>> loadXpetraMVComplex(const mxArray* mxa);
template<typename Scalar>
void fillMatlabArray(Scalar* array, const mxArray* mxa, int n);
//set up Tpetra matrix from MATLAB array
template<typename Scalar>
Teuchos::RCP<Tpetra::CrsMatrix<Scalar, mm_LocalOrd, mm_GlobalOrd, mm_node_t>> tpetraLoadMatrix(const mxArray* mxa);
//same as above but for Xpetra
template<typename Scalar>
Teuchos::RCP<Xpetra::Matrix<Scalar, mm_LocalOrd, mm_GlobalOrd, mm_node_t>> xpetraLoadMatrix(const mxArray* mxa);
//Get an Epetra_MultiVector from MATLAB array
Teuchos::RCP<Epetra_MultiVector> loadEpetraMV(const mxArray* mxa);
//Save an Epetra MV to MATLAB array
mxArray* saveEpetraMV(Teuchos::RCP<Epetra_MultiVector>& mv);
//Load an Epetra matrix from MATLAB array
Teuchos::RCP<Epetra_CrsMatrix> epetraLoadMatrix(const mxArray* mxa);
//Get an int from a MATLAB double or int input
int parseInt(const mxArray* mxa);
//Save an Epetra matrix to MATLAB array
mxArray* saveEpetraMatrix(Teuchos::RCP<Epetra_CrsMatrix>& mat);
//Load an ordinal vector
Teuchos::RCP<Xpetra_ordinal_vector> loadLOVector(const mxArray* mxa);
//Int conversion routine
int* mwIndex_to_int(int N, mwIndex* mwi_array);
//Aggregates conversion
Teuchos::RCP<MAggregates> loadAggregates(const mxArray* mxa);
mxArray* saveAggregates(Teuchos::RCP<MAggregates>& agg);
//AmalgamationInfo
Teuchos::RCP<MAmalInfo> loadAmalInfo(const mxArray* mxa);
mxArray* saveAmalInfo(Teuchos::RCP<MAmalInfo>& amal);
//Need an easy way to determine if an mxArray* points to a valid Aggregates struct
bool isValidMatlabAggregates(const mxArray* mxa);

class MuemexArg
{
 public:
  MuemexArg(MUEMEX_TYPE dataType) {type = dataType;}
  MUEMEX_TYPE type;
};

template<typename T> 
MUEMEX_TYPE getMuemexType(const T & data);

template<typename T>
class MuemexData : public MuemexArg
{
 public:
  MuemexData(T& data); //Construct from pre-existing data, to pass to MATLAB.
  MuemexData(T& data, MUEMEX_TYPE type);        //Construct from pre-existing data, to pass to MATLAB.
  MuemexData(const mxArray* mxa); //Construct from MATLAB array, to get from MATLAB.
  mxArray* convertToMatlab(); //Create a MATLAB object and copy this data to it
  T& getData();                         //Set and get methods
  void setData(T& data);
 private:
  T data;
};

//The two callback functions that MueLu can call to run anything in MATLAB
void callMatlabNoArgs(std::string function);
std::vector<Teuchos::RCP<MuemexArg>> callMatlab(std::string function, int numOutputs, std::vector<Teuchos::RCP<MuemexArg>> args);


}//end namespace

#endif //HAVE_MUELU_MATLAB error handler
#endif //MUELU_MATLABUTILS_DECL_HPP guard
