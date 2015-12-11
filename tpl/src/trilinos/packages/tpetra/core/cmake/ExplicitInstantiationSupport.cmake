# The file that is produced by this module
SET(${PACKAGE_NAME}_ETI_FILE TpetraCore_ETIHelperMacros.h)
SET(${PACKAGE_NAME}_ETI_FILE_PATH ${Tpetra_BINARY_DIR}/core/src/${${PACKAGE_NAME}_ETI_FILE})

#
# A) See if a static pre-created file is provide and us it if it is
#

ADVANCED_SET(Tpetra_USE_STATIC_ETI_MACROS_HEADER_FILE ""
  CACHE PATH
  "If set, gives the path to a static version of the file ${${PACKAGE_NAME}_ETI_FILE}.  If not set (default '') then the file is generated automatically (and at great cost)"
  )

IF(Tpetra_USE_STATIC_ETI_MACROS_HEADER_FILE)
  MESSAGE("-- NOTE: Skipping generation and using provided static file"
     " '${Tpetra_USE_STATIC_ETI_MACROS_HEADER_FILE}'")
  CONFIGURE_FILE(
    ${Tpetra_USE_STATIC_ETI_MACROS_HEADER_FILE}
    ${${PACKAGE_NAME}_ETI_FILE_PATH}
    COPYONY
    )
  RETURN()
ENDIF()

#
# B) We must generate the file anew :-(
#

# Tpetra ETI type fields
SET(${PACKAGE_NAME}_ETI_FIELDS "SIN|SOUT|S|LO|GO|N|CS|DS")

# mfh 11 Feb 2015: If a given Node type doesn't support certain
# combinations of Scalar, LO, GO, etc. types, this would be the place
# to add in only those type combinations that the Node type supports.
# We used to do this for Node = KokkosClassic::ThrustGPUNode.  I much
# prefer instead that all type combinations _build_, but that
# unsupported types for a particular Node get stub implementations
# that just throw.  This simplifies the CMake configuration process.

TRIBITS_ETI_TYPE_EXPANSION(${PACKAGE_NAME}_ETI_EXCLUDE_SET_ORDINAL_SCALAR "S=int|long|unsigned int|unsigned|long long" "LO=.*" "GO=.*" "N=.*")

ASSERT_DEFINED(TpetraCore_ENABLE_EXPLICIT_INSTANTIATION)
IF(TpetraCore_ENABLE_EXPLICIT_INSTANTIATION)
  MESSAGE(STATUS "User/Downstream ETI set: ${${PACKAGE_NAME}_ETI_LIBRARYSET}")
  MESSAGE(STATUS "Excluded instantiations: ${${PACKAGE_NAME}_ETI_EXCLUDE_SET}:${${PACKAGE_NAME}_ETI_EXCLUDE_SET_INT}")
  MESSAGE(STATUS "Full coverage explicit instantiation for scalars:         ${${PACKAGE_NAME}_ETI_SCALARS}")
  MESSAGE(STATUS "Full coverage explicit instantiation for global ordinals: ${${PACKAGE_NAME}_ETI_GORDS}")
  MESSAGE(STATUS "Full coverage explicit instantiation for local ordinals:  ${${PACKAGE_NAME}_ETI_LORDS}")
  MESSAGE(STATUS "Full coverage explicit instantiation for nodes:           ${${PACKAGE_NAME}_ETI_NODES}")
  # generate ETI macros for Tpetra usage 
  # this uses the following lists: 
  #             nodes: ${PACKAGE_NAME}_ETI_NODES 
  #           scalars: ${PACKAGE_NAME}_ETI_SCALARS
  #   global ordinals: ${PACKAGE_NAME}_ETI_GORDS
  #    local ordinals: ${PACKAGE_NAME}_ETI_LORDS
  # assemble dual scalar (only) instantiations
  JOIN(${PACKAGE_NAME}_ETI_LORDS   "|" FALSE ${${PACKAGE_NAME}_ETI_LORDS}  )
  JOIN(${PACKAGE_NAME}_ETI_GORDS   "|" FALSE ${${PACKAGE_NAME}_ETI_GORDS}  )
  JOIN(${PACKAGE_NAME}_ETI_NODES   "|" FALSE ${${PACKAGE_NAME}_ETI_NODES}  )
  JOIN(${PACKAGE_NAME}_ETI_SCALARS "|" FALSE ${${PACKAGE_NAME}_ETI_SCALARS})
  # assemble single scalar instantiations
  TRIBITS_ETI_TYPE_EXPANSION(SingleScalarInsts   "S=${${PACKAGE_NAME}_ETI_SCALARS}" "N=${${PACKAGE_NAME}_ETI_NODES}"
                                                 "LO=${${PACKAGE_NAME}_ETI_LORDS}" "GO=${${PACKAGE_NAME}_ETI_GORDS}")
  TRIBITS_ADD_ETI_INSTANTIATIONS(TpetraCore ${SingleScalarInsts})
  IF(${PROJECT_NAME}_VERBOSE_CONFIGURE)
    MESSAGE(STATUS "ETI set (before exclusions): ${${PACKAGE_NAME}_ETI_LIBRARYSET}")
  ENDIF()
ELSE()
  # no ETI: these macros are used only for testing
  JOIN(${PACKAGE_NAME}_ETI_NODES   "|" FALSE ${${PACKAGE_NAME}_ETI_NODES}  )
  TRIBITS_ETI_TYPE_EXPANSION(${PACKAGE_NAME}_ETI_LIBRARYSET "S=double" 
                                                   "LO=int" 
                                                   "GO=int" 
                                                   "N=${${PACKAGE_NAME}_ETI_NODES}")
  IF (SS_FOR_DEV_PS_FOR_RELEASE AND HAVE_COMPLEX_BLAS)
    TRIBITS_ETI_TYPE_EXPANSION(${PACKAGE_NAME}_ETI_LIBRARYSET "S=std::complex<double>" 
                                                     "LO=int" 
                                                     "GO=int" 
                                                     "N=${${PACKAGE_NAME}_ETI_NODES}")
  ENDIF()
ENDIF()

TRIBITS_ETI_GENERATE_MACROS(
    "${${PACKAGE_NAME}_ETI_FIELDS}" "${${PACKAGE_NAME}_ETI_LIBRARYSET}" 
    "${${PACKAGE_NAME}_ETI_EXCLUDE_SET}"
    list_of_manglings eti_typedefs
    "TPETRA_INSTANTIATE_VECTOR(S,LO,GO,N)"            TPETRA_ETIMACRO_VECTOR)
TRIBITS_ETI_GENERATE_MACROS(
    "${${PACKAGE_NAME}_ETI_FIELDS}" "${${PACKAGE_NAME}_ETI_LIBRARYSET}" 
    "${${PACKAGE_NAME}_ETI_EXCLUDE_SET};${${PACKAGE_NAME}_ETI_EXCLUDE_SET_ORDINAL_SCALAR}"  
    list_of_manglings eti_typedefs
    "TPETRA_INSTANTIATE_SLGN_NO_ORDINAL_SCALAR(S,LO,GO,N)" TPETRA_ETIMACRO_SLGN_NO_ORDINAL_SCALAR
    "TPETRA_INSTANTIATE_SLG_NO_ORDINAL_SCALAR(S,LO,GO)"    TPETRA_ETIMACRO_SLG_NO_ORDINAL_SCALAR
    "TPETRA_INSTANTIATE_SL_NO_ORDINAL_SCALAR(S,LO)"        TPETRA_ETIMACRO_SL_NO_ORDINAL_SCALAR
    "TPETRA_INSTANTIATE_S_NO_ORDINAL_SCALAR(S)"            TPETRA_ETIMACRO_S_NO_ORDINAL_SCALAR)
TRIBITS_ETI_GENERATE_MACROS(
    "${${PACKAGE_NAME}_ETI_FIELDS}" "${${PACKAGE_NAME}_ETI_LIBRARYSET}" 
    "${${PACKAGE_NAME}_ETI_EXCLUDE_SET};${${PACKAGE_NAME}_ETI_EXCLUDE_SET_INT}"  
    list_of_manglings eti_typedefs
    "TPETRA_INSTANTIATE_SLGN(S,LO,GO,N)"            TPETRA_ETIMACRO_SLGN 
    "TPETRA_INSTANTIATE_LGN(LO,GO,N)"               TPETRA_ETIMACRO_LGN
    "TPETRA_INSTANTIATE_SLG(S,LO,GO)"               TPETRA_ETIMACRO_SLG  
    "TPETRA_INSTANTIATE_LG(LO,GO)"                  TPETRA_ETIMACRO_LG 
    "TPETRA_INSTANTIATE_SL(S,LO)"                   TPETRA_ETIMACRO_SL
    "TPETRA_INSTANTIATE_S(S)"                       TPETRA_ETIMACRO_S
    "TPETRA_INSTANTIATE_N(N)"                       TPETRA_ETIMACRO_N
    "TPETRA_INSTANTIATE_TSLGN(CS,DS,LO,GO,N)"       TPETRA_ETIMACRO_TSLGN 
    "TPETRA_INSTANTIATE_TSLG(CS,DS,LO,GO)"          TPETRA_ETIMACRO_TSLG
    "TPETRA_INSTANTIATE_CONVERT(SOUT,SIN,LO,GO,N)"  TPETRA_ETIMACRO_CONVERT
    "TPETRA_INSTANTIATE_CONVERT_SSL(SOUT,SIN,LO)"    TPETRA_ETIMACRO_CONVERT_SSL)
TRIBITS_ETI_GENERATE_MACROS(
    "${${PACKAGE_NAME}_ETI_FIELDS}" "${${PACKAGE_NAME}_ETI_LIBRARYSET}" 
    "${${PACKAGE_NAME}_ETI_EXCLUDE_SET}"
    list_of_manglings   eti_typedefs
    "TPETRA_INSTANTIATE_SLGN_NOGPU(S,LO,GO,N)"            TPETRA_ETIMACRO_SLGN_NOGPU
    "TPETRA_INSTANTIATE_LGN_NOGPU(LO,GO,N)"               TPETRA_ETIMACRO_LGN_NOGPU
    "TPETRA_INSTANTIATE_SLG_NOGPU(S,LO,GO)"               TPETRA_ETIMACRO_SLG_NOGPU
    "TPETRA_INSTANTIATE_LG_NOGPU(LO,GO)"                  TPETRA_ETIMACRO_LG_NOGPU
    "TPETRA_INSTANTIATE_SL_NOGPU(S,LO)"                   TPETRA_ETIMACRO_SL_NOGPU
    "TPETRA_INSTANTIATE_S_NOGPU(S)"                       TPETRA_ETIMACRO_S_NOGPU
    "TPETRA_INSTANTIATE_N_NOGPU(N)"                       TPETRA_ETIMACRO_N_NOGPU
    "TPETRA_INSTANTIATE_TSLGN_NOGPU(CS,DS,LO,GO,N)"       TPETRA_ETIMACRO_TSLGN_NOGPU
    "TPETRA_INSTANTIATE_TSLG_NOGPU(CS,DS,LO,GO)"          TPETRA_ETIMACRO_TSLG_NOGPU
    "TPETRA_INSTANTIATE_CONVERT_NOGPU(SOUT,SIN,LO,GO,N)"  TPETRA_ETIMACRO_CONVERT_NOGPU
    "TPETRA_INSTANTIATE_CONVERT_NOGPU_SSL(SOUT,SIN,LO)"    TPETRA_ETIMACRO_CONVERT_NOGPU_SSL)
STRING(REPLACE "S="  "P=" ScalarToPacketSet "${${PACKAGE_NAME}_ETI_LIBRARYSET}")
STRING(REGEX REPLACE "GO=([^{ ]+)" "GO=\\1 P=\\1" GlobalToPacketSet1 "${${PACKAGE_NAME}_ETI_LIBRARYSET}")
STRING(REGEX REPLACE "GO={([^}]+)}" "GO={\\1} P={\\1}" GlobalToPacketSet2 "${${PACKAGE_NAME}_ETI_LIBRARYSET}")
TRIBITS_ETI_GENERATE_MACROS(
    "${${PACKAGE_NAME}_ETI_FIELDS}|P" "${ScalarToPacketSet};${GlobalToPacketSet1};${GlobalToPacketSet2}" 
    "${${PACKAGE_NAME}_ETI_EXCLUDE_SET}"  
    list_of_manglings eti_typedefs
    "TPETRA_INSTANTIATE_PLGN(P,LO,GO,N)"            TPETRA_ETIMACRO_PLGN)

# testing macros
TRIBITS_ETI_GENERATE_MACROS(
    "${${PACKAGE_NAME}_ETI_FIELDS}" "${${PACKAGE_NAME}_ETI_LIBRARYSET}" 
    "${${PACKAGE_NAME}_ETI_EXCLUDE_SET};S=int LO=.* GO=.* N=.*;S=long LO=.* GO=.* N=.*;S=unsigned LO=.* GO=.* N=.*;S=unsigned int LO=.* GO=.* N=.*;S=long long LO=.* GO=.* N=.*"
    list_of_manglings eti_typedefs
    "TPETRA_INSTANTIATE_TESTMV(S,LO,GO,N)"            TPETRA_ETIMACRO_TESTMV)
TRIBITS_ETI_GENERATE_MACROS(
    "${${PACKAGE_NAME}_ETI_FIELDS}" "${${PACKAGE_NAME}_ETI_LIBRARYSET}" 
    "${${PACKAGE_NAME}_ETI_EXCLUDE_SET};S=int LO=.* GO=.* N=.*;S=long LO=.* GO=.* N=.*;S=unsigned LO=.* GO=.* N=.*;S=unsigned int LO=.* GO=.* N=.*;S=long long LO=.* GO=.* N=.*"
    list_of_manglings eti_typedefs
    "TPETRA_INSTANTIATE_TESTMV_NOGPU(S,LO,GO,N)"      TPETRA_ETIMACRO_TESTMV_NOGPU)

TRIBITS_ETI_TYPE_EXPANSION(Tpetra_DII   "S=double" "N=${${PACKAGE_NAME}_ETI_NODES}" "LO=int" "GO=int")
TRIBITS_ETI_GENERATE_MACROS(
    "${${PACKAGE_NAME}_ETI_FIELDS}" "${Tpetra_DII}" 
    "${${PACKAGE_NAME}_ETI_EXCLUDE_SET}"
    list_of_manglings eti_typedefs
    "TPETRA_INSTANTIATE_DOUBLE_INT_INT_N(S,LO,GO,N)"      TPETRA_ETIMACRO_DII_NODE)

TRIBITS_ETI_GENERATE_TYPEDEF_MACRO(TPETRA_ETI_TYPEDEFS "TPETRA_ETI_MANGLING_TYPEDEFS" "${eti_typedefs}")

CONFIGURE_FILE(
  ${${PACKAGE_NAME}_SOURCE_DIR}/cmake/TpetraCore_ETIHelperMacros.h.in
  ${${PACKAGE_NAME}_ETI_FILE_PATH}
  )
