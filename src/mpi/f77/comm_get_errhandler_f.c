/*
 * $HEADER$
 */

#include "lam_config.h"

#include <stdio.h>

#include "mpi.h"
#include "mpi/f77/bindings.h"

#if LAM_HAVE_WEAK_SYMBOLS && LAM_PROFILE_LAYER
#pragma weak PMPI_COMM_GET_ERRHANDLER = mpi_comm_get_errhandler_f
#pragma weak pmpi_comm_get_errhandler = mpi_comm_get_errhandler_f
#pragma weak pmpi_comm_get_errhandler_ = mpi_comm_get_errhandler_f
#pragma weak pmpi_comm_get_errhandler__ = mpi_comm_get_errhandler_f
#elif LAM_PROFILE_LAYER
LAM_GENERATE_F77_BINDINGS (PMPI_COMM_GET_ERRHANDLER,
                           pmpi_comm_get_errhandler,
                           pmpi_comm_get_errhandler_,
                           pmpi_comm_get_errhandler__,
                           pmpi_comm_get_errhandler_f,
                           (MPI_Fint *comm, MPI_Fint *erhandler, MPI_Fint *ierr),
                           (comm, erhandler, ierr) )
#endif

#if LAM_HAVE_WEAK_SYMBOLS
#pragma weak MPI_COMM_GET_ERRHANDLER = mpi_comm_get_errhandler_f
#pragma weak mpi_comm_get_errhandler = mpi_comm_get_errhandler_f
#pragma weak mpi_comm_get_errhandler_ = mpi_comm_get_errhandler_f
#pragma weak mpi_comm_get_errhandler__ = mpi_comm_get_errhandler_f
#endif

#if ! LAM_HAVE_WEAK_SYMBOLS && ! LAM_PROFILE_LAYER
LAM_GENERATE_F77_BINDINGS (MPI_COMM_GET_ERRHANDLER,
                           mpi_comm_get_errhandler,
                           mpi_comm_get_errhandler_,
                           mpi_comm_get_errhandler__,
                           mpi_comm_get_errhandler_f,
                           (MPI_Fint *comm, MPI_Fint *erhandler, MPI_Fint *ierr),
                           (comm, erhandler, ierr) )
#endif


#if LAM_PROFILE_LAYER && ! LAM_HAVE_WEAK_SYMBOLS
#include "mpi/c/profile/defines.h"
#endif

void mpi_comm_get_errhandler_f(MPI_Fint *comm, MPI_Fint *erhandler, MPI_Fint *ierr)
{

}
