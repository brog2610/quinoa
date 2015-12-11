#pragma once
#ifndef __ICHOL_BY_BLOCKS_HPP__
#define __ICHOL_BY_BLOCKS_HPP__

/// \file ichol_by_blocks.hpp
/// \brief Sparse incomplete Cholesky factorization by blocks.
/// \author Kyungjoo Kim (kyukim@sandia.gov)

namespace Example {

  using namespace std;

  template<typename ParallelForType,
           typename CrsTaskViewType>
  KOKKOS_INLINE_FUNCTION
  static int genScalarTask_ICholUpperByBlocks(typename CrsTaskViewType::policy_type &policy,
                                              CrsTaskViewType &A);

  template<typename ParallelForType,
           typename CrsTaskViewType>
  KOKKOS_INLINE_FUNCTION
  static int genTrsmTasks_ICholUpperByBlocks(typename CrsTaskViewType::policy_type &policy,
                                             CrsTaskViewType &A,
                                             CrsTaskViewType &B);

  template<typename ParallelForType,
           typename CrsTaskViewType>
  KOKKOS_INLINE_FUNCTION
  static int genHerkTasks_ICholUpperByBlocks(typename CrsTaskViewType::policy_type &policy,
                                             CrsTaskViewType &A,
                                             CrsTaskViewType &C);

  template<>
  template<typename ParallelForType,
           typename CrsTaskViewType>
  KOKKOS_INLINE_FUNCTION
  int
  IChol<Uplo::Upper,AlgoIChol::ByBlocks>
  ::invoke(typename CrsTaskViewType::policy_type &policy,
           const typename CrsTaskViewType::policy_type::member_type &member,
           CrsTaskViewType &A) {
    // this task generation should be done by a root
    // ---------------------------------------------
    if (member.team_rank() == 0) {
      CrsTaskViewType ATL, ATR,      A00, A01, A02,
        /**/          ABL, ABR,      A10, A11, A12,
        /**/                         A20, A21, A22;

      Part_2x2(A,  ATL, ATR,
               /**/ABL, ABR,
               0, 0, Partition::TopLeft);

      while (ATL.NumRows() < A.NumRows()) {
        Part_2x2_to_3x3(ATL, ATR, /**/  A00, A01, A02,
                        /*******/ /**/  A10, A11, A12,
                        ABL, ABR, /**/  A20, A21, A22,
                        1, 1, Partition::BottomRight);
        // -----------------------------------------------------

        // A11 = chol(A11)
        genScalarTask_ICholUpperByBlocks<ParallelForType>(policy, A11);

        // A12 = inv(triu(A11)') * A12
        genTrsmTasks_ICholUpperByBlocks<ParallelForType>(policy, A11, A12);

        // A22 = A22 - A12' * A12
        genHerkTasks_ICholUpperByBlocks<ParallelForType>(policy, A12, A22);

        // -----------------------------------------------------
        Merge_3x3_to_2x2(A00, A01, A02, /**/ ATL, ATR,
                         A10, A11, A12, /**/ /******/
                         A20, A21, A22, /**/ ABL, ABR,
                         Partition::TopLeft);
      }
    }

    return 0;
  }

}

// select one of the following variants

#include "ichol_by_blocks_var1.hpp"
//#include "ichol_by_blocks_serial.hpp"

#endif
