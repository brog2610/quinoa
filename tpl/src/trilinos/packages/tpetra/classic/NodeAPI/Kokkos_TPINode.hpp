//@HEADER
// ************************************************************************
//
//          Kokkos: Node API and Parallel Node Kernels
//              Copyright (2008) Sandia Corporation
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

#ifndef KOKKOS_TPINODE_HPP_
#define KOKKOS_TPINODE_HPP_

/// \file Kokkos_TPINode.hpp
/// \brief Declaration and definition of the (now DEPRECATED)
///   KokkosClassic::DoNotUse::TPINode Node type.
/// \warning KokkosClassic::DoNotUse::TPINode has been DEPRECATED.
///   For a Node that uses Pthreads for thread-level parallelism,
///   please use Kokkos::Compat::KokkosThreadsWrapperNode instead.

#include "Kokkos_ConfigDefs.hpp"

// mfh 08 Jan 2015: Don't enable the contents of this file unless the
// appropriate CMake option is enabled.  This avoids deprecation
// warnings once we deprecate this Node type.
#ifdef HAVE_TPETRACLASSIC_THREADPOOL

#include "Kokkos_StandardNodeMemoryModel.hpp"
#include "Kokkos_NodeHelpers.hpp"
#include <TPI.h>

#ifdef HAVE_TPETRACLASSIC_TEUCHOSKOKKOSCOMPAT
#  include "Kokkos_Core.hpp"
#endif

namespace KokkosClassic {

  template <class WDP>
  struct WDPPlusRange {
    WDPPlusRange(int Beg, int End, WDP Wdp) : wdp(Wdp), beg(Beg), end(End){}
    WDP wdp;
    int beg, end;
  };

  inline
  void tpi_work_span(TPI_Work* work, int beg, int end, int& ibeg, int& iend)
  {
    const int chunk = ( end - beg + work->count - 1 ) / work->count ;

    iend = chunk * ( work->rank + 1 ) + beg;
    ibeg = chunk * ( work->rank ) + beg;

    if ( end < iend ) { iend = end; }
  }

  template<class WDP>
  void tpi_execute(TPI_Work * work)
  {
    // get work/data pair
    const WDPPlusRange<WDP>* const_wdp_wrapper = static_cast<const WDPPlusRange<WDP>*>(work->info);
    WDPPlusRange<WDP>* wdp_wrapper = const_cast<WDPPlusRange<WDP>*>(const_wdp_wrapper);
    WDP wdp = wdp_wrapper->wdp;
    int beg = wdp_wrapper->beg, end = wdp_wrapper->end;
    int ibeg, iend;
    // determine my share of the work
    tpi_work_span(work, beg, end, ibeg, iend);
    // do my share of the work
    for (int i=ibeg; i<iend; ++i) {
      wdp.execute(i);
    }
  }

  template<class WDP>
  void tpi_reduction_work(TPI_Work * work)
  {
    const WDPPlusRange<WDP>* wdp_wrapper = static_cast<const WDPPlusRange<WDP>*>(work->info);
    int beg = wdp_wrapper->beg, end = wdp_wrapper->end;
    WDP wdp = wdp_wrapper->wdp;
    int ibeg, iend;
    tpi_work_span(work, beg, end, ibeg, iend);

    typedef typename WDP::ReductionType ReductionType;
    ReductionType tmpi;
    ReductionType &res = *(static_cast<ReductionType*>(work->reduce));

    for (int i=ibeg; i<iend; ++i) {
      tmpi = wdp.generate(i);
      res = wdp.reduce(res, tmpi);
    }
  }

  template<class WDP>
  void tpi_reduction_join(TPI_Work * work, const void* src)
  {
    typedef typename WDP::ReductionType ReductionType;

    const WDPPlusRange<WDP>* wdp_wrapper = static_cast<const WDPPlusRange<WDP>*>(work->info);

    ReductionType& work_reduce = *(static_cast<ReductionType*>(work->reduce));
    const ReductionType& src_reduce  = *(static_cast<const ReductionType*>(src));

    work_reduce = wdp_wrapper->wdp.reduce(work_reduce, src_reduce);
  }

  template<class WDP>
  void tpi_reduction_init(TPI_Work * work)
  {
    typedef typename WDP::ReductionType ReductionType;
    const WDPPlusRange<WDP>* const_wdp_wrapper = static_cast<const WDPPlusRange<WDP>*>(work->info);
    *(static_cast<ReductionType*>(work->reduce)) = const_wdp_wrapper->wdp.identity();
  }

  namespace DoNotUse {

  /// \brief Node API implementation that uses the ThreadPool Trilinos
  ///   package for thread-level parallelism.
  /// \ingroup kokkos_node_api
  /// \warning This class has been DEPRECATED.  For a Node that uses
  ///   Pthreads for thread-level parallelism, please use
  ///   Kokkos::Compat::KokkosThreadsWrapperNode instead.
  ///
  /// ThreadPool uses the POSIX Threads (Pthreads) library underneath.
  class TPETRA_DEPRECATED TPINode : public StandardNodeMemoryModel {
  public:
    /// \brief This is a "classic" Node type.
    ///
    /// That means we plan to deprecate it with the 11.14 release of
    /// Trilinos, and remove it entirely with the 12.0 release.
    static const bool classic = true;

    //! Constructor that sets default parameters.
    TPINode ();

    /// \brief Constructor that takes a list of parameters.
    ///
    /// This constructor accepts the following parameters:
    ///
    ///   - "Num Threads" [int] Specifies the number of threads, calls
    ///     init(). Throws std::runtime_error if less than
    ///     zero. Default: 0.
    ///   - "Verbose" [int] Non-zero parameter specifies that the
    ///     constructor is verbose, printing information about the
    ///     number of threads. Default: 0.
    TPINode (ParameterList &plist);

    /*! \brief Get default parameters for this node */
    static ParameterList getDefaultParameters();

    /*! \brief Thread initialization method.
      If \c numThreads is greater than zero, this calls TPI_Init(). If the threads have already been initialized by this node, it first calls TPI_Finalize().
    */
    void init(int numThreads);

    /*! \brief Default destructor calls TPI_Finalize().
      TPI_Finalize() is called if the number of initialized threads is greater than zero; otherwise, the destructor has no effect.
    */
    ~TPINode();

    //! \begin parallel for skeleton, a wrapper around TPI_Run_threads. See \ref kokkos_node_api "Kokkos Node API"
    template <class WDP>
    static void parallel_for(int beg, int end, WDP wd) {
      WDPPlusRange<WDP> wdp_plus(beg,end,wd);
      TPI_Run_threads(tpi_execute<WDP>, &wdp_plus, 0 );
    }

    //! \begin parallel reduction skeleton, a wrapper around TPI_Run_threads_reduce. See \ref kokkos_node_api "Kokkos Node API"
    template <class WDP>
    static typename WDP::ReductionType
    parallel_reduce(int beg, int end, WDP wd) {
      typedef typename WDP::ReductionType ReductionType;
      ReductionType result = wd.identity();
      WDPPlusRange<WDP> wdp_plus(beg,end,wd);
      TPI_Run_threads_reduce(tpi_reduction_work<WDP>, &wdp_plus,
                             tpi_reduction_join<WDP>,
                             tpi_reduction_init<WDP>, sizeof(result), &result);
      return result;
    }

    //! \begin No-op for TPINode.
    inline void sync() const {}

    /// \brief Return the human-readable name of this Node.
    ///
    /// See \ref kokkos_node_api "Kokkos Node API"
    static std::string name ();

  private:
    int curNumThreads_;
  };

  } // namespace DoNotUse

#ifdef _MSC_VER
#pragma warning(push)
// destructor could not be generated because a base class destructor is inaccessible
#pragma warning(disable : 4624)
#endif

  template <> class ArrayOfViewsHelper<DoNotUse::TPINode> :
    public ArrayOfViewsHelperTrivialImpl<DoNotUse::TPINode>
  {};

#ifdef _MSC_VER
#pragma warning(pop)
#endif

} // namespace KokkosClassic

#if defined(HAVE_TPETRACLASSIC_TEUCHOSKOKKOSCOMPAT) && defined(KOKKOS_HAVE_PTHREAD)
namespace Kokkos {
  namespace Compat {
    template <>
    struct NodeDevice<KokkosClassic::DoNotUse::TPINode> {
      typedef Kokkos::Threads type;
    };
  }
}
#endif

#endif // HAVE_TPETRACLASSIC_THREADPOOL
#endif // KOKKOS_TPINODE_HPP_
