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
 */

#include "orte_config.h"
#include "include/orte_constants.h"
#include "util/proc_info.h"
#include "mca/base/mca_base_param.h"
#include "mca/ns/ns.h"
#include "mca/errmgr/errmgr.h"
#include "mca/ns/base/base.h"
#include "mca/ns/base/ns_base_nds.h"


int orte_ns_nds_env_get(void)
{
    int rc;
    int id;
    int vpid_start;
    int num_procs;
    char* name_string = NULL;

    id = mca_base_param_register_string("ns", "nds", "name", NULL, NULL);
    mca_base_param_lookup_string(id, &name_string);
    if(name_string != NULL) {

        if (ORTE_SUCCESS != (rc = orte_ns_base_convert_string_to_process_name(
           &(orte_process_info.my_name),
           name_string))) {
           ORTE_ERROR_LOG(rc);
           free(name_string);
           return rc;
        }
        free(name_string);

    } else {

        int cellid;
        int jobid;
        int vpid;
      
        id = mca_base_param_register_int("ns", "nds", "cellid", NULL, -1);
        mca_base_param_lookup_int(id, &cellid);
        if (cellid < 0) return ORTE_ERR_NOT_FOUND;
            
        id = mca_base_param_register_int("ns", "nds", "jobid", NULL, -1);
        mca_base_param_lookup_int(id, &jobid);
        if (jobid < 0) return ORTE_ERR_NOT_FOUND;

        id = mca_base_param_register_int("ns", "nds", "vpid", NULL, -1);
        mca_base_param_lookup_int(id, &vpid);
        if (vpid < 0) return ORTE_ERR_NOT_FOUND;

        if (ORTE_SUCCESS != (rc = orte_ns.create_process_name(
           &(orte_process_info.my_name),
           cellid,
           jobid,
           vpid))) {
           ORTE_ERROR_LOG(rc);
           return rc;
        }
    }

    id = mca_base_param_register_int("ns", "nds", "vpid_start", NULL, -1);
    mca_base_param_lookup_int(id, &vpid_start);
    if (vpid_start < 0) return ORTE_ERR_NOT_FOUND;

    id = mca_base_param_register_int("ns", "nds", "num_procs", NULL, -1);
    mca_base_param_lookup_int(id, &num_procs);
    if (num_procs < 0) return ORTE_ERR_NOT_FOUND;
    
    orte_process_info.vpid_start = vpid_start;
    orte_process_info.num_procs = num_procs;
    return ORTE_SUCCESS;
}


int orte_ns_nds_env_put(const orte_process_name_t* name, orte_vpid_t vpid_start, size_t num_procs)
{
    return ORTE_ERR_NOT_IMPLEMENTED;
}


