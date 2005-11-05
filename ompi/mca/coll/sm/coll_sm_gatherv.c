/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ompi_config.h"

#include "ompi/include/constants.h"
#include "coll_sm.h"


/*
 *	gatherv_intra
 *
 *	Function:	- basic gatherv operation
 *	Accepts:	- same arguments as MPI_Gatherb()
 *	Returns:	- MPI_SUCCESS or error code
 */
int mca_coll_sm_gatherv_intra(void *sbuf, int scount, 
                              struct ompi_datatype_t *sdtype,
                              void *rbuf, int *rcounts, int *disps,
                              struct ompi_datatype_t *rdtype, int root,
                              struct ompi_communicator_t *comm)
{
    return OMPI_ERR_NOT_IMPLEMENTED;
}
