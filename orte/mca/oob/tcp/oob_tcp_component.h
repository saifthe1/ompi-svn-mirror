/*
 * Copyright (c) 2004-2007 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2006 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2006-2013 Los Alamos National Security, LLC. 
 *                         All rights reserved.
 * Copyright (c) 2010-2011 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2014      Intel, Inc. All rights reserved
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#ifndef _MCA_OOB_TCP_COMPONENT_H_
#define _MCA_OOB_TCP_COMPONENT_H_

#include "orte_config.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#include "opal/class/opal_bitmap.h"
#include "opal/class/opal_list.h"
#include "opal/class/opal_pointer_array.h"

#include "orte/mca/oob/oob.h"
#include "oob_tcp.h"

/**
 *  OOB TCP Component
 */
typedef struct {
    mca_oob_base_component_t super;          /**< base OOB component */
    uint32_t addr_count;                     /**< total number of addresses */
    int num_links;                           /**< number of logical links per physical device */
    opal_pointer_array_t modules;            /**< array of available modules */
    int                  num_modules;
    int                  max_retries;        /**< max number of retries before declaring peer gone */
    opal_list_t          events;             /**< events for monitoring connections */
    int                  peer_limit;         /**< max size of tcp peer cache */

    /* Port specifications */
    char*              if_include;           /**< list of ip interfaces to include */
    char*              if_exclude;           /**< list of ip interfaces to exclude */
    int                tcp_sndbuf;           /**< socket send buffer size */
    int                tcp_rcvbuf;           /**< socket recv buffer size */

    /* IPv4 support */
    bool               disable_ipv4_family;  /**< disable this AF */
    char**             tcp_static_ports;    /**< Static ports - IPV4 */
    char**             tcp_dyn_ports;       /**< Dynamic ports - IPV4 */
    char**             ipv4conns;
    char**             ipv4ports;

#if OPAL_ENABLE_IPV6
    /* IPv6 support */
    bool               disable_ipv6_family;  /**< disable this AF */
    char**             tcp6_static_ports;    /**< Static ports - IPV6 */
    char**             tcp6_dyn_ports;       /**< Dynamic ports - IPV6 */
    char**             ipv6conns;
    char**             ipv6ports;
#endif

    /* connection support */
    char*              my_uri;                 /**< uri for connecting to the TCP modules */
    int                num_hnp_ports;          /**< number of ports the HNP should listen on */
    opal_list_t        listeners;              /**< List of sockets being monitored by event or thread */
    opal_thread_t      listen_thread;          /**< handle to the listening thread */
    bool               listen_thread_active;
    struct timeval     listen_thread_tv;       /**< Timeout when using listen thread */
    int                stop_thread[2];         /**< pipe used to exit the listen thread */

    /* peers available via this transport - the index is the process name,
     * and the pointer returned is the pointer to the last module that
     * said it could reach that peer. When a module loses its connection
     * to the peer, this pointer either gets set to NULL (if nobody can
     * reach the peer) or to the next module that can. Must be accessed
     * only from the framework-level event base
     */
    opal_hash_table_t  peers;
} mca_oob_tcp_component_t;

ORTE_MODULE_DECLSPEC extern mca_oob_tcp_component_t mca_oob_tcp_component;

typedef struct {
    opal_object_t super;
    mca_oob_tcp_module_t *mod;  // current module handling this peer
    opal_bitmap_t reachable;    // marks the modules that can reach at least one of the peer's addresses
} mca_oob_tcp_component_peer_t;
OBJ_CLASS_DECLARATION(mca_oob_tcp_component_peer_t);

ORTE_MODULE_DECLSPEC void mca_oob_tcp_component_set_module(int fd, short args, void *cbdata);
ORTE_MODULE_DECLSPEC void mca_oob_tcp_component_lost_connection(int fd, short args, void *cbdata);
ORTE_MODULE_DECLSPEC void mca_oob_tcp_component_failed_to_connect(int fd, short args, void *cbdata);
ORTE_MODULE_DECLSPEC void mca_oob_tcp_component_no_route(int fd, short args, void *cbdata);
ORTE_MODULE_DECLSPEC void mca_oob_tcp_component_hop_unknown(int fd, short args, void *cbdata);

#endif /* _MCA_OOB_TCP_COMPONENT_H_ */
