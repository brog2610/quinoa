#ifndef MUELU_CONFIG_HPP
#define MUELU_CONFIG_HPP

/* Options */

#define HAVE_MPI

/* #undef HAVE_MUELU_DEBUG */

/* #undef HAVE_MUELU_EXPLICIT_INSTANTIATION */

/* #undef HAVE_MUELU_PROFILING */

/* #undef HAVE_MUELU_GOOGLE_PERFTOOLS */

/* #undef HAVE_MUELU_EXPERIMENTAL */

/* #undef HAVE_MUELU_BOOST_FOR_REAL */

/* #undef HAVE_MUELU_KOKKOS_REFACTOR */

/* Optional Dependencies */

#define HAVE_MUELU_AMESOS

#define HAVE_MUELU_AMESOS2

#define HAVE_MUELU_AZTECOO

#define HAVE_MUELU_BELOS

#define HAVE_MUELU_EPETRA

#define HAVE_MUELU_EPETRAEXT

#define HAVE_MUELU_GALERI

#define HAVE_MUELU_IFPACK

#define HAVE_MUELU_IFPACK2

/* #undef HAVE_MUELU_ISORROPIA */

/* #undef HAVE_MUELU_KOKKOSCLASSIC */

#define HAVE_MUELU_ML

/* #undef HAVE_MUELU_TEKO */

#define HAVE_MUELU_TPETRA

#define HAVE_MUELU_STRATIMIKOS

#define HAVE_MUELU_ZOLTAN

#define HAVE_MUELU_ZOLTAN2

/* Optional TPL */

/* #undef HAVE_MUELU_BOOST */

/* #undef HAVE_MUELU_TIMER_SYNCHRONIZATION */

/* #undef HAVE_MUELU_MATLAB */

/* #undef HAVE_MUELU_AMGX */

/* #undef HAVE_MUELU_CGAL */

/*  
 If deprecated warnings are on, and the compiler supports them, then
 define MUELU_DEPRECATED to emit deprecated warnings.  Otherwise,
 give it an empty definition.
*/
#define MUELU_DEPRECATED
#define MUELU_DEPRECATED_MSG(MSG)


#endif /* MUELU_CONFIG_HPP */
