/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
 *                         All rights reserved.
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

#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif  /* HAVE_UNIST_H */

#include "opal/mca/mca.h"
#include "opal/mca/base/base.h"

#include "opal/util/output.h"
#include "opal/mca/base/mca_base_param.h"

#include "opal/util/output.h"
#include "opal/util/show_help.h"
#include "opal/util/argv.h"
#include "opal/util/opal_environ.h"

#include "ompi/mca/crcp/crcp.h"
#include "ompi/mca/crcp/base/base.h"

#include "crcp_coord.h"
#include "crcp_coord_pml.h"
#include "crcp_coord_btl.h"

/*
 * Coord module
 */
static ompi_crcp_base_module_t loc_module = {
    /** Initialization Function */
    ompi_crcp_coord_module_init,
    /** Finalization Function */
    ompi_crcp_coord_module_finalize,

    /** PML Wrapper */
    ompi_crcp_coord_pml_enable,

    ompi_crcp_coord_pml_add_comm,
    ompi_crcp_coord_pml_del_comm,

    ompi_crcp_coord_pml_add_procs,
    ompi_crcp_coord_pml_del_procs,

    ompi_crcp_coord_pml_progress,

    ompi_crcp_coord_pml_iprobe,
    ompi_crcp_coord_pml_probe,

    ompi_crcp_coord_pml_isend_init,
    ompi_crcp_coord_pml_isend,
    ompi_crcp_coord_pml_send,

    ompi_crcp_coord_pml_irecv_init,
    ompi_crcp_coord_pml_irecv,
    ompi_crcp_coord_pml_recv,

    ompi_crcp_coord_pml_dump,
    ompi_crcp_coord_pml_start,

    ompi_crcp_coord_pml_ft_event,

    /* Request Functions */
    ompi_crcp_coord_request_complete,

    /* BTL Wrapper Functions */
    NULL, /* btl_add_procs */
    NULL, /* btl_del_procs */
    NULL, /* btl_register */
    NULL, /* btl_finalize */
    NULL, /* btl_alloc */
    NULL, /* btl_free */
    NULL, /* btl_prepare_src */
    NULL, /* btl_prepare_dst */
    NULL, /* btl_send */
    NULL, /* btl_put */
    NULL, /* btl_get */
    NULL, /* btl_dump */
    NULL /* btl_ft_event */
};

/************************************
 * Locally Global vars & functions :)
 ************************************/
    opal_mutex_t     ompi_crcp_coord_ft_global_cs_lock;
    opal_condition_t ompi_crcp_coord_ft_global_cs_cond;
    int              ompi_crcp_coord_ft_global_cs_count;
    bool             ompi_crcp_coord_ft_global_cs_block;

    opal_mutex_t     ompi_crcp_coord_ft_send_cs_lock;
    opal_condition_t ompi_crcp_coord_ft_send_cs_cond;
    int              ompi_crcp_coord_ft_send_cs_count;
    bool             ompi_crcp_coord_ft_send_cs_block;

    opal_mutex_t     ompi_crcp_coord_ft_recv_cs_lock;
    opal_condition_t ompi_crcp_coord_ft_recv_cs_cond;
    int              ompi_crcp_coord_ft_recv_cs_count;
    bool             ompi_crcp_coord_ft_recv_cs_block;

/************************
 * Function Definitions
 ************************/
/*
 * MCA Functions
 */
ompi_crcp_base_module_1_0_0_t *
ompi_crcp_coord_component_query(int *priority)
{
    opal_output_verbose(10, mca_crcp_coord_component.super.output_handle,
                        "crcp:coord: component_query()");

    *priority = mca_crcp_coord_component.super.priority;

    return &loc_module;
}

int ompi_crcp_coord_module_init(void)
{
    opal_output_verbose(10, mca_crcp_coord_component.super.output_handle,
                        "crcp:coord: module_init()");

    ompi_crcp_coord_pml_init();

    ompi_crcp_coord_btl_init();

    return OMPI_SUCCESS;
}

int ompi_crcp_coord_module_finalize(void)
{
    opal_output_verbose(10, mca_crcp_coord_component.super.output_handle,
                        "crcp:coord: module_finalize()");

    ompi_crcp_coord_pml_finalize();

    ompi_crcp_coord_btl_finalize();

    return OMPI_SUCCESS;
}

/******************
 * Local functions
 ******************/
