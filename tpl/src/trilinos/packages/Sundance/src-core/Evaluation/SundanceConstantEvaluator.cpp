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

#include "SundanceSubtypeEvaluator.hpp"
#include "SundanceEvalManager.hpp"
#include "SundanceSpatiallyConstantExpr.hpp"
#include "SundanceSet.hpp"
#include "PlayaTabs.hpp"
#include "SundanceOut.hpp"

using namespace Sundance;
using namespace Sundance;

using namespace Sundance;
using namespace Teuchos;




ConstantEvaluator::ConstantEvaluator(const SpatiallyConstantExpr* expr, 
                                     const EvalContext& context)
  : SubtypeEvaluator<SpatiallyConstantExpr>(expr, context)
{
  Tabs tab;
  SUNDANCE_MSG1(context.setupVerbosity(), tab << "in ConstantEvaluator ctor");
  /*
   * There is only one possible nonzero derivative of this expression: the
   * zeroth-order derivative. 
   *
   * There's nothing to do in this ctor other than running some sanity checks.
   */

  TEUCHOS_TEST_FOR_EXCEPTION(this->sparsity()->numDerivs() > 1, std::logic_error,
                     "ConstantEvaluator ctor found a sparsity table "
                     "without more than one entry. The bad sparsity table is "
                     << *(this->sparsity)());

  if (this->sparsity()->numDerivs() > 0)
    {
      const MultipleDeriv& d = this->sparsity()->deriv(0);

      TEUCHOS_TEST_FOR_EXCEPTION(d.order() != 0, std::logic_error,
                         "ConstantEvaluator ctor found a nonzero derivative "
                         "of order greater than zero. The bad sparsity "
                         "table is " << *(this->sparsity)());
      addConstantIndex(0,0);
    }
}





void ConstantEvaluator::internalEval(const EvalManager& mgr,
  Array<double>& constantResults,
  Array<RCP<EvalVector> >& vectorResults) const 
{
  Tabs tabs;
  SUNDANCE_MSG1(mgr.verb(), tabs << "ConstantEvaluator::eval() expr="
    << expr()->toString());

  if (this->sparsity()->numDerivs() > 0)
  {
    constantResults.resize(1);
    constantResults[0] = expr()->value();
    
    SUNDANCE_MSG2(mgr.verb(), tabs << "result=" << constantResults[0]);
  }
  else
  {
    SUNDANCE_MSG2(mgr.verb(), tabs << "no results requested");
  }
}