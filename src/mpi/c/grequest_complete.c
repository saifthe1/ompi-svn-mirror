/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
#include "ompi_config.h"
#include <stdio.h>

#include "mpi.h"
#include "mpi/c/bindings.h"
#include "communicator/communicator.h"
#include "request/grequest.h"

#if OMPI_HAVE_WEAK_SYMBOLS && OMPI_PROFILING_DEFINES
#pragma weak MPI_Grequest_complete = PMPI_Grequest_complete
#endif

#if OMPI_PROFILING_DEFINES
#include "mpi/c/profile/defines.h"
#endif

static const char FUNC_NAME[] = "MPI_Grequest_complete";


int MPI_Grequest_complete(MPI_Request request) 
{
    int rc = MPI_SUCCESS;
    if (MPI_PARAM_CHECK) {
        OMPI_ERR_INIT_FINALIZE(FUNC_NAME);
        if (request == MPI_REQUEST_NULL) {
           rc = MPI_ERR_REQUEST;
        }
        OMPI_ERRHANDLER_CHECK(rc, MPI_COMM_WORLD, rc, FUNC_NAME);
    }

    switch(request->req_type) {
        case OMPI_REQUEST_GEN:
            rc = ompi_grequest_complete((ompi_grequest_t*)request);
            break;
        default:
            rc = MPI_ERR_REQUEST;
            break;
    }
    OMPI_ERRHANDLER_RETURN(rc, MPI_COMM_WORLD, MPI_ERR_INTERN, FUNC_NAME);
}

