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
 * Populates global structure with process-specific information.
 *
 *
 */

#ifndef _OMPI_PROC_INFO_H_
#define _OMPI_PROC_INFO_H_

#include "orte_config.h"

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include "mca/ns/ns_types.h"

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif

/**
 * Process information structure
 *
 * The orte_proc_info() function fills the pid field and obtains the
 * process name, storing that information in the global structure. The
 * structure also holds path names to the universe, job, and process
 * session directories, and to the stdin, stdout, and stderr temp
 * files - however, these are all initialized elsewhere.
 */
struct orte_proc_info_t {
    bool init;             /**< Certifies that values have been filled.
			    * Certifies that the orte_sys_info() function has been
			    * called at least once so fields have valid values
			    */
    orte_process_name_t *my_name;  /**< My official process name */
    pid_t pid;             /**< Local process ID for this process */
    bool seed;             /**< Indicate whether or not this is seed daemon */
    bool daemon;           /**< Indicate whether or not I am a daemon */
    orte_process_name_t *ns_replica;       /**< Name of my name server replica (NULL=>me) */
    orte_process_name_t *gpr_replica;      /**< Name of my registry replica (NULL=>me) */
    char *my_universe;     /**< Name of the universe to which this process belongs */
    char *tmpdir_base;    /**< Base directory of the session dir tree */
    char *top_session_dir;   /**< Top-most directory of the session tree */
    char *universe_session_dir;  /**< Location of universe temp dir.
			    * The session directory has the form
			    * <prefix><openmpi-sessions-user><universe>, where the prefix
			    * can either be provided by the user via the
			    * --tmpdir command-line flag, the use of one of several
			    * environmental variables, or else a default location.
			    */

    char *job_session_dir;  /**< Session directory for job */

    char *proc_session_dir;    /**< Session directory for the process */

    char *sock_stdin;      /**< Path name to temp file for stdin. */
    char *sock_stdout;     /**< Path name to temp file for stdout. */
    char *sock_stderr;     /**< Path name to temp file for stderr. */
};
typedef struct orte_proc_info_t orte_proc_info_t;


/**
 *
 * Global process info descriptor.  Initialized to almost no
 * meaningful information - data is provided by calling \c
 * orte_rte_init() (which calls \c orte_proc_info() to fill in the
 * structure).
 *
 * The exception to this rule is the \c orte_process_info.seed field,
 * which will be initialized to \c false, but should be set to \c true
 * before calling \c orte_rte_info() if the caller is a seed daemon.
 */
OMPI_DECLSPEC extern orte_proc_info_t orte_process_info;


/**
 * \internal
 *
 * Global structure to store a wide range of information about the
 * process.  orte_proc_info populates a global variable with
 * information about the process being executing. This function should
 * be called only once, from orte_rte_init().
 *
 * @param None.
 *
 * @retval OMPI_SUCCESS Successfully initialized the various fields.
 * @retval OMPI_ERROR Failed to initialize one or more fields.
 */

OMPI_DECLSPEC int orte_proc_info(void);

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif

#endif
