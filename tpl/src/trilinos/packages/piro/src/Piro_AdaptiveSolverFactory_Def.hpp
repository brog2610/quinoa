// @HEADER
// ************************************************************************
//
//        Piro: Strategy package for embedded analysis capabilitites
//                  Copyright (2010) Sandia Corporation
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
// Questions? Contact Glen Hansen (gahanse@sandia.gov), Sandia
// National Laboratories.
//
// ************************************************************************
// @HEADER

#include "Piro_ConfigDefs.hpp"

#ifdef HAVE_PIRO_NOX
#include "Piro_NOXSolver.hpp"
#include "Piro_LOCAAdaptiveSolver.hpp"
#endif /* HAVE_PIRO_NOX */

#ifdef HAVE_PIRO_RYTHMOS
// This "define" turns on the extended template interface in RythmosSolver. This should be cleaned up at some
// point.
#define ALBANY_BUILD
#include "Piro_RythmosSolver.hpp"
#endif /* HAVE_PIRO_RYTHMOS */

#include "Teuchos_TestForException.hpp"

#include <string>
#include <stdexcept>

namespace Piro {

template <typename Scalar, typename LocalOrdinal, typename GlobalOrdinal, typename Node>
Teuchos::RCP<Thyra::ResponseOnlyModelEvaluatorBase<Scalar> > AdaptiveSolverFactory::createSolver(
    const Teuchos::RCP<Teuchos::ParameterList> &piroParams,
    const Teuchos::RCP<Thyra::ModelEvaluator<Scalar> > &model,
    const Teuchos::RCP<LOCA::Thyra::AdaptiveSolutionManager> &solMgr,
    const Teuchos::RCP<Piro::ObserverBase<Scalar> > &observer)
{
  Teuchos::RCP<Thyra::ResponseOnlyModelEvaluatorBase<Scalar> > result;

  const std::string &solverType = piroParams->get("Solver Type", "NOX");

#ifdef HAVE_PIRO_NOX
  if (solverType == "NOX") {
    result = Teuchos::rcp(new NOXSolver<Scalar>(piroParams, model, observer));
  } else
  if (solverType == "LOCA") {
    result = observedLocaSolver(piroParams, model, solMgr, observer);
  } else
#endif /* HAVE_PIRO_NOX */
#ifdef HAVE_PIRO_RYTHMOS
  if (solverType == "Rythmos") {
    result = rythmosSolver<Scalar, LocalOrdinal, GlobalOrdinal, Node>(piroParams, model, observer);
  } else
#endif /* HAVE_PIRO_RYTHMOS */
  {
    TEUCHOS_TEST_FOR_EXCEPTION(
        true,
        std::runtime_error,
        "Error: Unknown Piro Solver Type: " << solverType);
  }

  return result;
}

} // namespace Piro
