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
/**
 * @file
 */
#ifndef MCA_PML_BASE_RECV_REQUEST_H
#define MCA_PML_BASE_RECV_REQUEST_H

#include "mca/pml/base/pml_base_request.h"
#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif
OMPI_DECLSPEC extern ompi_class_t mca_pml_base_recv_request_t_class;
/**
 * Base type for receive requests.
 */
struct mca_pml_base_recv_request_t {
   mca_pml_base_request_t req_base;  /**< base request */
   size_t req_bytes_packed;          /**< size of message being received */
   size_t req_bytes_received;        /**< number of bytes received from network */
   size_t req_bytes_delivered;       /**< number of bytes delivered to user */
};
typedef struct mca_pml_base_recv_request_t mca_pml_base_recv_request_t;


/**
 * Initialize a receive request with call parameters.
 *
 * @param request (IN)       Receive request.
 * @param addr (IN)          User buffer.
 * @param count (IN)         Number of elements of indicated datatype.
 * @param datatype (IN)      User defined datatype.
 * @param src (IN)           Source rank w/in the communicator.
 * @param tag (IN)           User defined tag.
 * @param comm (IN)          Communicator.
 * @param persistent (IN)    Is this a ersistent request.
 */
#define MCA_PML_BASE_RECV_REQUEST_INIT(                   \
    request,                                              \
    addr,                                                 \
    count,                                                \
    datatype,                                             \
    src,                                                  \
    tag,                                                  \
    comm,                                                 \
    persistent)                                           \
{                                                         \
    /* increment reference count on communicator */       \
    OBJ_RETAIN(comm);                                     \
                                                          \
    OMPI_REQUEST_INIT(&(request)->req_base.req_ompi);     \
    (request)->req_bytes_packed = 0;                      \
    (request)->req_base.req_sequence = 0;                 \
    (request)->req_base.req_addr = addr;                  \
    (request)->req_base.req_count = count;                \
    (request)->req_base.req_datatype = datatype;          \
    (request)->req_base.req_peer = src;                   \
    (request)->req_base.req_tag = tag;                    \
    (request)->req_base.req_comm = comm;                  \
    (request)->req_base.req_proc = NULL;                  \
    (request)->req_base.req_persistent = persistent;      \
    (request)->req_base.req_pml_complete = false;         \
    (request)->req_base.req_free_called = false;          \
                                                          \
}

#if defined(c_plusplus) || defined(__cplusplus)
}
#endif
#endif

