/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 The Trustees of the University of Tennessee.
 *                         All rights reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "ompi_config.h"

#include "mpi.h"
#include "attribute/attribute.h"

#include "errhandler/errclass.h"
#include "communicator/communicator.h"
#include "util/proc_info.h"
#include "mca/ns/ns.h"
#include "mca/gpr/gpr.h"


/*
 * Private functions
 */
static int set(int keyval, void *value);


/*
 * Back-end for attribute values
 */
static int attr_tag_ub = MPI_TAG_UB_VALUE;
static char *attr_host = NULL;
static int attr_io = 1;
static int attr_wtime_is_global = 0;

/* Filled in at run-time, below */
static int attr_appnum = -1;
/* Filled in at run-time, below */
static int attr_universe_size = -1;

#if 0
/* JMS for when we implement windows */
static int attr_win_base = 0;
static int attr_win_size = 0;
static int attr_win_disp_unit = 0;
#endif

#if 0
/* JMS for when we implement IMPI */
static int attr_impi_client_size = 0;
static int attr_impi_client_color = 0;
static int attr_impi_host_size = 0;
static int attr_impi_host_color = 0;
#endif


int ompi_attr_create_predefined(void)
{
    orte_gpr_notify_id_t rc;
    int ret;

     ret = orte_gpr.subscribe(
         ORTE_GPR_OR,
	     ORTE_GPR_NOTIFY_ON_STARTUP|ORTE_GPR_NOTIFY_INCLUDE_STARTUP_DATA,
         ORTE_NODE_SEGMENT,
         NULL,
         NULL,
         &rc,
         ompi_attr_create_predefined_callback,
         NULL);
     if(ORTE_SUCCESS != ret) {
         ompi_output(0, "ompi_attr_create_predefined: subscribe failed");
         return OMPI_ERROR;
     }
     return OMPI_SUCCESS;
}


void ompi_attr_create_predefined_callback(
	orte_gpr_notify_message_t *msg,
	void *cbdata)
{
    int err;
    int32_t i, j;
    orte_gpr_keyval_t **keyval;
    orte_gpr_value_t **value;
    orte_jobid_t job;

    /* Set some default values */

    if (ORTE_SUCCESS != orte_ns.get_jobid(&job, orte_process_info.my_name)) {
        return;
    }

    /* Per conversation between Jeff, Edgar, and Ralph - this needs
     * to be fixed to properly determine the appnum
     */
    attr_appnum = (int)job;
    
    /* Query the gpr to find out how many CPUs there will be.
       This will only return a non-empty list in a persistent
       universe.  If we don't have a persistent universe, then just
       default to the size of MPI_COMM_WORLD. 

       JMS: I think we need more here -- there are cases where you
       wouldn't have a persistent universe but still may have a
       comm_size(COMM_WORLD) != UNIVERSE_SIZE.  For example, say you
       reserve 8 CPUs in a batch environment and then run ./master,
       where the master is supposed to SPAWN the other processes.
       Perhaps need some integration with the LLM here...?  [shrug] */

    /* RHC: Needed to change this code so it wouldn't issue a gpr.get
     * during the compound command phase of mpi_init. Since all you need
     * is to have the data prior to dtypes etc., and since this function
     * is called right before we send the compound command, I've changed
     * it to a subscription and a callback function. This allows you to
     * get the data AFTER the compound command executes. Nothing else
     * happens in-between anyway, so this shouldn't cause a problem.
     */

    attr_universe_size = 0;
    if (0 == msg->cnt) {  /* no data returned */
        attr_universe_size = ompi_comm_size(MPI_COMM_WORLD);
    } else {
        value = msg->values;
        for (i=0; i < msg->cnt; i++) {
            if (0 < value[i]->cnt) {  /* make sure some data was returned here */
                keyval = value[i]->keyvals;
                for (j=0; j < value[i]->cnt; j++) {
                    if (0 == strncmp(keyval[j]->key, "num_cpus", strlen("num_cpus"))) {
                        /* Process slot count */
                        attr_universe_size += keyval[j]->value.i16;
                    }
                }
            }
        }
    }
    OBJ_RELEASE(msg);

    /* DO NOT CHANGE THE ORDER OF CREATING THESE KEYVALS!  This order
       strictly adheres to the order in mpi.h.  If you change the
       order here, you must change the order in mpi.h as well! */

    if (OMPI_SUCCESS != (err = set(MPI_TAG_UB, &attr_tag_ub)) ||
        OMPI_SUCCESS != (err = set(MPI_HOST, &attr_host)) ||
        OMPI_SUCCESS != (err = set(MPI_IO, &attr_io)) ||
        OMPI_SUCCESS != (err = set(MPI_WTIME_IS_GLOBAL,
                                   &attr_wtime_is_global)) ||
        OMPI_SUCCESS != (err = set(MPI_APPNUM, &attr_appnum)) ||
        OMPI_SUCCESS != (err = set(MPI_LASTUSEDCODE,
                                   &ompi_errclass_lastused)) ||
        OMPI_SUCCESS != (err = set(MPI_UNIVERSE_SIZE, &attr_universe_size)) ||
#if 0
        /* JMS for when we implement windows */
        /* JMS BE SURE TO READ ALL OF MPI-2 4.12.7 BEFORE IMPLEMENTING
           THESE ADDRESS-VALUED ATTRIBUTES! */
        OMPI_SUCCESS != (err = set(MPI_WIN_BASE, &attr_win_base)) ||
        OMPI_SUCCESS != (err = set(MPI_WIN_SIZE, &attr_win_size)) ||
        OMPI_SUCCESS != (err = set(MPI_WIN_DISP_UNIT, &attr_win_disp_unit)) ||
#endif
#if 0
        /* JMS For when we implement IMPI */
        OMPI_SUCCESS != (err = set(MPI_IMPI_CLIENT_SIZE,
                                   &attr_impi_client_size)) ||
        OMPI_SUCCESS != (err = set(MPI_IMPI_CLIENT_COLOR, 
                                   &attr_impi_client_color)) ||
        OMPI_SUCCESS != (err = set(MPI_IMPI_HOST_SIZE,
                                   &attr_impi_host_size)) ||
        OMPI_SUCCESS != (err = set(MPI_IMPI_HOST_COLOR, 
                                   &attr_impi_host_color)) ||
#endif
        0) {
        return;
    }

    return;
}


static int set(int target_keyval, void *value)
{
    int err;
    int keyval;
    ompi_attribute_fn_ptr_union_t copy;
    ompi_attribute_fn_ptr_union_t del;

    keyval = -1;
    copy.attr_communicator_copy_fn = MPI_COMM_DUP_FN;
    del.attr_communicator_delete_fn = MPI_COMM_NULL_DELETE_FN;
    err = ompi_attr_create_keyval(COMM_ATTR, copy, del,
                                  &keyval, NULL, OMPI_KEYVAL_PREDEFINED);
    if (keyval != target_keyval || OMPI_SUCCESS != err) {
        return err;
    }
    err = ompi_attr_set(COMM_ATTR, MPI_COMM_WORLD,
                        &MPI_COMM_WORLD->c_keyhash, keyval, value, true, true);
    if (OMPI_SUCCESS != err) {
        return err;
    }

    return OMPI_SUCCESS;
}
