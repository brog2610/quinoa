#ifndef BASKER_ERROR_MANAGER
#define BASKER_ERROR_MANAGER

/*Basker Includes*/
#include "basker_types.hpp"
#include "basker_util.hpp"
#include "basker_structs.hpp"
#include "basker_matrix_view_decl.hpp"
#include "basker_matrix_view_def.hpp"


/*Kokkos Includes*/
#ifdef BASKER_KOKKOS
#include <Kokkos_Core.hpp>
#include <impl/Kokkos_Timer.hpp>
#else
#include <omp.h>
#endif


/*System Includes*/
#include <iostream>
#include <string>

namespace BaskerNS
{
  template <class Int, class Entry, class Exe_Space>
  BASKER_INLINE
  int Basker<Int,Entry,Exe_Space>::nfactor_domain_error
  (
   INT_1DARRAY threads_start
   )
  {
    Int nthread_remalloc = 0;
    for(Int ti = 0; ti < num_threads; ti++)
      {

	//Note: jdb we can make this into a switch
	if(thread_array(ti).error_type ==
	   BASKER_ERROR_NOERROR)
	  {
	    threads_start(ti) = BASKER_MAX_IDX;
	    continue;
	  }//end if NOERROR

	if(thread_array(ti).error_type ==
	   BASKER_ERROR_SINGULAR)
	  {
	    printf("ERROR THREAD: %d DOMBLK SINGULAR: %d\n",
		   ti,
		   thread_array(ti).error_blk);
	    return BASKER_ERROR;
	  }//end if SINGULAR
	
	if(thread_array(ti).error_type ==
	   BASKER_ERROR_NOMALLOC)
	  {
	    printf("ERROR THREADS: %d DOMBLK NOMALLOC: %d\n",
		   ti,
		   thread_array(ti).error_blk);
	    return BASKER_ERROR;
	  }//end if NOMALLOC
	
	if(thread_array(ti).error_type ==
	   BASKER_ERROR_REMALLOC)
	  {

	    BASKER_ASSERT(thread_array(ti).error_blk > 0,
			  "nfactor_dom_error error_blk");

	    printf("ERROR THREADS: %d DOMBLK MALLOC: %d \n",
		   ti,
		   thread_array(ti).error_blk);
	    

	    //Resize L
	    BASKER_MATRIX &L = LL(thread_array(ti).error_blk)(0);
	    RESIZE_INT_1DARRAY(L.row_idx,
			       L.nnz,
			       thread_array(ti).error_info);
	    RESIZE_ENTRY_1DARRAY(L.val,
				 L.nnz,
				 thread_array(ti).error_info);
	    L.nnz = thread_array(ti).error_info;
	    
	    //Resize U
	    BASKER_MATRIX &U = LU(thread_array(ti).error_blk)(0);
	    RESIZE_INT_1DARRAY(U.row_idx,
			       U.nnz,
			       thread_array(ti).error_info);
	    RESIZE_ENTRY_1DARRAY(U.val,
				 U.nnz,
				 thread_array(ti).error_info);
	    U.nnz = thread_array(ti).error_info;
	    
	    threads_start(ti) = thread_array(ti).error_blk;
	

	    //Reset 
	    thread_array(ti).error_type = BASKER_ERROR_NOERROR;
	    thread_array(ti).error_blk  = BASKER_MAX_IDX;
	    thread_array(ti).error_info = BASKER_MAX_IDX;

	    nthread_remalloc++;
    
	  }//if REMALLOC

      }//for all threads
    
    if(nthread_remalloc == 0)
      {
	return BASKER_SUCCESS;
      }
    else
      {
	return nthread_remalloc;
      }

    //Should never be here
    BASKER_ASSERT(0==1, "nfactor_diag_error, should never");
    return BASKER_SUCCESS;
  }//end nfactor_domain_error



  template <class Int, class Entry, class Exe_Space>
  BASKER_INLINE
  int Basker<Int,Entry,Exe_Space>::nfactor_diag_error
  (
   INT_1DARRAY threads_start
   )
  {
    Int nthread_remalloc = 0;
    for(Int ti = 0; ti < num_threads; ti++)
      {

	//Note: jdb we can make this into a switch
	if(thread_array(ti).error_type ==
	   BASKER_ERROR_NOERROR)
	  {
	    threads_start(ti) = BASKER_MAX_IDX;
	    continue;
	  }//end if NOERROR

	if(thread_array(ti).error_type ==
	   BASKER_ERROR_SINGULAR)
	  {
	    printf("ERROR THREAD: %d DIAGBLK SINGULAR: %d\n",
		   ti,
		   thread_array(ti).error_blk);
	    return BASKER_ERROR;
	  }//end if SINGULAR
	
	if(thread_array(ti).error_type ==
	   BASKER_ERROR_NOMALLOC)
	  {
	    printf("ERROR THREADS: %d DIAGBLK NOMALLOC: %d\n",
		   ti,
		   thread_array(ti).error_blk);
	    return BASKER_ERROR;
	  }//end if NOMALLOC
	
	if(thread_array(ti).error_type ==
	   BASKER_ERROR_REMALLOC)
	  {

	    BASKER_ASSERT(thread_array(ti).error_blk > 0,
			  "nfactor_diag_error error_blk");

	    printf("ERROR THREADS: %d DIAGBLK MALLOC: %d \n",
		   ti,
		   thread_array(ti).error_blk);
	    

	    //Resize L
	    BASKER_MATRIX &L = LBTF(thread_array(ti).error_blk);
	    RESIZE_INT_1DARRAY(L.row_idx,
			       L.nnz,
			       thread_array(ti).error_info);
	    RESIZE_ENTRY_1DARRAY(L.val,
				 L.nnz,
				 thread_array(ti).error_info);
	    L.nnz = thread_array(ti).error_info;
	    
	    //Resize U
	    BASKER_MATRIX &U = UBTF(thread_array(ti).error_blk);
	    RESIZE_INT_1DARRAY(U.row_idx,
			       U.nnz,
			       thread_array(ti).error_info);
	    RESIZE_ENTRY_1DARRAY(U.val,
				 U.nnz,
				 thread_array(ti).error_info);
	    U.nnz = thread_array(ti).error_info;
	    
	    threads_start(ti) = thread_array(ti).error_blk;
	

	    //Reset 
	    thread_array(ti).error_type = BASKER_ERROR_NOERROR;
	    thread_array(ti).error_blk  = BASKER_MAX_IDX;
	    thread_array(ti).error_info = BASKER_MAX_IDX;

	    nthread_remalloc++;
    
	  }//if REMALLOC

      }//for all threads
    
    if(nthread_remalloc == 0)
      {
	return BASKER_SUCCESS;
      }
    else
      {
	return nthread_remalloc;
      }

    //Should never be here
    BASKER_ASSERT(0==1, "nfactor_diag_error, should never");
    return BASKER_SUCCESS;
  }//end nfactor_diag_error

}//end namespace BaskerNS

#endif //END BASER_ERROR_MANAGER
