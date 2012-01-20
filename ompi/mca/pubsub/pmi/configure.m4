# -*- shell-script -*-
#
# Copyright (c) 2011      Cisco Systems, Inc.  All rights reserved.
# $COPYRIGHT$
# 
# Additional copyrights may follow
# 
# $HEADER$
#
# MCA_ompi_pubsub_pmi_CONFIG([action-if-found], [action-if-not-found])
# -----------------------------------------------------------
AC_DEFUN([MCA_ompi_pubsub_pmi_CONFIG], [
    AC_CONFIG_FILES([ompi/mca/pubsub/pmi/Makefile])
         
    ORTE_CHECK_PMI([pubsub_pmi], [pubsub_pmi_good=1], [pubsub_pmi_good=0])
         
    # if check worked, set wrapper flags if so.  
    # Evaluate succeed / fail
    AS_IF([test "$pubsub_pmi_good" = 1],
          [if test -z "$orte_pmi_wrapper_flags_added"; then
              orte_pmi_wrapper_flags_added=yes
              WRAPPER_EXTRA_LDFLAGS="$WRAPPER_EXTRA_LDFLAGS $pubsub_pmi_LDFLAGS"
              WRAPPER_EXTRA_LIBS="$WRAPPER_EXTRA_LIBS $pubsub_pmi_LIBS"
           fi
           $1],
          [$2])

    # set build flags to use in makefile
    AC_SUBST([pubsub_pmi_CPPFLAGS])
    AC_SUBST([pubsub_pmi_LDFLAGS])
    AC_SUBST([pubsub_pmi_LIBS])

])
