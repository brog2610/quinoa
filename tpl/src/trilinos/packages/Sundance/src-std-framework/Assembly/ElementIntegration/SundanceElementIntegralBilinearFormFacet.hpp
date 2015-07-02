/* @HEADER@ */
// ************************************************************************
// 
//                             Sundance
//                 Copyright 2011 Sandia Corporation
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
// Questions? Contact Kevin Long (kevin.long@ttu.edu)
// 

/* @HEADER@ */

#ifndef SUNDANCE_ELEMENTINTEGRALBILINEARFORMFACET_H
#define SUNDANCE_ELEMENTINTEGRALBILINEARFORMFACET_H

#include "SundanceDefs.hpp"
#include "SundanceCellJacobianBatch.hpp"
#include "SundanceQuadratureFamily.hpp"
#include "SundanceBasisFamily.hpp"
#include "Teuchos_Array.hpp"
#include "SundanceElementIntegralBilinearForm.hpp"

#ifndef DOXYGEN_DEVELOPER_ONLY

namespace SundanceStdFwk
{
  using namespace SundanceUtils;
  using namespace SundanceStdMesh;
  using namespace SundanceStdMesh::Internal;
  using namespace SundanceCore;
  using namespace SundanceCore::Internal;
  
  namespace Internal
  {
    using namespace Teuchos;
    
    /** 
     * ElementIntegralBilinearFormFacet provides an abstract base class for
     streaming functional evaluation over a batch of facets of cells.
     It provides a single pure virtual function for subclasses
     to implement.
    */
    class ElementIntegralBilinearFormFacet: public ElementIntegralBilinearForm

    {
    public:
      /** Constructor */
      ElementIntegralBilinearFormFacet( int spatialDim,
					const CellType& maxCellType,
					int dim,
					const CellType& cellType,
					const BasisFamily& testBasis,
					const BasisFamily& unkBasis,
					const QuadratureFamily& quad,
					const ParameterList& verbParams 
					= *ElementIntegralBase::defaultVerbParams()):
	ElementIntegralBilinearForm( spatialDim , maxCellType ,
				     dim , cellType ,
				     testBasis , unkBasis ,
				     quad , verbParams ){;}
      
      /** Destructor */
      virtual ~ElementIntegralBilinearFormFacet() {;}
      
      virtual void evaluate( CellJacobianBatch& JTrans,
			     const CellJacobianBatch& JVol,
			     const Array<int>& facetIndex,
			     const double* const coeff,
			     RefCountPtr<Array<double> >& A) const = 0;
    };
  }
}
#endif  /* DOXYGEN_DEVELOPER_ONLY */

#endif