// @HEADER
//
// ***********************************************************************
//
//   Zoltan2: A package of combinatorial algorithms for scientific computing
//                  Copyright 2012 Sandia Corporation
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
// Questions? Contact Karen Devine      (kddevin@sandia.gov)
//                    Erik Boman        (egboman@sandia.gov)
//                    Siva Rajamanickam (srajama@sandia.gov)
//
// ***********************************************************************
//
// @HEADER
#ifndef _ZOLTAN2_ALGNATURAL_HPP_
#define _ZOLTAN2_ALGNATURAL_HPP_

#include <Zoltan2_IdentifierModel.hpp>
#include <Zoltan2_OrderingSolution.hpp>


namespace Zoltan2{

////////////////////////////////////////////////////////////////////////
//! \file Zoltan2_AlgNatural.hpp
//! \brief Natural ordering == identity permutation.
//! \brief Mainly useful for testing "no ordering"

template <typename Adapter>
class AlgNatural
{
  public:

  AlgNatural()
  {
  }

  int order(
    const RCP<IdentifierModel<Adapter> > &model, 
    const RCP<OrderingSolution<typename Adapter::gid_t,
                               typename Adapter::lno_t> > &solution,
    const RCP<Teuchos::ParameterList> &pl,
    const RCP<Teuchos::Comm<int> > &comm
  ) 
  {
    typedef typename Adapter::lno_t lno_t;

    int ierr= 0;

    HELLO;

    // Local permutation only for now.

    // Set identity permutation.
    const size_t n = model->getLocalNumIdentifiers();
    lno_t *perm;
    perm = (lno_t *) (solution->getPermutationRCP().getRawPtr());
    if (perm){
      for (size_t i=0; i<n; i++){
        perm[i] = i;
      }
    }
    else
      // TODO: throw exception?
      ierr = -1;

    solution->setHavePerm(true);
    return ierr;

  }

};
}
#endif