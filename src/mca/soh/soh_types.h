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

#ifndef ORTE_SOH_TYPES_H
#define ORTE_SOH_TYPES_H

#include "orte_config.h"

/*
 * Process exit codes
 */

typedef int8_t orte_exit_code_t;

#define ORTE_PROC_EXIT_CODE_NORMAL      0x00
#define ORTE_PROC_EXIT_CODE_ABNORMAL    0x01

/*
 * Process state codes
 */

typedef int8_t orte_proc_state_t;

#define ORTE_PROC_STATE_RUNNING        0x00
#define ORTE_PROC_STATE_FINALIZING     0x01
#define ORTE_PROC_STATE_INIT           0x02
#define ORTE_PROC_STATE_TERMINATING    0x03
#define ORTE_PROC_STATE_STARTING       0x04
#define ORTE_PROC_STATE_LAUNCHING      0x05


/**
 * Node State, correspondinding to the ORTE_NODE_STATE_* #defines,
 * below.  These are #defines instead of an enum because the thought
 * is that we may have lots and lots of entries of these in the
 * registry and by making this an int8_t, it's only 1 byte, whereas an
 * enum defaults to an int (probably 4 bytes).  So it's a bit of a
 * space savings.
 */
typedef int8_t orte_node_state_t;

/** Node is in an unknown state (see orte_node_state_t) */
#define ORTE_NODE_STATE_UNKNOWN  0x00
/** Node is down (see orte_node_state_t) */
#define ORTE_NODE_STATE_DOWN     0x01
/** Node is up / available for use (see orte_node_state_t) */
#define ORTE_NODE_STATE_UP       0x02
/** Node is rebooting (only some systems will support this; see
    orte_node_state_t) */
#define ORTE_NODE_STATE_REBOOT   0x03


/* composite (union) type used to reduce number of function calls to SOH routines */
/* only used on routines that do not store data in this format (coms) */
typedef enum {orte_soh_entity_type_not_set, orte_soh_entity_type_cellid, orte_soh_entity_type_jobid, 
		  orte_soh_entity_type_proc, orte_soh_entity_type_node
} orte_soh_entity_type_t;

typedef union {
	orte_cellid_t         cellid;
	orte_jobid_t          jobid;
	orte_process_name_t   procname;
	orte_process_name_t   nodename;
} orte_soh_entity_value_t;


/* master SOH constants and types */

typedef int8_t orte_soh_master_update_policy_t;

/* these depend on soh master implementation so not all possible */
/* power2 so some can be combined */

/* update GPR on all changes */
#define ORTE_SOH_MASTER_UPDATE_IMMEDIATE  0x01
/* update GPR after time out (bundle/pack updates) */
#define ORTE_SOH_MASTER_UPDATE_PERIOD     0x02
/* update GPR on severity/level of change of status */
#define ORTE_SOH_MASTER_UPDATE_ON_LEVEL   0x04
/* update GPR on request/push only */
#define ORTE_SOH_MASTER_UPDATE_ON_REQUEST 0x08

/* key used by soh slaves to find master soh services in GPR */
#define ORTE_SOH_MASTER_LOC       "orte-soh-master-location"

#endif
