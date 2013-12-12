# -*- shell-script -*-
#
# Copyright (c) 2004-2009 The Trustees of Indiana University and Indiana
#                         University Research and Technology
#                         Corporation.  All rights reserved.
# Copyright (c) 2004-2005 The University of Tennessee and The University
#                         of Tennessee Research Foundation.  All rights
#                         reserved.
# Copyright (c) 2004-2007 High Performance Computing Center Stuttgart, 
#                         University of Stuttgart.  All rights reserved.
# Copyright (c) 2004-2005 The Regents of the University of California.
#                         All rights reserved.
# Copyright (c) 2006-2013 Cisco Systems, Inc.  All rights reserved.
# Copyright (c) 2006-2008 Sun Microsystems, Inc.  All rights reserved.
# Copyright (c) 2006-2007 Los Alamos National Security, LLC.  All rights
#                         reserved. 
# Copyright (c) 2009      Oak Ridge National Labs.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#

AC_DEFUN([OMPI_SETUP_MPI_FORTRAN],[
    OPAL_VAR_SCOPE_PUSH([ompi_fortran_happy])

    # Default to building nothing
    OMPI_BUILD_FORTRAN_MPIFH_BINDINGS=0
    OMPI_BUILD_FORTRAN_USEMPI_BINDINGS=0
    OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS=0

    OMPI_FORTRAN_USEMPI_DIR=
    OMPI_FORTRAN_USEMPI_LIB=

    OMPI_FORTRAN_USEMPIF08_DIR=
    OMPI_FORTRAN_USEMPIF08_LIB=

    OMPI_FORTRAN_HAVE_IGNORE_TKR=0
    OMPI_FORTRAN_HAVE_OPTIONAL_ARGS=0
    OMPI_FORTRAN_HAVE_BIND_C=0
    OMPI_FORTRAN_HAVE_F08_ASSUMED_RANK=0
    OMPI_FORTRAN_HAVE_PRIVATE=0

    # These macros control symbol names for Fortran/C interoperability
    #
    OMPI_F08_SUFFIX="_f08"
    OMPI_F_SUFFIX="_f"

    OMPI_MPI_PREFIX="MPI_"
    OMPI_MPI_BIND_PREFIX="mpi_"

    # Open MPI now treats $F77 and $FC the same, meaning that we
    # expect them to be the same back-end compiler.  If they're not,
    # results are undefined.  We do a cursory check to see that FC and
    # F77 are the same string value (if they're defined).  If they're
    # not, we'll issue a warning, but keep going on the assumption
    # that they're the same back-end compiler (e.g., pgf77 and pgf90).
    # Open MPI only uses $FC and $FCFLAGS -- $F77 and $FFLAGS are now
    # ignored.
    AS_IF([test "$F77" != "" -o "$FFLAGS" != ""],
          [AC_MSG_WARN([Open MPI now ignores the F77 and FFLAGS environment variables; only the FC and FCFLAGS environment variables are used.])
           sleep 5])

    #-----------------------------------------------------------------------
    # If we want any of the Fortran MPI bindings, setup the Fortran compiler
    #-----------------------------------------------------------------------
    ompi_fortran_happy=0
    # $LN_S is used below
    AC_PROG_LN_S

    ompi_fortran_double_underscore=0
    ompi_fortran_single_underscore=0
    ompi_fortran_caps=0
    ompi_fortran_plain=0

    AS_IF([test $OMPI_WANT_FORTRAN_BINDINGS -eq 1],
          [OMPI_SETUP_FC([ompi_fortran_happy=1])])

    # These values will be determined by SETUP_FC.  We must always
    # AC_DEFINE these results, even in the --disable-mpi-fortran case,
    # for ompi_info.
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_DOUBLE_UNDERSCORE],
        [$ompi_fortran_double_underscore], 
        [Whether fortran symbols have a trailing double underscore or not])
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_SINGLE_UNDERSCORE], 
        [$ompi_fortran_single_underscore],
        [Whether fortran symbols have a trailing underscore or not])
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_CAPS],
        [$ompi_fortran_caps],
        [Whether fortran symbols are all caps or not])
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_PLAIN], 
        [$ompi_fortran_plain],
        [Whether fortran symbols have no trailing underscore or not])

    # Check to see if any of the MPI Fortran bindings were
    # specifically requested.  If so, and we weren't able to setup the
    # Fortran compiler properly, it's an error.
    AS_IF([test $ompi_fortran_happy -eq 0 -a "$enable_mpi_fortran" = "yes"],
          [AC_MSG_WARN([MPI Fortran bindings requested, but no suitable Fortran compiler found])
          AC_MSG_ERROR([Cannot continue])])

    # This allows us to mark bogus types, but still have them be a valid
    # [sentinel] value
    AC_DEFINE([ompi_fortran_bogus_type_t], [int],
              [A bogus type that allows us to have sentinel type values that are still valid])

    # These get filled in as we check for each type
    OMPI_FORTRAN_IKINDS=
    OMPI_FORTRAN_RKINDS=
    OMPI_FORTRAN_CKINDS=

    # We want to set the #define's for all of these, so invoke the macros
    # regardless of whether we have F77 support or not.
    OMPI_FORTRAN_CHECK([CHARACTER], [yes],
                   [char, int32_t, int, int64_t, long long, long], [-1], [yes])
    
    OMPI_FORTRAN_CHECK([LOGICAL], [yes],
                   [char, int32_t, int, int64_t, long long, long], [-1], [yes])
    OMPI_FORTRAN_CHECK([LOGICAL*1], [yes],
                   [char, int8_t, short, int32_t, int, int64_t, long long, long], [1], [yes])
    OMPI_FORTRAN_CHECK([LOGICAL*2], [yes],
                   [short, int16_t, int32_t, int, int64_t, long long, long], [2], [yes])
    OMPI_FORTRAN_CHECK([LOGICAL*4], [yes],
                   [int32_t, int, int64_t, long long, long], [4], [yes])
    OMPI_FORTRAN_CHECK([LOGICAL*8], [yes],
                   [int, int64_t, long long, long], [8], [yes])
    
    OMPI_FORTRAN_CHECK([INTEGER], [yes],
                   [int32_t, int, int64_t, long long, long], [-1], [yes])
    OMPI_FORTRAN_CHECK([INTEGER*1], [no],
                   [char, int8_t, short, int, int64_t, long long, long], [1], [yes])
    OMPI_FORTRAN_CHECK([INTEGER*2], [no],
                   [short, int16_t, int32_t, int, int64_t, long long, long], [2], [yes])
    OMPI_FORTRAN_CHECK([INTEGER*4], [no],
                   [int32_t, int, int64_t, long long, long], [4], [yes])
    OMPI_FORTRAN_CHECK([INTEGER*8], [no],
                   [int, int64_t, long long, long], [8], [yes])
    OMPI_FORTRAN_CHECK([INTEGER*16], [no],
                   [int, int64_t, long long, long], [16], [yes])
    
    OMPI_FORTRAN_CHECK([REAL], [yes],
                   [float, double, long double], [-1], [yes])
    OMPI_FORTRAN_CHECK([REAL*2], [no],
                   [float, double, long double], [2], [yes])
    OMPI_FORTRAN_CHECK([REAL*4], [no],
                   [float, double, long double], [4], [yes])
    OMPI_FORTRAN_CHECK([REAL*8], [no],
                   [float, double, long double], [8], [yes])
    OMPI_FORTRAN_CHECK([REAL*16], [no],
                   [float, double, long double], [16], [yes])
    
    # In some compilers, the bit representation of REAL*16 is not the same
    # as the C counterpart that we found.  If this is the case, then we
    # want to disable reduction support for MPI_REAL16 (per ticket #1603).
    OMPI_FORTRAN_CHECK_REAL16_C_EQUIV
    
    OMPI_FORTRAN_CHECK([DOUBLE PRECISION], [yes],
                   [float, double, long double], [-1], [yes])
    
    OMPI_FORTRAN_CHECK([COMPLEX], [yes], [float _Complex], [-1], [no])

    # The complex*N tests are a bit different (note: the complex tests are
    # the same as all the rest, because complex is a composite of two
    # reals, which we *have* to have.  It's only the complex*N tests that
    # are different).  The fortran complex types are composites of the
    # real*(N/2) types.  So for us to support complex*N, two conditions
    # must be true:
    #
    # a) we must support real*(N/2) (i.e., compiler supports it and we
    #    have a back-end C type for it)
    # b) compiler supports complex*N
    
    OMPI_FORTRAN_CHECK([COMPLEX*4], [no], [float _Complex], [4], [no])
    OMPI_FORTRAN_CHECK([COMPLEX*8], [no], 
                   [float _Complex, double _Complex, long double _Complex],
                   [8], [no])
    OMPI_FORTRAN_CHECK([COMPLEX*16], [no], 
                   [float _Complex, double _Complex, long double _Complex], 
                   [16], [no])
    OMPI_FORTRAN_CHECK([COMPLEX*32], [no], 
                   [float _Complex, double _Complex, long double _Complex],
                   [32], [no])
    # Double precision complex types are not standard, but many
    # compilers support it.  Code should be wrapped with #ifdef
    # OMPI_HAVE_FORTRAN_DOUBLE_COMPLEX
    OMPI_FORTRAN_CHECK([DOUBLE COMPLEX], [no],
                   [float _Complex, double _Complex, long double _Complex], 
                   [-1], [no])
    
    # Regardless of whether we have fortran bindings, or even a
    # fortran compiler, get the max value for a fortran MPI handle
    # (this macro handles the case where we don't have a fortran
    # compiler).  
    OMPI_FORTRAN_GET_HANDLE_MAX

    # Check for Fortran compilers value of TRUE and for the correct
    # assumption on LOGICAL for conversion into what C considers to be
    # a true value.  
    OMPI_FORTRAN_GET_VALUE_TRUE
    OMPI_FORTRAN_CHECK_LOGICAL_ARRAY
    
    # How big should MPI_STATUS_SIZE be?  (i.e., the size of
    # MPI_STATUS, expressed in units of Fortran INTEGERs).  The C
    # equivalent of MPI_Status contains 4 C ints and a size_t.

    # Per the thread starting here:
    # http://www.open-mpi.org/community/lists/users/2013/10/22882.php,
    # the size was computed wrong for a long time.  However, we can't
    # change the size in the middle of the v1.6 series because it
    # would break ABI.  Hence, we have to add an additional,
    # non-default configure switch that allows users to intentionally
    # break ABI if they want to.
    AC_ARG_ENABLE([abi-breaking-fortran-status-i8-fix],
        AC_HELP_STRING([--enable-abi-breaking-fortran-status-i8-fix],
            [If the size of Fortran INTEGERs are larger than C ints, problems can occur with MPI_STATUS_SIZE.  Enabling this option fixes the issue, but breaks ABI with the rest of the v1.5/v1.6 series.  Note that if you use MPI_STATUS_IGNORE in your Fortran application and never examine an MPI_Status INTEGER array, you do not need this fix (default: disabled).]))
    size=$OMPI_SIZEOF_FORTRAN_INTEGER
    AS_IF([test "$enable_abi_breaking_fortran_status_i8_fix" = "yes"],
          [size=$ac_cv_sizeof_int])

    OMPI_FORTRAN_STATUS_SIZE=0
    AC_MSG_CHECKING([for the value of MPI_STATUS_SIZE])
    bytes=`expr 4 \* $ac_cv_sizeof_int + $ac_cv_sizeof_size_t`
    num_integers=`expr $bytes / $size`
    sanity=`expr $num_integers \* $size`
    AS_IF([test "$sanity" != "$bytes"],
          [AC_MSG_RESULT([unknown!])
           AC_MSG_WARN([WARNING: Size of C int: $ac_cv_sizeof_int])
           AC_MSG_WARN([WARNING: Size of C size_t: $ac_cv_sizeof_size_t])
           AC_MSG_WARN([WARNING: Size of Fortran INTEGER: $OMPI_SIZEOF_FORTRAN_INTEGER])
           AC_MSG_WARN([Could not make this work out evenly...!])
           AC_MSG_ERROR([Cannot continue])])
    OMPI_FORTRAN_STATUS_SIZE=$num_integers
    AC_MSG_RESULT([$OMPI_FORTRAN_STATUS_SIZE Fortran INTEGERs])
    AC_SUBST(OMPI_FORTRAN_STATUS_SIZE)

    # Setup for the compilers that don't support ignore TKR functionality
    OPAL_UNIQ(OMPI_FORTRAN_IKINDS)
    AC_SUBST(OMPI_FORTRAN_IKINDS)
    OPAL_UNIQ(OMPI_FORTRAN_RKINDS)
    AC_SUBST(OMPI_FORTRAN_RKINDS)
    OPAL_UNIQ(OMPI_FORTRAN_CKINDS)
    AC_SUBST(OMPI_FORTRAN_CKINDS)

    # We can't use C_INTxx_T KIND values in mpif.h because many
    # existing MPI Fortran applications are of the form:
    #
    #   program main
    #   implicit none
    #   include 'mpif.h'
    #
    # ...and you can't have a "use..." statement before that (to get
    # the Fortran/C interop C_INTxx_T KIND values).  So figure out
    # those KIND values here and just substitue them in via
    # AC_DEFINE's.  Kinda gross, but there you are.  :-\
    OMPI_FORTRAN_GET_KIND_VALUE([C_INT16_T], 4, [OMPI_FORTRAN_C_INT16_T_KIND])
    OMPI_FORTRAN_GET_KIND_VALUE([C_INT32_T], 9, [OMPI_FORTRAN_C_INT32_T_KIND])
    OMPI_FORTRAN_GET_KIND_VALUE([C_INT64_T], 18, [OMPI_FORTRAN_C_INT64_T_KIND])

    # Get the kind value for Fortran MPI_INTEGER_KIND (corresponding
    # to whatever is the same size as a F77 INTEGER -- for the
    # most-likely-will-never-occur case where F77 INTEGER is smaller
    # than an F90 INTEGER; see MPI-2 4.12.6.5.  As with OMPI
    # FORTRAN_CHECK, use the official BIND(C) KIND names (see comment
    # in fortran_check.m4).
    if test $OMPI_SIZEOF_FORTRAN_INTEGER -eq 2; then
        OMPI_MPI_INTEGER_KIND=$OMPI_FORTRAN_C_INT16_T_KIND
    elif test $OMPI_SIZEOF_FORTRAN_INTEGER -eq 4; then
        OMPI_MPI_INTEGER_KIND=$OMPI_FORTRAN_C_INT32_T_KIND
    elif test $OMPI_SIZEOF_FORTRAN_INTEGER -eq 8; then
        OMPI_MPI_INTEGER_KIND=$OMPI_FORTRAN_C_INT64_T_KIND
    elif test $OMPI_SIZEOF_FORTRAN_INTEGER -eq 16; then
        AC_MSG_ERROR([Cannot support Fortran MPI_INTEGER_KIND!])
    fi
    AC_SUBST(OMPI_MPI_INTEGER_KIND)

    # Get the kind value for Fortran MPI_ADDRESS_KIND (corresponding
    # to whatever is big enough to hold (void*)).
    if test $ac_cv_sizeof_void_p = 2 ; then
        OMPI_MPI_ADDRESS_KIND=$OMPI_FORTRAN_C_INT16_T_KIND
    elif test $ac_cv_sizeof_void_p = 4 ; then
        OMPI_MPI_ADDRESS_KIND=$OMPI_FORTRAN_C_INT32_T_KIND
    elif test $ac_cv_sizeof_void_p = 8 ; then
        OMPI_MPI_ADDRESS_KIND=$OMPI_FORTRAN_C_INT64_T_KIND
    elif test $ac_cv_sizeof_void_p = 16 ; then
        AC_MSG_ERROR([Cannot support Fortran MPI_ADDRESS_KIND!])
    fi
    AC_SUBST(OMPI_MPI_ADDRESS_KIND)
    
    # Get the kind value for Fortran MPI_OFFSET_KIND (corresponding to
    # INTEGER*8).
    OMPI_MPI_OFFSET_KIND=$OMPI_FORTRAN_C_INT64_T_KIND
    AC_SUBST(OMPI_MPI_OFFSET_KIND)

    # Get the kind value for Fortran MPI_COUNT_KIND (corresponding to 
    # INTEGER*8) 
    OMPI_MPI_COUNT_KIND=$OMPI_FORTRAN_C_INT64_T_KIND 
    AC_SUBST(OMPI_MPI_COUNT_KIND) 

    #--------------------------------------------------------
    # This is all we need for the Fortran mpif.h MPI bindings
    #--------------------------------------------------------

    AC_MSG_CHECKING([if building Fortran mpif.h bindings])
    AS_IF([test $ompi_fortran_happy -eq 1],
          [AC_MSG_RESULT([yes])
           OMPI_BUILD_FORTRAN_MPIFH_BINDINGS=1],
          [AC_MSG_RESULT([no])])
    
    #--------------------------------------------
    # Fortran use mpi or use mpi_f08 MPI bindings
    #--------------------------------------------

    AS_IF([test $ompi_fortran_happy -eq 1 -a \
              \( $OMPI_WANT_FORTRAN_USEMPI_BINDINGS -eq 1 -o \
                 $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 \)],
          [ # Look for the fortran module compiler flag
           OMPI_FORTRAN_FIND_MODULE_INCLUDE_FLAG([], 
               [AC_MSG_WARN([*** Could not determine the fortran compiler flag to indicate where modules reside])
                AC_MSG_ERROR([*** Cannot continue])])

           # This is all we need to build the "use mpi" module.  It's
           # an extra bonus if we have ignore TKR functionality (i.e.,
           # we'll build the "use mpi" module *better* if we have
           # ignore TKR, but we can build it with the legacy
           # implementation if we don't).
           OMPI_BUILD_FORTRAN_USEMPI_BINDINGS=1
           OMPI_FORTRAN_USEMPI_DIR=mpi/fortran/use-mpi-tkr
           OMPI_FORTRAN_USEMPI_LIB=-lmpi_usempi

           # Look for ignore TKR syntax
           OMPI_FORTRAN_CHECK_IGNORE_TKR(
               [OMPI_FORTRAN_HAVE_IGNORE_TKR=1

                OMPI_FORTRAN_USEMPI_DIR=mpi/fortran/use-mpi-ignore-tkr
                OMPI_FORTRAN_USEMPI_LIB=-lmpi_usempi_ignore_tkr

                # We now know that we can now build the mpi module
                # with at least ignore TKR functionality
                OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS=1
                OMPI_FORTRAN_F08_PREDECL=$OMPI_FORTRAN_IGNORE_TKR_PREDECL
                OMPI_FORTRAN_F08_TYPE=$OMPI_FORTRAN_IGNORE_TKR_TYPE])
          ])
    AC_MSG_CHECKING([if building Fortran 'use mpi' bindings])
    AS_IF([test $OMPI_BUILD_FORTRAN_USEMPI_BINDINGS -eq 1],
          [AC_MSG_RESULT([yes])],
          [AC_MSG_RESULT([no])])
    
    #---------------------------------
    # Fortran use mpi_f08 MPI bindings
    #---------------------------------

    # If we got all the stuff from above, then also look for the new
    # F08 syntax that we can use for the use_mpif08 module.
    OMPI_FORTRAN_HAVE_BIND_C=0
    AS_IF([test $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 -a \
           $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [ # If we don't have BIND(C), we won't build mpi_f08 at all
           OMPI_FORTRAN_CHECK_BIND_C(
               [OMPI_FORTRAN_HAVE_BIND_C=1],
               [OMPI_FORTRAN_HAVE_BIND_C=0
                OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS=0])])

    # Check whether BIND(C) can be used with default LOGICAL
    # parameters (per https://svn.open-mpi.org/trac/ompi/ticket/3523
    # and http://lists.mpi-forum.org/mpi-comments/2013/02/0076.php).
    # For the moment (Feb 2013), MPI-3 says we have to use LOGICAL
    # (not LOGICAL(BIND=C_BOOL)).  So if the compiler doesn't allow
    # this behavior, then disable the F08 bindings.
    OMPI_FORTRAN_BINDC_LIKES_LOGICAL=0
    AS_IF([test $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 -a \
           $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [OMPI_FORTRAN_CHECK_BIND_C_LOGICAL(
               [OMPI_FORTRAN_BIND_C_LIKES_LOGICAL=1],
               [OMPI_FORTRAN_BIND_C_LIKES_LOGICAL=0
                OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS=0])])

    OMPI_FORTRAN_HAVE_OPTIONAL_ARGS=0
    AS_IF([test $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 -a \
           $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [ # Does the compiler have optional arguments?
           OMPI_FORTRAN_CHECK_OPTIONAL_ARGS(
               [OMPI_FORTRAN_HAVE_OPTIONAL_ARGS=1],
               [OMPI_FORTRAN_HAVE_OPTIONAL_ARGS=0
                OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS=0])])

    OMPI_FORTRAN_HAVE_PRIVATE=0
    AS_IF([test $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 -a \
           $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [ # Does the compiler support the "private" 
           OMPI_FORTRAN_CHECK_PRIVATE(
               [OMPI_FORTRAN_HAVE_PRIVATE=1],
               [OMPI_FORTRAN_HAVE_PRIVATE=0])])

    OMPI_FORTRAN_HAVE_ABSTRACT=0
    AS_IF([test $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 -a \
           $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [ # Does the compiler support the "abstract" 
           OMPI_FORTRAN_CHECK_ABSTRACT(
               [OMPI_FORTRAN_HAVE_ABSTRACT=1],
               [OMPI_FORTRAN_HAVE_ABSTRACT=0])])

    OMPI_FORTRAN_HAVE_ASYNCHRONOUS=0
    AS_IF([test $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 -a \
           $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [ # Does the compiler support the "asynchronous" 
           OMPI_FORTRAN_CHECK_ASYNCHRONOUS(
               [OMPI_FORTRAN_HAVE_ASYNCHRONOUS=1],
               [OMPI_FORTRAN_HAVE_ASYNCHRONOUS=0])])

    OMPI_FORTRAN_HAVE_PROCEDURE=0
    AS_IF([test $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 -a \
           $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [ # Does the compiler support the "procedure" 
           OMPI_FORTRAN_CHECK_PROCEDURE(
               [OMPI_FORTRAN_HAVE_PROCEDURE=1],
               [OMPI_FORTRAN_HAVE_PROCEDURE=0])])

    OMPI_FORTRAN_F08_HANDLE_SIZE=4
    AS_IF([test $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 -a \
           $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [ # How big are derived types with a single INTEGER?
           OMPI_FORTRAN_GET_SIZEOF([type, BIND(C) :: test_mpi_handle
  integer :: MPI_VAL
end type test_mpi_handle], 
                                   [type(test_mpi_handle)], 
                                   [OMPI_FORTRAN_F08_HANDLE_SIZE])
          ])

    OMPI_FORTRAN_NEED_WRAPPER_ROUTINES=1
    OMPI_FORTRAN_F08_PREDECL='!'
    OMPI_FORTRAN_F08_TYPE=real
    OMPI_FORTRAN_HAVE_F08_ASSUMED_RANK=0
    AS_IF([test $OMPI_WANT_FORTRAN_USEMPIF08_BINDINGS -eq 1 -a \
           $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [ # Look for Fortran 2008 assumed rank syntax
           OMPI_FORTRAN_CHECK_F08_ASSUMED_RANK(
               [ # If we have assumed rank, we can build the use
                 # mpi_f08 module "better"
                OMPI_FORTRAN_F08_PREDECL='!'
                OMPI_FORTRAN_F08_TYPE='type(*), dimension(..)'
                OMPI_FORTRAN_HAVE_F08_ASSUMED_RANK=1])

               # Which mpi_f08 implementation are we using?
               # a) partial, proof-of-concept that supports array
               #    subsections (Intel compiler only)
               # b) compiler supports BIND(C) and optional arguments
               #    ("good" compilers)
               # c) compiler that does not support the items listed
               #    in b) ("bad" compilers)
    
               AC_MSG_CHECKING([which mpi_f08 implementation to build])
               AS_IF([test $OMPI_BUILD_FORTRAN_F08_SUBARRAYS -eq 1],
                     [ # Case a) partial/prototype implementation
                      OMPI_FORTRAN_USEMPIF08_DIR=mpi/fortran/use-mpi-f08-desc
                      OMPI_FORTRAN_SUBARRAYS_SUPPORTED=.TRUE.
                      OMPI_FORTRAN_NEED_WRAPPER_ROUTINES=0
                      AC_MSG_RESULT([array subsections (partial/experimental)])
                     ],
                     [ # Both cases b) and c)
                      OMPI_FORTRAN_USEMPIF08_DIR=mpi/fortran/use-mpi-f08
                      OMPI_FORTRAN_SUBARRAYS_SUPPORTED=.FALSE.
                      AS_IF([test $OMPI_FORTRAN_HAVE_OPTIONAL_ARGS -eq 1],
                            [ # Case b) "good compiler"
                             OMPI_FORTRAN_NEED_WRAPPER_ROUTINES=0
                             AC_MSG_RESULT(["good" compiler, no array subsections])
                            ],
                            [ # Case c) "bad compiler"
                             OMPI_FORTRAN_NEED_WRAPPER_ROUTINES=1
                             AC_MSG_RESULT(["bad" compiler, no array subsections])
                            ])
                      # Override for the moment: we're using wrappers
                      # for everything right now.  We'll optimize this
                      # later to avoid the use of wrappers for "good"
                      # compilers where possible (e.g., for functions
                      # without character/string parameters).
                      AC_MSG_WARN([Temporary development override: forcing the use of F08 wrappers])
                      OMPI_FORTRAN_NEED_WRAPPER_ROUTINES=1
                     ])
          ])
    AC_MSG_CHECKING([if building Fortran 'use mpi_f08' bindings])
    AS_IF([test $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1],
          [OMPI_FORTRAN_USEMPIF08_LIB=-lmpi_usempif08
           AC_MSG_RESULT([yes])],
          [AC_MSG_RESULT([no])])

    # -------------------
    # mpif.h final setup
    # -------------------

    # A preprocessor header file just for Fortran.  We cannot use AC
    # CONFIG_HEADER because it adds a /* */-style comment at the top,
    # and this header file must be usable in .F90 files.  :-(
    AC_CONFIG_FILES([ompi/mpi/fortran/configure-fortran-output.h])

    # Values for wrapper compilers    
    OMPI_FC=$FC
    set dummy $OMPI_FC
    OMPI_FC_ARGV0=[$]2
    AS_IF([test -n "$OMPI_FC_ARGV0"],
          [BASEFC="`basename $OMPI_FC_ARGV0`"
           OPAL_WHICH([$OMPI_FC_ARGV0], [OMPI_FC_ABSOLUTE])],
          [OMPI_FC=none
           BASEFC=none
           OMPI_FC_ABSOLUTE=none])

    AC_SUBST(OMPI_FC)
    AC_SUBST(OMPI_FC_ABSOLUTE)
    AC_DEFINE_UNQUOTED(OMPI_FC, ["$OMPI_FC"], [Underlying Fortran compiler])
    AC_DEFINE_UNQUOTED(OMPI_FC_ABSOLUTE, ["$OMPI_FC_ABSOLUTE"],
                       [Absolutey path to the underlying Fortran compiler found by configure])

    # There are 2 layers to the MPI mpif.h layer. The only extra thing
    # that determine mpif.h bindings is that fortran can be disabled
    # by user. In such cases, we need to not build the target at all.
    # One layer generates MPI_<foo> bindings. The other layer
    # generates PMPI_<foo> bindings. The following conditions
    # determine whether each (or both) these layers are built.
    #
    # Superceeding clause:
    #   - Fortran bindings should be enabled, else everything is
    #     disabled
    # 1. MPI_<foo> bindings are needed if:
    #   - Profiling is not required
    #   - Profiling is required but weak symbols are not supported
    # 2. PMPI_<foo> bindings are needed if profiling is required.
    #
    # Hence we define 2 conditionals which tell us whether each of
    # these layers need to be built or NOT

    AM_CONDITIONAL(BUILD_MPI_FORTRAN_MPIFH_BINDINGS_LAYER,
                   [test \( $WANT_MPI_PROFILING -eq 0 -o $OMPI_PROFILING_COMPILE_SEPARATELY -eq 1 \) -a $OMPI_BUILD_FORTRAN_MPIFH_BINDINGS -eq 1])
    AM_CONDITIONAL(BUILD_PMPI_FORTRAN_MPIFH_BINDINGS_LAYER,
                   [test $OMPI_BUILD_FORTRAN_MPIFH_BINDINGS -eq 1 -a $WANT_MPI_PROFILING -eq 1])
    AC_DEFINE_UNQUOTED(OMPI_BUILD_FORTRAN_MPIFH_BINDINGS,
                       $OMPI_BUILD_FORTRAN_MPIFH_BINDINGS,
                       [Whether we will build the MPI Fortran mpif.h bindings or not])
    AM_CONDITIONAL(OMPI_BUILD_FORTRAN_MPIFH_BINDINGS, 
                   [test $OMPI_BUILD_FORTRAN_MPIFH_BINDINGS -eq 1])

    # -------------------
    # use mpi final setup
    # -------------------

    # This goes into ompi/Makefile.am
    AC_SUBST(OMPI_FORTRAN_USEMPI_DIR)
    # This goes into mpifort-wrapper-data.txt
    AC_SUBST(OMPI_FORTRAN_USEMPI_LIB)

    # These go into mpi-ignore-tkr-interfaces.h / mpi-ignore-tkr-file-interfaces.h
    AC_SUBST(OMPI_FORTRAN_IGNORE_TKR_PREDECL)
    AC_SUBST(OMPI_FORTRAN_IGNORE_TKR_TYPE)

    # These go into ompi/info/param.c
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_IGNORE_TKR_PREDECL],
                       ["$OMPI_FORTRAN_IGNORE_TKR_PREDECL"],
                       [Pre declaration for FORTRAN ignore parameter TKR behavior])
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_IGNORE_TKR_TYPE],
                       [$type],
                       [Type declaration for FORTRAN ignore parameter TKR behavior])
    AC_DEFINE_UNQUOTED(OMPI_BUILD_FORTRAN_USEMPI_BINDINGS, 
                       $OMPI_BUILD_FORTRAN_USEMPI_BINDINGS,
                       [Whether we will build the MPI Fortran "use mpi" bindings or not])
    AC_DEFINE_UNQUOTED(OMPI_FORTRAN_MODULE_SIZE,
                       ["$OMPI_FORTRAN_MODULE_SIZE"],
                       [Only relevant for Fortran compilers that do not support ignore TKR functionality: size of the "use mpi" and "use mpi_f08" modules])
    AC_DEFINE_UNQUOTED(OMPI_FORTRAN_HAVE_IGNORE_TKR,
                       [$OMPI_FORTRAN_HAVE_IGNORE_TKR],
                       [Whether the Fortran compiler supports ignore TKR functionality or not])

    # Somewhat redundant because ompi/Makefile.am won't traverse into
    # the unused "use mpi" directory, but we might as well have the
    # ompi/mpi/fortran/use-mpi*/Makefile.ams be safe, too.
    # True if we're building either "use mpi" bindings
    AM_CONDITIONAL(OMPI_BUILD_FORTRAN_USEMPI_BINDINGS, 
                   [test $OMPI_BUILD_FORTRAN_USEMPI_BINDINGS -eq 1 -o \
                         $OMPI_FORTRAN_HAVE_IGNORE_TKR -eq 1])
    # True if we're building the old TKR-style bindings
    AM_CONDITIONAL(OMPI_BUILD_FORTRAN_USEMPI_TKR_BINDINGS, 
                   [test $OMPI_BUILD_FORTRAN_USEMPI_BINDINGS -eq 1 -a \
                         $OMPI_FORTRAN_HAVE_IGNORE_TKR -eq 0])
    # True if we're building the new ignore-TKR-style bindings
    AM_CONDITIONAL(OMPI_BUILD_FORTRAN_USEMPI_IGNORE_TKR_BINDINGS, 
                   [test $OMPI_BUILD_FORTRAN_USEMPI_BINDINGS -eq 1 -a \
                         $OMPI_FORTRAN_HAVE_IGNORE_TKR -eq 1])

    # -------------------
    # use mpi_f08 final setup
    # -------------------

    # This goes into ompi/Makefile.am
    AC_SUBST(OMPI_FORTRAN_USEMPIF08_DIR)
    # This goes into mpifort-wrapper-data.txt
    AC_SUBST(OMPI_FORTRAN_USEMPIF08_LIB)

    # These go into interfaces/mpi-f08-interfaces-[no]bind.h (and
    # mpi-f*-interfaces*.h files)
    AC_SUBST(OMPI_FORTRAN_F08_PREDECL)
    AC_SUBST(OMPI_FORTRAN_F08_TYPE)

    AC_SUBST(OMPI_MPI_PREFIX)
    AC_SUBST(OMPI_MPI_BIND_PREFIX)
    AC_SUBST(OMPI_F08_SUFFIX)
    AC_SUBST(OMPI_F_SUFFIX)

    # This goes into ompi/mpi/fortran/configure-fortran-output.h
    AC_SUBST(OMPI_FORTRAN_SUBARRAYS_SUPPORTED)
    AC_DEFINE_UNQUOTED(OMPI_FORTRAN_SUBARRAYS_SUPPORTED,
                       [$OMPI_FORTRAN_SUBARRAYS_SUPPORTED],
                       [Value to load to the MPI_SUBARRAYS_SUPPORTED compile-time constant])

    # This is used to generate weak symbols (or not) in
    # ompi/mpi/fortran/mpif-h/<foo>_f.c, and
    # ompi/mpi/fortran/configure-fortran-output.h.
    AC_SUBST(OMPI_FORTRAN_NEED_WRAPPER_ROUTINES)
    AC_DEFINE_UNQUOTED(OMPI_FORTRAN_NEED_WRAPPER_ROUTINES,
                       [$OMPI_FORTRAN_NEED_WRAPPER_ROUTINES],
                       [Whether the mpi_f08 implementation is using wrapper routines ("bad" Fortran compiler) or weak symbols ("good" Fortran compiler) for the F08 interface definition implementations])

    AC_DEFINE_UNQUOTED(OMPI_FORTRAN_F08_HANDLE_SIZE, 
                       $OMPI_FORTRAN_F08_HANDLE_SIZE,
                       [How many bytes the mpi_f08 TYPE(MPI_<foo>) handles will be])

    # These go into ompi/info/param.c
    AC_DEFINE_UNQUOTED(OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS, 
                       $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS,
                       [For ompi_info: Whether we will build the MPI Fortran "use mpi_f08" bindings or not])
    AC_DEFINE_UNQUOTED(OMPI_FORTRAN_HAVE_F08_ASSUMED_RANK,
                       [$OMPI_FORTRAN_HAVE_F08_ASSUMED_RANK],
                       [For ompi_info: Whether the Fortran compiler supports the Fortran 2008 "assumed rank" syntax or not])
    AC_DEFINE_UNQUOTED(OMPI_FORTRAN_HAVE_BIND_C,
                       [$OMPI_FORTRAN_HAVE_BIND_C],
                       [For ompi_info: Whether we want to use BIND(C) in the mpi_f08 module or not (based on "good" or "bad" compiler determination, i.e., whether we are using fortran wrapper functions for choice buffers or not)])
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_HAVE_OPTIONAL_ARGS], 
                       [$OMPI_FORTRAN_HAVE_OPTIONAL_ARGS],
                       [For ompi_info: whether the Fortran compiler supports optional arguments or not])

    # For configure-fortran-output.h, mpi-f08-types.F90 (and ompi_info)
    AC_SUBST([OMPI_FORTRAN_HAVE_PRIVATE])
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_HAVE_PRIVATE], 
                       [$OMPI_FORTRAN_HAVE_PRIVATE],
                       [For mpi-f08-types.f90 and ompi_info: whether the compiler supports the "private" keyword or not (used in MPI_Status)])

    # For configure-fortran-output.h, mpi-f08-interfaces-callbacks.F90
    # (and ompi_info)
    AC_SUBST([OMPI_FORTRAN_HAVE_ABSTRACT])
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_HAVE_ABSTRACT], 
                       [$OMPI_FORTRAN_HAVE_ABSTRACT],
                       [For mpi-f08-interfaces-callbacks.f90 and ompi_info: whether the compiler supports the "abstract" keyword or not])

    # For configure-fortran-output.h, various files in
    # ompi/mpi/fortran/use-mpi-f08/*.F90 and *.h files (and ompi_info)
    AC_SUBST([OMPI_FORTRAN_HAVE_ASYNCHRONOUS])
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_HAVE_ASYNCHRONOUS], 
                       [$OMPI_FORTRAN_HAVE_ASYNCHRONOUS],
                       [For ompi/mpi/fortran/use-mpi-f08/blah.F90 and blah.h and ompi_info: whether the compiler supports the "asynchronous" keyword or not])

    # For configure-fortran-output.h, various files in
    # ompi/mpi/fortran/use-mpi-f08/*.F90 and *.h files (and ompi_info)
    AC_SUBST([OMPI_FORTRAN_HAVE_PROCEDURE])
    AC_DEFINE_UNQUOTED([OMPI_FORTRAN_HAVE_PROCEDURE], 
                       [$OMPI_FORTRAN_HAVE_PROCEDURE],
                       [For ompi/mpi/fortran/use-mpi-f08/blah.F90 and blah.h and ompi_info: whether the compiler supports the "procedure" keyword or not])

    # For configure-fortran-output.h
    AC_SUBST(OMPI_FORTRAN_HAVE_BIND_C)

    # Somewhat redundant because ompi/Makefile.am won't traverse into
    # ompi/mpi/fortran/use-mpi-f08 if it's not to be built, but we
    # might as well have ompi/mpi/fortran/use-mpi-f08/Makefile.am be
    # safe, too.
    AM_CONDITIONAL(OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS, 
                   [test $OMPI_BUILD_FORTRAN_USEMPIF08_BINDINGS -eq 1])
])
