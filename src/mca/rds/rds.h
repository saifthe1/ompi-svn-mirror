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
 * The Open RTE Resource Discovery Subsystem (RDS)
 * 
 * The resource discovery subsystem is responsible for discovering information about
 * computing resources available to the ORTE universe. The information provided is
 * stored on the registry for use by other subsystems, and is assumed to represent
 * some base state - in other words, the resource discovery information provides a
 * picture of what COULD be available, and not what necessarily IS available at a
 * specific point in time.
 * 
 * This can best be understood by considering two examples:
 * 
 * 1. A resource file (e.g., etc/lanl_resources.xml) describes one or more computing
 * resources in a given site. Information included in the file tells the ORTE system
 * how many compute nodes are available on a given cluster, the type of interconnect
 * used, the resource management software used, etc. This info subsequently is used
 * by the system to assist ORTE in determining how best to interact with the cluster
 * to provide the best performance for a user's application. Resource files are
 * intended to be provided at the site (as opposed to user) level, though users can
 * define their own resource files if desired (e.g., to describe resources they have
 * access to at some foreign site).
 * 
 * 2. A hostfile (e.g., etc/openmpi-default-hostfile) describes a set of hosts to
 * which a specific user has access and/or wishes to use to run an application. The
 * information in the hostfile is much less detailed than in a resource file - it
 * is primarily intended to allow a user to specify which hosts are to be used for
 * an application, and potentially how the application's processes can/are to be 
 * distributed across them. Thus, one would not expect to find info on memory/node
 * in a hostfile, but would expect to find info on user id for a given host and the
 * maximum number of processes the user wants put on a given host.
 * 
 * The location and name of the input files to be read (or whatever input information
 * source the component utilizes) will be provided via the MCA parameter mechanism.
 * Users will provide the information either via environmental parameters or as options
 * on the command line. Thus, the RDS components must be aware that another application
 * invocation may have already populated the registry with information on a given
 * resource. In addition, each component must be capable of accepting and processing
 * info from multiple input sources (e.g., a user may specify several resource files,
 * one from the site and one or more that the user has pesonally created to describe
 * foreign sites).
 * 
 * NOTE: It is the responsibility of each component to define unique MCA
 * parameters for passing their required input information.
 * 
 * The information developed by the RDS is placed on the ORTE_RESOURCE_SEGMENT of the
 * registry. To ensure proper operation, the information MUST be described by the
 * following tokens:
 * 
 * - site name: the name of the site wherein the resource resides (e.g., "lanl")
 * 
 * - resource name: the name of the specific resource being described (e.g., "pink")
 * 
 * Keywords to describe the individual data elements within the resource are mostly
 * defined in the rds_types.h
 * file. However, some higher level types (e.g., nodename) may be defined in the
 * include/orte_names.h
 * file as they need to be generally understandable across multiple subsystems.
 * 
 * The resource allocation subsystem (RAS) is the primary consumer of this information,
 * although other subsystems can also access it. The information is typically read at
 * the start of any application, but can also be accessed dynamically when an application
 * requests additional resources as part of a spawn process. The RAS does NOT alter the
 * information placed by the RDS on the ORTE_RESOURCE_SEGMENT, but augments it with
 * a dynamic count of the resources currently allocated/consumed from each resource
 * defined on the segment. Thus, the ORTE_RESOURCE_SEGMENT, contains both static and
 * dynamic information. This allows the allocator, for example, to possibly determine
 * that a request for additional resources should draw from another compute resource
 * as the requested one is already fully consumed.
 * 
 * Consumers of the information must always be prepared to find "holes" or "gaps" in
 * the information itself. The RDS can ONLY provide information it is given - it has
 * no dynamic discovery capabilities of its own at this time. Hence, if a resource
 * file doesn't provide any info on the launcher used by a particular cluster, then
 * other subsystems must be prepared to operate without it - or at least detect that
 * this information is not available and act accordintly.
 * 
 * Because the RDS operates as a multi-component subsystem (i.e., multiple components
 * are instantiated at any one time), its functions should NOT be accessed directly.
 * Instead, all calls to RDS functions should be made through the ORTE Resource
 * Manager (RMGR) subsystem to ensure proper operation.
 * 
 */

#ifndef ORTE_MCA_RDS_H
#define ORTE_MCA_RDS_H

/*
 * includes
 */

#include "orte_config.h"
#include "include/orte_constants.h"

#include "mca/mca.h"
#include "mca/ns/ns_types.h"

#include "rds_types.h"


/*
 * Component functions - all MUST be provided!
 */

/**
 * Query/update a resource
 * The query function requests that the component read its associated information
 * and store/update it on the ORTE_RESOURCE_SEGMENT of the registry. It must be
 * emphasized that this function has to support multiple calls in case the user
 * updates an input file or data source while the ORTE system is in operation - the
 * function should
 * not assume that the registry is currently empty. The registry design accommodates
 * this need by providing an "overwrite" capability that allows the user to permit
 * updating of existing info if it exists, while still adding the info to the registry
 * if prior info does not exist.
 * 
 * Requests to update the resource information will be manually provided. The RDS
 * is NOT responsible for detecting changes in input information and automatically
 * updating itself.
 *
 * @code
 * return_value = orte_rmgr.query();
 * @endcode
 */
typedef int (*orte_rds_base_module_query_fn_t)(void);

/**
 * Cleanup module resources.
 */

typedef int (*orte_rds_base_module_finalize_fn_t)(void);

/*
 * Ver 1.0.0
 */
struct orte_rds_base_module_1_0_0_t {
    orte_rds_base_module_query_fn_t query;
    orte_rds_base_module_finalize_fn_t finalize;
};

typedef struct orte_rds_base_module_1_0_0_t orte_rds_base_module_1_0_0_t;
typedef orte_rds_base_module_1_0_0_t orte_rds_base_module_t;

/*
 * RDS Component
 */

typedef orte_rds_base_module_t* (*orte_rds_base_component_init_fn_t)(
    bool *allow_multi_user_threads,
    bool *have_hidden_threads);

 
/*
 * the standard component data structure
 */

struct orte_rds_base_component_1_0_0_t {
    mca_base_component_t rds_version;
    mca_base_component_data_1_0_0_t rds_data;
    orte_rds_base_component_init_fn_t rds_init;
};
typedef struct orte_rds_base_component_1_0_0_t orte_rds_base_component_1_0_0_t;
typedef orte_rds_base_component_1_0_0_t orte_rds_base_component_t;


/*
 * Macro for use in components that are of type ns v1.0.0
 */
#define ORTE_RDS_BASE_VERSION_1_0_0 \
  /* ras v1.0 is chained to MCA v1.0 */ \
  MCA_BASE_VERSION_1_0_0, \
  /* ras v1.0 */ \
  "rds", 1, 0, 0

#endif

