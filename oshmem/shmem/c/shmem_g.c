/*
 * Copyright (c) 2013      Mellanox Technologies, Inc.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
#include "oshmem_config.h"

#include "oshmem/constants.h"
#include "oshmem/include/shmem.h"

#include "oshmem/runtime/runtime.h"

#include "oshmem/mca/spml/spml.h"

/*
 * These routines provide a low latency mechanism to retrieve basic types (short, int, float,
 * double, long) from symmetric data objects on remote PEs. 
 * Retrieves the value at the symmetric address addr of the remote PE pe.
 */
#define SHMEM_TYPE_G(type_name, type)    \
    type shmem##type_name##_g(type *addr, int pe) \
    {                                                               \
        int rc = OSHMEM_SUCCESS;                                    \
        size_t size = 0;                                            \
        type out_value;                                             \
                                                                    \
        RUNTIME_CHECK_INIT();                                       \
        RUNTIME_CHECK_PE(pe);                                       \
        RUNTIME_CHECK_ADDR(addr);                                   \
                                                                    \
        size = sizeof(out_value);                                   \
        rc = MCA_SPML_CALL(get(                                     \
            (void*)addr,                                            \
            size,                                                   \
            (void*)&out_value,                                      \
            pe));                                                   \
        RUNTIME_CHECK_RC(rc);                                       \
                                                                    \
        return out_value;                                           \
    }

#if OSHMEM_PROFILING
#include "oshmem/include/pshmem.h"
#pragma weak shmem_char_g = pshmem_char_g
#pragma weak shmem_short_g = pshmem_short_g
#pragma weak shmem_int_g = pshmem_int_g
#pragma weak shmem_long_g = pshmem_long_g
#pragma weak shmem_longlong_g = pshmem_longlong_g
#pragma weak shmem_float_g = pshmem_float_g
#pragma weak shmem_double_g = pshmem_double_g
#pragma weak shmem_longdouble_g = pshmem_longdouble_g
#include "oshmem/shmem/c/profile/defines.h"
#endif

SHMEM_TYPE_G(_char, char)
SHMEM_TYPE_G(_short, short)
SHMEM_TYPE_G(_int, int)
SHMEM_TYPE_G(_long, long)
SHMEM_TYPE_G(_longlong, long long)
SHMEM_TYPE_G(_float, float)
SHMEM_TYPE_G(_double, double)
SHMEM_TYPE_G(_longdouble, long double)
