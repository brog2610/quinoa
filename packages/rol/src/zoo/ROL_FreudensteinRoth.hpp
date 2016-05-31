// @HEADER
// ************************************************************************
//
//               Rapid Optimization Library (ROL) Package
//                 Copyright (2014) Sandia Corporation
//
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
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
// Questions? Contact lead developers:
//              Drew Kouri   (dpkouri@sandia.gov) and
//              Denis Ridzal (dridzal@sandia.gov)
//
// ************************************************************************
// @HEADER

/** \file
    \brief  Contains definitions for Freudenstein and Roth's function.
    \author Created by D. Ridzal and D. Kouri.
 */

#ifndef USE_HESSVEC 
#define USE_HESSVEC 1
#endif

#ifndef ROL_FREUDENSTEINROTH_HPP
#define ROL_FREUDENSTEINROTH_HPP

#include "ROL_StdVector.hpp"
#include "ROL_Objective.hpp"

namespace ROL {
namespace ZOO {

/** \brief Freudenstein and Roth's function.
 */
template<class Real>
class Objective_FreudensteinRoth : public Objective<Real> {
public:
  Objective_FreudensteinRoth() {}

  Real value( const Vector<Real> &x, Real &tol ) {
    Teuchos::RCP<const std::vector<Real> > ex
      = Teuchos::dyn_cast<const StdVector<Real> >(x).getVector();

    Real f1 = -13.0 + (*ex)[0] + ((5.0-(*ex)[1])*(*ex)[1] - 2.0)*(*ex)[1];
    Real f2 = -29.0 + (*ex)[0] + (((*ex)[1]+1.0)*(*ex)[1] - 14.0)*(*ex)[1];

    return f1*f1+f2*f2;
  }

  void gradient( Vector<Real> &g, const Vector<Real> &x, Real &tol ) {
    Teuchos::RCP<std::vector<Real> > eg
      = Teuchos::dyn_cast<StdVector<Real> >(g).getVector();
    Teuchos::RCP<const std::vector<Real> > ex
      = Teuchos::dyn_cast<const StdVector<Real> >(x).getVector();

    Real f1 = -13.0 + (*ex)[0] + ((5.0-(*ex)[1])*(*ex)[1] - 2.0)*(*ex)[1];
    Real f2 = -29.0 + (*ex)[0] + (((*ex)[1]+1.0)*(*ex)[1] - 14.0)*(*ex)[1];

    Real f11 = 1.0;
    Real f12 = 10.0*(*ex)[1] - 3.0*(*ex)[1]*(*ex)[1] - 2.0;
    Real f21 = 1.0;
    Real f22 = 3.0*(*ex)[1]*(*ex)[1] + 2.0*(*ex)[1] - 14.0;

    (*eg)[0] = 2.0*(f11*f1 + f21*f2);
    (*eg)[1] = 2.0*(f12*f1 + f22*f2);
  }
#if USE_HESSVEC
  void hessVec( Vector<Real> &hv, const Vector<Real> &v, const Vector<Real> &x, Real &tol ) {
    Teuchos::RCP<std::vector<Real> > ehv
      = Teuchos::dyn_cast<StdVector<Real> >(hv).getVector();
    Teuchos::RCP<const std::vector<Real> > ev
      = Teuchos::dyn_cast<const StdVector<Real> >(v).getVector();
    Teuchos::RCP<const std::vector<Real> > ex
      = Teuchos::dyn_cast<const StdVector<Real> >(x).getVector();

    Real f1 = -13.0 + (*ex)[0] + ((5.0-(*ex)[1])*(*ex)[1] - 2.0)*(*ex)[1];
    Real f2 = -29.0 + (*ex)[0] + (((*ex)[1]+1.0)*(*ex)[1] - 14.0)*(*ex)[1];

    Real f11 = 1.0;
    Real f12 = 10.0*(*ex)[1] - 3.0*(*ex)[1]*(*ex)[1] - 2.0;
    Real f21 = 1.0;
    Real f22 = 3.0*(*ex)[1]*(*ex)[1] + 2.0*(*ex)[1] - 14.0;

    Real f122 = 10.0 - 6.0*(*ex)[1];
    Real f222 = 6.0*(*ex)[1] + 2.0;

    Real h11 = 2.0*(f11*f11) + 2.0*(f21*f21);
    Real h12 = 2.0*(f12*f11) + 2.0*(f22*f21);
    Real h22 = 2.0*(f122*f1 + f12*f12) + 2.0*(f222*f2 + f22*f22);

    (*ehv)[0] = h11*(*ev)[0] + h12*(*ev)[1];
    (*ehv)[1] = h12*(*ev)[0] + h22*(*ev)[1];
  }
#endif
  void invHessVec( Vector<Real> &hv, const Vector<Real> &v, const Vector<Real> &x, Real &tol ) {
    Teuchos::RCP<std::vector<Real> > ehv
      = Teuchos::dyn_cast<StdVector<Real> >(hv).getVector();
    Teuchos::RCP<const std::vector<Real> > ev
      = Teuchos::dyn_cast<const StdVector<Real> >(v).getVector();
    Teuchos::RCP<const std::vector<Real> > ex
      = Teuchos::dyn_cast<const StdVector<Real> >(x).getVector();

    Real f1 = -13.0 + (*ex)[0] + ((5.0-(*ex)[1])*(*ex)[1] - 2.0)*(*ex)[1];
    Real f2 = -29.0 + (*ex)[0] + (((*ex)[1]+1.0)*(*ex)[1] - 14.0)*(*ex)[1];

    Real f11 = 1.0;
    Real f12 = 10.0*(*ex)[1] - 3.0*(*ex)[1]*(*ex)[1] - 2.0;
    Real f21 = 1.0;
    Real f22 = 3.0*(*ex)[1]*(*ex)[1] + 2.0*(*ex)[1] - 14.0;

    Real f122 = 10.0 - 6.0*(*ex)[1];
    Real f222 = 6.0*(*ex)[1] + 2.0;

    Real h11 = 2.0*(f11*f11) + 2.0*(f21*f21);
    Real h12 = 2.0*(f12*f11) + 2.0*(f22*f21);
    Real h22 = 2.0*(f122*f1 + f12*f12) + 2.0*(f222*f2 + f22*f22);

    (*ehv)[0] = (1.0/(h11*h22-h12*h12))*( h22*(*ev)[0] - h12*(*ev)[1]);
    (*ehv)[1] = (1.0/(h11*h22-h12*h12))*(-h12*(*ev)[0] + h11*(*ev)[1]);
  }
};

template<class Real>
void getFreudensteinRoth( Teuchos::RCP<Objective<Real> > &obj,
                          Teuchos::RCP<Vector<Real> >    &x0,
                          Teuchos::RCP<Vector<Real> >    &x ) {
  // Problem dimension
  int n = 2;

  // Get Initial Guess
  Teuchos::RCP<std::vector<Real> > x0p = Teuchos::rcp(new std::vector<Real>(n,0.0));
  (*x0p)[0] = 0.5; (*x0p)[1] = -2.0;
  x0 = Teuchos::rcp(new StdVector<Real>(x0p));

  // Get Solution
  Teuchos::RCP<std::vector<Real> > xp = Teuchos::rcp(new std::vector<Real>(n,0.0));
  (*xp)[0] = 5.0; (*xp)[1] = 4.0;
  x = Teuchos::rcp(new StdVector<Real>(xp));

  // Instantiate Objective Function
  obj = Teuchos::rcp(new Objective_FreudensteinRoth<Real>);
}


} // End ZOO Namespace
} // End ROL Namespace

#endif
