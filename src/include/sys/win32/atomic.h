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

#ifndef OMPI_SYS_ARCH_ATOMIC_H
#define OMPI_SYS_ARCH_ATOMIC_H 1

/*
 * On ia64, we use cmpxchg, which supports acquire/release semantics natively.
 */

static inline void ompi_atomic_mb(void)
{    
#if 0
   return KeMemoryBarrier();
#endif
}


static inline void ompi_atomic_rmb(void)
{
#if 0
   return KeMemoryBarrier();
#endif
}


static inline void ompi_atomic_wmb(void)
{    
#if 0
   return KeMemoryBarrier();
#endif
}

#define OMPI_ARCHITECTURE_DEFINE_ATOMIC_CMPSET_32
static inline int ompi_atomic_cmpset_acq_32( volatile int32_t *addr,
                                             int32_t oldval, int32_t newval)
{
#if 0
   int32_t ret = InterlockedCompareExchangeAcquire ((int32_t volatile*) addr,
                                                    (int32_t) newval, (int32_t) oldval);
   return (oldval == ret) ? 1: 0;
#else
   return 0;
#endif
}


static inline int ompi_atomic_cmpset_rel_32( volatile int32_t *addr,
                                             int32_t oldval, int32_t newval)
{
#if 0
   int32_t ret = InterlockedCompareExchangeRelease ((int32_t volatile*) addr,
                                                    (int32_t) newval, (int32_t) oldval);
   return (oldval == ret) ? 1: 0;
#else
   return 0;
#endif
}

static inline int ompi_atomic_cmpset_32( volatile int32_t *addr,
                                         int32_t oldval, int32_t newval)
{
#if 0
   int32_t ret = InterlockedCompareExchange ((int32_t volatile*) addr,
                                             (int32_t) newval, (int32_t) oldval);

   return (oldval == ret) ? 1: 0;
#else
   return 0;
#endif
}

#define OMPI_ARCHITECTURE_DEFINE_ATOMIC_CMPSET_64
static inline int ompi_atomic_cmpset_acq_64( volatile int64_t *addr,
                                             int64_t oldval, int64_t newval)
{
#if 0
   int64_t ret = InterlockedCompareExchangeAcquire64 ((int64_t volatile*) addr,
                                                      (int64_t) newval, (int64_t) oldval);
   return (oldval == ret) ? 1: 0;
#else
   return 0;
#endif
}

static inline int ompi_atomic_cmpset_rel_64( volatile int64_t *addr,
                                             int64_t oldval, int64_t newval)
{
#if 0
   int64_t ret = InterlockedCompareExchangeRelease64 ((int64_t volatile*) addr,
                                                      (int64_t) newval, (int64_t) oldval);
   return (oldval == ret) ? 1: 0;
#else
   return 0;
#endif
}


static inline int ompi_atomic_cmpset_64( volatile int64_t *addr,
                                         int64_t oldval, int64_t newval)
{
#if 0
   int64_t ret = InterlockedCompareExchange64 ((int64_t volatile*) addr,
                                                (int64_t) newval, (int64_t) oldval);
   return (oldval == ret) ? 1: 0;
#else
   return 0;
#endif
}

#define OMPI_ARCHITECTURE_DEFINE_ATOMIC_ADD_32
static inline int32_t ompi_atomic_add_32(volatile int32_t *addr, int32_t delta)
{
   return InterlockedExchangeAdd ((LONG volatile *) addr,
                                  (int32_t) delta);
   
}

#define OMPI_ARCHITECTURE_DEFINE_ATOMIC_ADD_64
static inline int64_t ompi_atomic_add_64(volatile int64_t *addr, int64_t delta)
{
#if 0
   return InterlockedExchangeAdd64 ((int64_t volatile *) addr,
                                    (int64_t) delta);
#else
   return 0;
#endif
   
}

#define OMPI_ARCHITECTURE_DEFINE_ATOMIC_SUB_32
static inline int32_t ompi_atomic_sub_32(volatile int32_t *addr, int32_t delta)
{
   return InterlockedExchangeAdd( (LONG volatile *) addr,
                                  (int32_t) (-delta));

}

#define OMPI_ARCHITECTURE_DEFINE_ATOMIC_SUB_64
static inline int64_t ompi_atomic_sub_64(volatile int64_t *addr, int64_t delta)
{
#if 0
   return InterlockedExchangeAdd64 ((int64_t volatile *) addr,
                                    (int64_t) (-delta));
#else
   return 0;
#endif

}
#endif /* ! OMPI_SYS_ARCH_ATOMIC_H */
