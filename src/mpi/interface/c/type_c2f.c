/*
 * $HEADERS$
 */
#include "lam_config.h"
#include <stdio.h>

#include "mpi.h"
#include "mpi/interface/c/bindings.h"

#if LAM_HAVE_WEAK_SYMBOLS && LAM_PROFILING_DEFINES
#pragma weak MPI_Type_c2f = PMPI_Type_c2f
#endif

MPI_Fint MPI_Type_c2f(MPI_Datatype datatype) {
    return (MPI_Fint)0;
}
