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
/** @file:
 *
 * The Open MPI general purpose registry - implementation.
 *
 */

/*
 * includes
 */

#include "orte_config.h"

#include "dps/dps.h"
#include "mca/ns/ns.h"
#include "mca/errmgr/errmgr.h"

#include "gpr_replica_api.h"

int
orte_gpr_replica_synchro(orte_gpr_addr_mode_t addr_mode,
             orte_gpr_synchro_mode_t synchro_mode,
			orte_gpr_value_t *value, int trigger,
             orte_gpr_notify_id_t *local_idtag,
			orte_gpr_notify_cb_fn_t cb_func, void *user_tag)
{
    int rc;
    orte_gpr_replica_segment_t *seg;
    orte_gpr_replica_itag_t *token_itags, *key_itags;
    orte_gpr_notify_id_t idtag, remote_idtag;
    orte_gpr_replica_act_sync_t flag;
    
    *local_idtag = ORTE_GPR_NOTIFY_ID_MAX;
    flag.trig_synchro = synchro_mode;

    /* protect against errors */
    if (NULL == value || NULL == value->segment) {
        ORTE_ERROR_LOG(ORTE_ERR_BAD_PARAM);
	    return ORTE_ERR_BAD_PARAM;
    }
        
    OMPI_THREAD_LOCK(&orte_gpr_replica_globals.mutex);

    /* locate the segment */
    if (ORTE_SUCCESS != (rc = orte_gpr_replica_find_seg(&seg, true, value->segment))) {
        ORTE_ERROR_LOG(rc);
        OMPI_THREAD_UNLOCK(&orte_gpr_replica_globals.mutex);
        return rc;
    }

    if (orte_gpr_replica_globals.compound_cmd_mode) {
        	if (ORTE_SUCCESS != (rc = orte_gpr_base_pack_synchro(orte_gpr_replica_globals.compound_cmd,
        				  synchro_mode, addr_mode, value, trigger))) {
            ORTE_ERROR_LOG(rc);
            OMPI_THREAD_UNLOCK(&orte_gpr_replica_globals.mutex);
            return rc;
        }


        /* enter request on notify tracking system */
        if (ORTE_SUCCESS != (rc = orte_gpr_replica_enter_notify_request(&idtag, seg,
                                     ORTE_GPR_SYNCHRO_CMD, &flag,
        							   NULL, 0, cb_func, user_tag))) {
            ORTE_ERROR_LOG(rc);
            OMPI_THREAD_UNLOCK(&orte_gpr_replica_globals.mutex);
            return rc;
        }

        if (ORTE_SUCCESS != (rc = orte_dps.pack(orte_gpr_replica_globals.compound_cmd,
                                &idtag, 1, ORTE_GPR_NOTIFY_ID))) {
            ORTE_ERROR_LOG(rc);
            OMPI_THREAD_UNLOCK(&orte_gpr_replica_globals.mutex);
            return rc;
        }

        OMPI_THREAD_UNLOCK(&orte_gpr_replica_globals.mutex);
        *local_idtag = idtag;
        return ORTE_SUCCESS;
    }

    /* enter request on notify tracking system */
    if (ORTE_SUCCESS != (rc = orte_gpr_replica_enter_notify_request(&idtag,
                                    seg, ORTE_GPR_SYNCHRO_CMD, &flag,
                                    NULL, 0, cb_func, user_tag))) {
        ORTE_ERROR_LOG(rc);
        OMPI_THREAD_UNLOCK(&orte_gpr_replica_globals.mutex);
        return rc;
    }

    /* process synchro request */
    if (ORTE_SUCCESS != (rc = orte_gpr_replica_synchro_fn(addr_mode,
				    seg, value, trigger, idtag))) {
        ORTE_ERROR_LOG(rc);
        orte_gpr_replica_remove_notify_request(idtag, &remote_idtag);
    }

    if (ORTE_SUCCESS == rc) {
        *local_idtag = idtag;
        orte_gpr_replica_check_synchros(seg);
    }

    if (NULL != key_itags) {
      free(key_itags);
    }

    if (NULL != token_itags) {
      free(token_itags);
    }

    OMPI_THREAD_UNLOCK(&orte_gpr_replica_globals.mutex);

    if (ORTE_SUCCESS == rc) {
        orte_gpr_replica_process_callbacks();
    }
    
    return rc;
}

int orte_gpr_replica_cancel_synchro(orte_gpr_notify_id_t local_idtag)
{
    int rc;
    
    if (orte_gpr_replica_globals.compound_cmd_mode) {
	   return orte_gpr_base_pack_cancel_synchro(orte_gpr_replica_globals.compound_cmd,
						local_idtag);
    }

    OMPI_THREAD_LOCK(&orte_gpr_replica_globals.mutex);

    rc = orte_gpr_replica_cancel_synchro_fn(local_idtag);

    OMPI_THREAD_UNLOCK(&orte_gpr_replica_globals.mutex);

    return rc;
}
