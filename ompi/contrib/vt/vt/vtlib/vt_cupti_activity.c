/**
 * VampirTrace
 * http://www.tu-dresden.de/zih/vampirtrace
 *
 * Copyright (c) 2005-2013, ZIH, TU Dresden, Federal Republic of Germany
 *
 * Copyright (c) 1998-2005, Forschungszentrum Juelich, Juelich Supercomputing
 *                          Centre, Federal Republic of Germany
 *
 * See the file COPYING in the package base directory for details
 **/

#include "vt_env.h"             /* get environment variables */
#include "vt_pform.h"           /* VampirTrace time measurement */
#include "vt_gpu.h"             /* common for GPU */
#include "vt_mallocwrap.h"      /* wrapping of malloc and free */
#include "vt_cupti.h"           /* CUPTI header */
#include "vt_cupti_common.h"    /* CUPTI common structures, functions, etc. */
#include "vt_cupti_activity.h"

#include "stdio.h"

/* reduce buffer size for alignment, if necessary */
#define ALIGN_BUFFER(buffer, align) \
  (((uintptr_t) (buffer) & ((align)-1)) ? \
        ((buffer) - ((uintptr_t) (buffer) & ((align)-1))) : (buffer)) 

/*
 * Register the finalize function before the CUDA and CUPTI library clean up 
 * their data.
 
#define VT_CUPTI_ACT_REGISTER_FINALIZE                         \
  if(!vt_cuptiact_finalize_registered){                        \
    VT_CUPTI_LOCK(); \
    if(!vt_cuptiact_finalize_registered){                      \
      atexit(vt_cupti_activity_finalize);                      \
      vt_cntl_msg(2, "[CUPTI Activity] Finalize registered!"); \
      vt_cuptiact_finalize_registered = 1;                     \
    }                                                          \
    VT_CUPTI_UNLOCK(); \
  }
*/

/* initialization and finalization flags */
static uint8_t vt_cuptiact_initialized = 0;
static uint8_t vt_cuptiact_finalized = 0;
/*static uint8_t vt_cuptiact_finalize_registered = 0;*/

/* VampirTrace global CUPTI activity buffer 
static uint8_t *vt_cuptiact_global_buffer = NULL;*/

/* size of the activity buffer */
static size_t vt_cuptiact_bufSize = VT_CUPTI_ACT_DEFAULT_BSIZE;

/* cupti activity specific kernel counter IDs */
static uint32_t vt_cuptiact_cid_knStaticSharedMem = VT_NO_ID;
static uint32_t vt_cuptiact_cid_knDynamicSharedMem = VT_NO_ID;
static uint32_t vt_cuptiact_cid_knLocalMemTotal = VT_NO_ID;
static uint32_t vt_cuptiact_cid_knRegistersPerThread = VT_NO_ID;

/* global region IDs for wrapper internal tracing */
static uint32_t vt_cuptiact_rid_flush = VT_NO_ID;

/*********************** function declarations ***************************/
static vt_cupti_activity_t* vt_cuptiact_createCtxActivity(CUcontext cuCtx);

static void vt_cuptiact_destroyContext(vt_cupti_activity_t *vtcuptiActCtx);

static void vt_cuptiact_writeRecord(CUpti_Activity *record, 
                                    vt_cupti_ctx_t *vtCtx);

static void vt_cuptiact_writeMemcpyRecord(CUpti_ActivityMemcpy *mcpy, 
                                          vt_cupti_ctx_t *vtCtx);

static void vt_cuptiact_writeKernelRecord(CUpti_ActivityKernel *kernel, 
                                          vt_cupti_ctx_t *vtCtx);

/******************************************************************************/

/* no need to lock, because it is only called by vt_cupti_callback_init() */
void vt_cupti_activity_init()
{
  /*if(!vt_cuptiact_initialized){
    vt_cupti_init();
    VT_CUPTI_LOCK();*/
    if(!vt_cuptiact_initialized){
      vt_cntl_msg(2, "[CUPTI Activity] Initializing ... ");
      
      {        
        vt_cuptiact_bufSize = vt_env_cudatrace_bsize();
        
        /* no buffer size < 1024 bytes allowed (see CUPTI documentation) */
        if(vt_cuptiact_bufSize < 1024){
          if(vt_cuptiact_bufSize > 0){
            vt_warning("[CUPTI Activity] Buffer size has to be at least 1024 "
                       "bytes! It has been set to %d.", vt_cuptiact_bufSize);
          }
          vt_cuptiact_bufSize = VT_CUPTI_ACT_DEFAULT_BSIZE;
        }
        
        /* queue a global buffer to initialize CUPTI before CUDA init 
        vt_cuptiact_buffer = (uint8_t *)malloc(vt_cuptiact_bufSize);
        VT_CUPTI_CALL(cuptiActivityEnqueueBuffer(NULL, 0, 
                                      vt_cuptiact_buffer, vt_cuptiact_bufSize), 
                      "cuptiActivityEnqueueBuffer");*/
      }
      
#if (defined(VT_MT) || defined(VT_HYB))
      VTTHRD_LOCK_IDS();
#endif
      if(vt_gpu_trace_kernels > 1){
        /* define kernel counters */
        vt_cuptiact_cid_knStaticSharedMem = vt_def_counter(VT_MASTER_THREAD, 
                      "staticSharedMemory", "Bytes",
                      VT_CNTR_ABS | VT_CNTR_NEXT | VT_CNTR_UNSIGNED, 
                      vt_cupti_cgid_cuda_kernel, 0);
        vt_cuptiact_cid_knDynamicSharedMem = vt_def_counter(VT_MASTER_THREAD, 
                      "dynamicSharedMemory", "Bytes",
                      VT_CNTR_ABS | VT_CNTR_NEXT | VT_CNTR_UNSIGNED, 
                      vt_cupti_cgid_cuda_kernel, 0);
        vt_cuptiact_cid_knLocalMemTotal = vt_def_counter(VT_MASTER_THREAD, 
                      "localMemoryPerKernel", "Bytes",
                      VT_CNTR_ABS | VT_CNTR_NEXT | VT_CNTR_UNSIGNED, 
                      vt_cupti_cgid_cuda_kernel, 0);
        vt_cuptiact_cid_knRegistersPerThread = vt_def_counter(VT_MASTER_THREAD, 
                      "registersPerThread", "#",
                      VT_CNTR_ABS | VT_CNTR_NEXT | VT_CNTR_UNSIGNED, 
                      vt_cupti_cgid_cuda_kernel, 0);
      }
     
      /* define region for GPU activity flush */
      vt_cuptiact_rid_flush = vt_def_region(VT_MASTER_THREAD, "flushActivities", 
                        VT_NO_ID, VT_NO_LNO, VT_NO_LNO, "VT_CUDA", VT_FUNCTION);
#if (defined(VT_MT) || defined(VT_HYB))
      VTTHRD_UNLOCK_IDS();
#endif
      
      /*** enable the activities ***/
      /* enable kernel tracing */
      if(vt_gpu_trace_kernels > 0){
#if (defined(CUPTI_API_VERSION) && (CUPTI_API_VERSION >= 3))
        if((vt_gpu_config & VT_GPU_TRACE_CONCURRENT_KERNEL) 
           == VT_GPU_TRACE_CONCURRENT_KERNEL){
          /*VT_CUPTI_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_KERNEL), 
                        "cuptiActivityEnable");*/
          VT_CUPTI_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL), 
                        "cuptiActivityEnable");
        }else
#endif
          VT_CUPTI_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_KERNEL), 
                        "cuptiActivityEnable");
      }
      
      /* enable memory copy tracing */
      if(vt_gpu_trace_mcpy){
        VT_CUPTI_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_MEMCPY), 
                      "cuptiActivityEnable");
      }
      
      /* register the finalize function of VampirTrace CUPTI to be called before
       * the program exits 
      atexit(vt_cupti_activity_finalize);*/

      vt_cuptiact_initialized = 1;
      /*VT_CUPTI_UNLOCK();
    }*/
  }
}

void vt_cupti_activity_finalize()
{
  if(!vt_cuptiact_finalized && vt_cuptiact_initialized){
    VT_CUPTI_LOCK();
    if(!vt_cuptiact_finalized && vt_cuptiact_initialized){
      vt_cupti_ctx_t *vtCtx = vt_cupti_ctxList;
      
      vt_cntl_msg(2, "[CUPTI Activity] Finalizing ... ");

      while(vtCtx != NULL){
        
        /* write buffered activities, which have not been dumped yet */
        vt_cuptiact_flushCtxActivities(vtCtx);
        
        /* free the context */
        vt_cuptiact_destroyContext(vtCtx->activity);
        vtCtx->activity = NULL;
        
        /* set pointer to next context */
        vtCtx = vtCtx->next;
      }
      
      vt_cuptiact_finalized = 1;
      VT_CUPTI_UNLOCK();
    }
  }
}

/*
 * Allocate a new buffer and add it to the queue specified by a CUDA context.
 * 
 * @param cuCtx the CUDA context, specifying the queue
 * 
 * @return pointer to the created buffer
 */
static uint8_t* vt_cuptiact_queueNewBuffer(CUcontext cuCtx)
{
  uint8_t *buffer = (uint8_t *)malloc(vt_cuptiact_bufSize);
	
  VT_CUPTI_CALL(cuptiActivityEnqueueBuffer(cuCtx, 0, ALIGN_BUFFER(buffer, 8), 
                                           vt_cuptiact_bufSize), 
                "cuptiActivityEnqueueBuffer");
  
  return buffer;
}

void vt_cuptiact_setupActivityContext(vt_cupti_ctx_t *vtCtx)
{  
  /* try to get the global VampirTrace CUPTI context */
  if(vtCtx == NULL){
    vt_warning("[CUPTI Activity] No context given!");
    return;
  }
  
  VT_SUSPEND_MALLOC_TRACING(vtCtx->ptid);
  
  /* create the VampirTrace CUPTI activity context */
  if(vtCtx->activity == NULL)
    vtCtx->activity = vt_cuptiact_createCtxActivity(vtCtx->cuCtx);
  
  /* queue new buffer to context to record activities */
  vtCtx->activity->buffer = vt_cuptiact_queueNewBuffer(vtCtx->cuCtx);
  
  VT_RESUME_MALLOC_TRACING(vtCtx->ptid);
}
  
/*
 * Create a VampirTrace CUPTI Activity stream.
 * 
 * @param devID ID of the CUDA device
 * @param strmID ID of the CUDA stream
 * 
 * @return pointer to created VampirTrace CUPTI Activity stream
 */
static vt_cuptiact_strm_t* vt_cuptiact_createStream(vt_cupti_ctx_t *vtCtx, 
                                                    uint32_t strmID)
{
  vt_cuptiact_strm_t *vtStrm = NULL;
  
  vtStrm = (vt_cuptiact_strm_t *)malloc(sizeof(vt_cuptiact_strm_t));
  if(vtStrm == NULL)
    vt_error_msg("[CUPTI Activity] Could not allocate memory for stream!");
  vtStrm->strmID = strmID;
  vtStrm->vtLastTime = vt_gpu_init_time;
  vtStrm->destroyed = 0;
  vtStrm->next = NULL;
  
  /* create VT-User-Thread with name and parent id and get its id */
  {
    char thread_name[16] = "CUDA";

    if(vt_gpu_stream_reuse){
      if(vtCtx->devID != VT_NO_ID){
        if(-1 == snprintf(thread_name+4, 12, "[%d]", vtCtx->devID))
          vt_cntl_msg(1, "Could not create thread name for CUDA thread!");
      }
    }else{
      if(vtCtx->devID == VT_NO_ID){
        if(-1 == snprintf(thread_name+4, 12, "[?:%d]", strmID))
          vt_cntl_msg(1, "Could not create thread name for CUDA thread!");
      }else{
        if(-1 == snprintf(thread_name+4, 12, "[%d:%d]", vtCtx->devID, strmID))
          vt_cntl_msg(1, "Could not create thread name for CUDA thread!");
      }
    }
    
    VT_CHECK_THREAD;
    vt_gpu_registerThread(thread_name, VT_MY_THREAD, &(vtStrm->vtThrdID));
  }
  
  /* if first stream created for this device, make it the default stream */
  if(vtCtx->activity->strmList == NULL){
    /* write enter event for GPU_IDLE on first stream */
    if(vt_gpu_trace_idle == 1){
      if(vt_gpu_init_time < vt_start_time)
        vt_gpu_init_time = vt_start_time;
          
      vt_enter(vtStrm->vtThrdID, &vt_gpu_init_time, vt_gpu_rid_idle);
      /*vt_warning("IDLEente: %llu (%d)", vt_gpu_init_time, vtStrm->vtThrdID);*/
      vtCtx->activity->gpuIdleOn = 1;
    }
  }
  
  return vtStrm;
}

/*
 * Create a VampirTrace CUPTI activity context.
 * 
 * @return pointer to created VampirTrace CUPTI Activity context
 */
static vt_cupti_activity_t* vt_cuptiact_createCtxActivity(CUcontext cuCtx)
{
  vt_cupti_activity_t* vtCtxAct = NULL;
  
  /* create new context, as it is not listed */
  vtCtxAct = (vt_cupti_activity_t *)malloc(sizeof(vt_cupti_activity_t));
  if(vtCtxAct == NULL) 
    vt_error_msg("[CUPTI Activity] Could not allocate memory for activity context!");
  vtCtxAct->strmList = NULL;
  vtCtxAct->gpuMemAllocated = 0;
  vtCtxAct->gpuMemList = NULL;
  vtCtxAct->buffer = NULL;
  vtCtxAct->vtLastGPUTime = vt_gpu_init_time;
  vtCtxAct->gpuIdleOn = 1;
  
  /* 
   * Get time synchronization factor between host and GPU time for measurement 
   * interval 
   */
  {
    VT_CUPTI_CALL(cuptiGetTimestamp(&(vtCtxAct->sync.gpuStart)), "cuptiGetTimestamp");
    vtCtxAct->sync.hostStart = vt_pform_wtime();
  }
  
    /* set default CUPTI stream ID (needed for memory usage and idle tracing) */
  VT_CUPTI_CALL(cuptiGetStreamId(cuCtx, NULL, &(vtCtxAct->defaultStrmID)), 
                                 "cuptiGetStreamId");
  
  return vtCtxAct;
}

/*
 * Destroy a VampirTrace CUPTI Activity context.
 * 
 * @param vtcuptiActCtx VampirTrace CUPTI Activity context
 */
static void vt_cuptiact_destroyContext(vt_cupti_activity_t *vtcuptiActCtx)
{
  if(vtcuptiActCtx == NULL) 
    return;
  
  /* write exit event for GPU idle time */
  if(vt_gpu_trace_idle == 1 && vtcuptiActCtx->gpuIdleOn == 1){
    uint64_t idle_end = vt_pform_wtime();
    /*vt_warning("IDLEexit: %llu (%d)", idle_end, vtCtx->strmList->vtThrdID);*/
    vt_exit(vtcuptiActCtx->strmList->vtThrdID, &idle_end);
    /*vtCtx->gpuIdleOn = 0;*/
  }
  
  /* cleanup stream list */
  while(vtcuptiActCtx->strmList != NULL){
    vt_cuptiact_strm_t *vtStrm = vtcuptiActCtx->strmList;
    
    vtcuptiActCtx->strmList = vtcuptiActCtx->strmList->next;
    
    free(vtStrm);
    vtStrm = NULL;
  }
  
  /* free CUDA malloc entries, if user application has memory leaks */
  while(vtcuptiActCtx->gpuMemList != NULL){
    vt_cupti_gpumem_t *vtMem =  vtcuptiActCtx->gpuMemList;
    
    if(vt_gpu_trace_memusage > 1)
      vt_cntl_msg(1, "[CUPTI Activity] Free of %d bytes GPU memory missing!", 
                     vtMem->size);
    
    vtcuptiActCtx->gpuMemList = vtMem->next;
    free(vtMem);
    vtMem = NULL;
  }
  
  /* free activity buffer */
  if(vtcuptiActCtx->buffer != NULL){
    free(vtcuptiActCtx->buffer);
    vtcuptiActCtx->buffer = NULL;
  }
  
  free(vtcuptiActCtx);
}

/*
 * Check for a VampirTrace activity stream by stream ID. If it does not exist,
 * create it.
 * 
 * @param vtCtx VampirTrace CUPTI Activity context
 * @param strmID the CUDA stream ID provided by CUPTI callback API
 * 
 * @return the VampirTrace CUDA stream
 */
static vt_cuptiact_strm_t* vt_cuptiact_checkStream(vt_cupti_ctx_t* vtCtx, 
                                                   uint32_t strmID)
{
  vt_cupti_activity_t *vtcuptiActivity = vtCtx->activity;
  vt_cuptiact_strm_t *currStrm = NULL;
  vt_cuptiact_strm_t *lastStrm = NULL;
  vt_cuptiact_strm_t *reusableStrm = NULL;
  
  if(vtCtx == NULL){
    vt_warning("[CUPTI Activity] No context given in vt_cuptiact_checkStream()!");
    return NULL;
  }
  
  /* lookup stream */
  /*VT_CUPTI_LOCK();*/
  currStrm = vtcuptiActivity->strmList;
  lastStrm = vtcuptiActivity->strmList;
  while(currStrm != NULL){
    /* check for existing stream */
    if(currStrm->strmID == strmID){
      /*VT_CUPTI_UNLOCK();*/
      return currStrm;
    }
    
    /* check for reusable stream */
    if(vt_gpu_stream_reuse && reusableStrm == NULL && currStrm->destroyed == 1){
      reusableStrm = currStrm;
    }
    
    lastStrm = currStrm;
    currStrm = currStrm->next;
  }
  
  /* reuse a destroyed stream, if there is any available */
  if(vt_gpu_stream_reuse && reusableStrm){
    vt_cntl_msg(2, "[CUPTI Activity] Reusing CUDA stream %d with stream %d",
                   reusableStrm->strmID, strmID);
    reusableStrm->destroyed = 0;
    reusableStrm->strmID = strmID;

    return reusableStrm;
  }
  
  /* 
   * If stream list is empty, the stream to be created is not the default
   * stream and GPU idle and memory copy tracing is enabled, then create
   * a default stream.
   */
  if(vtcuptiActivity->strmList == NULL && strmID != vtcuptiActivity->defaultStrmID && 
     vt_gpu_trace_idle == 1 && vt_gpu_trace_mcpy){
    vtcuptiActivity->strmList = 
            vt_cuptiact_createStream(vtCtx, vtcuptiActivity->defaultStrmID);
    lastStrm = vtcuptiActivity->strmList;
  }
  
  /* create the stream, which has not been created yet */
  currStrm = vt_cuptiact_createStream(vtCtx, strmID);
  
  /* append the newly created stream */
  if(NULL != lastStrm) lastStrm->next = currStrm;
  else vtcuptiActivity->strmList = currStrm;
  
  /*VT_CUPTI_UNLOCK();*/
  return currStrm;
}

void vt_cuptiact_markStreamAsDestroyed(CUcontext cuCtx, uint32_t strmID)
{
  vt_cupti_ctx_t *vtCtx = NULL;
  vt_cuptiact_strm_t *currStrm = NULL;

  VT_CUPTI_LOCK();
  
  if(cuCtx == NULL){
    vt_warning("[CUPTI Activity] No CUDA context given in "
               "vt_cuptiact_markStreamAsDestroyed()!");
    VT_CUPTI_UNLOCK();
    return;
  }
  
  vtCtx = vt_cupti_getCtxNoLock(cuCtx);
  
  if(vtCtx == NULL){
    vt_warning("[CUPTI Activity] No context found in "
               "vt_cuptiact_markStreamAsDestroyed()!");
    VT_CUPTI_UNLOCK();
    return;
  }
  
  currStrm = vtCtx->activity->strmList;
  while(currStrm != NULL){
    if(currStrm->strmID == strmID){
      currStrm->destroyed = 1;
      VT_CUPTI_UNLOCK();
      return;
    }
    currStrm = currStrm->next;
  }
  
  VT_CUPTI_UNLOCK();
}

void vt_cuptiact_flushCtxActivities(vt_cupti_ctx_t *vtCtx)
{ 
  CUptiResult status;
  uint8_t *buffer = NULL;
  size_t bufSize;
  CUpti_Activity *record = NULL;
  uint64_t hostStop, gpuStop;
  uint32_t ptid = VT_NO_ID;
  vt_cupti_activity_t *vtcuptiActivity = NULL;
  
  /* check for VampirTrace CUPTI context */
  if(vtCtx == NULL || vtCtx->activity == NULL){
    vt_warning("[CUPTI Activity] Context not found!");
    return;
  }
  vtcuptiActivity = vtCtx->activity;
  
  /* check if the buffer contains records */
  status = cuptiActivityQueryBuffer(vtCtx->cuCtx, 0, &bufSize);
  if(status != CUPTI_SUCCESS){
    if(CUPTI_ERROR_QUEUE_EMPTY == status || 
       CUPTI_ERROR_MAX_LIMIT_REACHED != status){
      return;
    }
  }

  /* expose VampirTrace CUPTI activity flush as measurement overhead */
  VT_CHECK_THREAD;
  ptid = VT_MY_THREAD;
  hostStop = vt_pform_wtime();
  vt_enter(ptid, &hostStop, vt_cuptiact_rid_flush);
  
  vt_cntl_msg(2,"[CUPTI Activity] Handle context %d activities", vtCtx->cuCtx);
  
  /* lock the whole buffer flush 
  VT_CUPTI_LOCK();*/
  
  /* dump the contents of the global queue */
  VT_CUPTI_CALL(cuptiActivityDequeueBuffer(vtCtx->cuCtx, 0, &buffer, 
                &bufSize), "cuptiActivityDequeueBuffer");

  /* 
   * Get time synchronization factor between host and GPU time for measured 
   * period 
   */
  {
    VT_CUPTI_CALL(cuptiGetTimestamp(&gpuStop), "cuptiGetTimestamp");
    hostStop = vt_pform_wtime();
    vtcuptiActivity->sync.hostStop = hostStop;
    
    vtcuptiActivity->sync.factor = (double)(hostStop - vtcuptiActivity->sync.hostStart)
                       /(double)(gpuStop - vtcuptiActivity->sync.gpuStart);
  }

  /*vt_cntl_msg(1, "hostStop: %llu , gpuStop: %llu", hostStopTS, gpuStopTS);
  vt_cntl_msg(1, "factor: %lf", syncFactor);*/
  
  do{
    status = cuptiActivityGetNextRecord(buffer, bufSize, &record);
    if(status == CUPTI_SUCCESS) {
      vt_cuptiact_writeRecord(record, vtCtx);
    }else if(status == CUPTI_ERROR_MAX_LIMIT_REACHED){
      break;
    }else{
      VT_CUPTI_CALL(status, "cuptiActivityGetNextRecord");
    }
  }while(1);

  /* report any records dropped from the global queue */
  {
    size_t dropped;
    
    VT_CUPTI_CALL(cuptiActivityGetNumDroppedRecords(vtCtx->cuCtx, 0, &dropped), 
                  "cuptiActivityGetNumDroppedRecords");
    if(dropped != 0)
      vt_warning("[CUPTI Activity] Dropped %u records. Current buffer size: %llu bytes\n"
                 "To avoid dropping of records increase the buffer size!\n"
                 "Proposed minimum VT_CUDATRACE_BUFFER_SIZE=%llu", 
                 (unsigned int)dropped, vt_cuptiact_bufSize, 
                 vt_cuptiact_bufSize + dropped/2 * 
                 (sizeof(CUpti_ActivityKernel) + sizeof(CUpti_ActivityMemcpy)));
  }
  
  /* enter GPU idle region after last kernel, if exited before */
  if(vtcuptiActivity->gpuIdleOn == 0){
    vt_enter(vtcuptiActivity->strmList->vtThrdID, 
             &(vtcuptiActivity->vtLastGPUTime), vt_gpu_rid_idle);
    vtcuptiActivity->gpuIdleOn = 1;
    /*vt_warning("IDLfente: %llu (%d)", vtCtx->vtLastGPUTime, vtCtx->strmList->vtThrdID);*/
  }
  
  /* enqueue buffer again */
  VT_CUPTI_CALL(cuptiActivityEnqueueBuffer(vtCtx->cuCtx, 0, buffer, 
                vt_cuptiact_bufSize), "cuptiActivityEnqueueBuffer");
  
    
  /* set new synchronization point */
  vtcuptiActivity->sync.hostStart = hostStop;
  vtcuptiActivity->sync.gpuStart = gpuStop;
  
  /*VT_CUPTI_UNLOCK();*/
  
  /* use local variable hostStop to write exit event for activity flush */
  hostStop = vt_pform_wtime();
  vt_exit(ptid, &hostStop);
}

/*
 * Select record type and call respective function.
 * 
 * @param record the basic CUPTI activity record
 * @param vtCtx the VampirTrace CUPTI activity context
 */
static void vt_cuptiact_writeRecord(CUpti_Activity *record, 
                                    vt_cupti_ctx_t *vtCtx)
{
  switch (record->kind) {
    case CUPTI_ACTIVITY_KIND_KERNEL: {
      vt_cuptiact_writeKernelRecord((CUpti_ActivityKernel *)record, vtCtx);
      break;
    }

#if (defined(CUPTI_API_VERSION) && (CUPTI_API_VERSION >= 3))
    case CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL: {
      vt_cuptiact_writeKernelRecord((CUpti_ActivityKernel *)record, vtCtx);
      break;
    }
#endif
    
    case CUPTI_ACTIVITY_KIND_MEMCPY: {
      vt_cuptiact_writeMemcpyRecord((CUpti_ActivityMemcpy *)record, vtCtx);
      break;
    }
    default: {
      break;
    }
  }
}

/*
 * Use the CUPTI activity kernel record to write the corresponding VampirTrace
 * events.
 * 
 * @param kernel the CUPTI activity kernel record
 * @param vtCtx the VampirTrace CUPTI activity context
 */
static void vt_cuptiact_writeKernelRecord(CUpti_ActivityKernel *kernel, 
                                          vt_cupti_ctx_t *vtCtx)
{
  vt_cupti_activity_t *vtcuptiActivity = vtCtx->activity;
  vt_cuptiact_strm_t *vtStrm = NULL;
  uint32_t vtThrdID = VT_NO_ID;
  uint32_t knRID = VT_NO_ID;
  vt_gpu_hn_string_t *hn = NULL;
  
  VT_SUSPEND_MALLOC_TRACING(vtCtx->ptid);
  
  /* get VampirTrace thread ID for the kernel's stream */  
  vtStrm = vt_cuptiact_checkStream(vtCtx, kernel->streamId);
  vtThrdID = vtStrm->vtThrdID;
  
  VT_RESUME_MALLOC_TRACING(vtCtx->ptid);
  
  /* get the VampirTrace region ID for the kernel */
  hn = vt_gpu_stringHashGet(kernel->name);
  if(hn){
    knRID = hn->rid;
  }else{
    char *knName = vt_cuda_demangleKernel(kernel->name);
    
    if(knName == NULL || *knName == '\0') {
      knName = (char *)kernel->name;
      
      if(knName == NULL) knName = "unknownKernel";
    }
    
    knRID = vt_def_region(VT_MASTER_THREAD, knName, VT_NO_ID,
                          VT_NO_LNO, VT_NO_LNO, "CUDA_KERNEL", VT_FUNCTION);

    hn = vt_gpu_stringHashPut(kernel->name, knRID);
  }

  /* write events */
  {
    uint64_t start = vtcuptiActivity->sync.hostStart 
                   + (kernel->start - vtcuptiActivity->sync.gpuStart) * vtcuptiActivity->sync.factor;
    uint64_t stop = start + (kernel->end - kernel->start) * vtcuptiActivity->sync.factor;
    
    /* if current activity's start time is before last written timestamp */
    if(start < vtStrm->vtLastTime){
      vt_warning("[CUPTI Activity] Kernel: start time < last written timestamp!");
      vt_warning("[CUPTI Activity] Kernel: '%s', CUdevice: %d, "
                 "CUDA stream ID: %d, Thread ID: %d", 
                 hn->sname, vtCtx->cuDev, vtStrm->strmID, vtStrm->vtThrdID);
      
      if(vtStrm->vtLastTime < stop){
        vt_warning("[CUPTI Activity] Set kernel start time to sync-point time"
                   "(cut %.4lf%%)", 
                   (double)(vtStrm->vtLastTime - start)/(double)(stop-start));
        start = vtStrm->vtLastTime;
      }else{
        vt_warning("[CUPTI Activity] Skipping ...");
        return;
      }
    }
    
    /* check if time between start and stop is increasing */
    if(stop < start){
      vt_warning("[CUPTI Activity] Kernel: start time > stop time!");
      vt_warning("[CUPTI Activity] Skipping '%s' on CUDA device:stream [%d:%d],"
                 " Thread ID %d", 
                 hn->sname, vtCtx->cuDev, vtStrm->strmID, vtStrm->vtThrdID);
      return;
    }
    
    /* check if synchronization stop time is before kernel stop time */
    if(vtcuptiActivity->sync.hostStop < stop){
      vt_warning("[CUPTI Activity] Kernel: sync-point time < kernel stop time");
      vt_warning("[CUPTI Activity] Kernel: '%s', CUdevice: %d, "
                 "CUDA stream ID: %d, Thread ID: %d", 
                 hn->sname, vtCtx->cuDev, vtStrm->strmID, vtStrm->vtThrdID);
      
      /* Write kernel with sync.hostStop stop time stamp, if possible */
      if(vtcuptiActivity->sync.hostStop > start){
        vt_warning("[CUPTI Activity] Set kernel-stop-time to sync-point-time "
                   "(cut %.4lf%%)", 
                   (double)(stop - vtcuptiActivity->sync.hostStop)/(double)(stop-start));
        
        stop = vtcuptiActivity->sync.hostStop;
      }else{
        vt_warning("[CUPTI Activity] Skipping ...");
        return;
      }
    }
    
    /* set the last VampirTrace timestamp, written in this stream */
    vtStrm->vtLastTime = stop;

    /*vt_cntl_msg(1, "'%s'(%d) start: %llu; stop: %llu (tid: %d)", 
                   kernel->name, knRID, start, stop, vtThrdID);*/
    
    /* GPU idle time will be written to first CUDA stream in list */
    if(vt_gpu_trace_idle){
      if(vtcuptiActivity->gpuIdleOn){
        /*vt_warning("IDLEexit: %llu (%d)", start, vtCtx->strmList->vtThrdID);*/
        vt_exit(vtcuptiActivity->strmList->vtThrdID, &start);
        vtcuptiActivity->gpuIdleOn = 0;
      }else if(start > vtcuptiActivity->vtLastGPUTime){
        /* idle is off and kernels are consecutive */
        /*vt_warning("IDLEente: %llu (%d)", vtCtx->vtLastGPUTime, vtCtx->strmList->vtThrdID);
        vt_warning("IDLEexit: %llu (%d)", start, vtCtx->strmList->vtThrdID);*/
        vt_enter(vtcuptiActivity->strmList->vtThrdID, &(vtcuptiActivity->vtLastGPUTime), vt_gpu_rid_idle);
        vt_exit(vtcuptiActivity->strmList->vtThrdID, &start);
      }
    }

    vt_enter(vtThrdID, &start, knRID);
    /*vt_warning("KERNente: %llu (%d)", start, vtThrdID);*/
    
    /* use counter to provide additional information for kernels */
    if(vt_gpu_trace_kernels > 1){
      /* grid and block size counter (start) */
      {
        uint32_t threadsPerBlock = kernel->blockX * kernel->blockY * kernel->blockZ;
        uint32_t blocksPerGrid = kernel->gridX * kernel->gridY * kernel->gridZ;

        vt_count(vtThrdID, &start, vt_cupti_cid_blocksPerGrid, 
                 blocksPerGrid);
        vt_count(vtThrdID, &start, vt_cupti_cid_threadsPerBlock, 
                 threadsPerBlock);
        vt_count(vtThrdID, &start, vt_cupti_cid_threadsPerKernel,
                 threadsPerBlock * blocksPerGrid);
      }

      /* memory counter (start) */
      vt_count(vtThrdID, &start, vt_cuptiact_cid_knStaticSharedMem,
               kernel->staticSharedMemory);
      vt_count(vtThrdID, &start, vt_cuptiact_cid_knDynamicSharedMem,
               kernel->dynamicSharedMemory);
      vt_count(vtThrdID, &start, vt_cuptiact_cid_knLocalMemTotal,
               kernel->localMemoryTotal);
      vt_count(vtThrdID, &start, vt_cuptiact_cid_knRegistersPerThread,
               kernel->registersPerThread);

      /* memory counter (stop) */
      vt_count(vtThrdID, &stop, vt_cuptiact_cid_knStaticSharedMem, 0);
      vt_count(vtThrdID, &stop, vt_cuptiact_cid_knDynamicSharedMem, 0);
      vt_count(vtThrdID, &stop, vt_cuptiact_cid_knLocalMemTotal, 0);
      vt_count(vtThrdID, &stop, vt_cuptiact_cid_knRegistersPerThread, 0);

      /* grid and block size counter (stop) */
      vt_count(vtThrdID, &stop, vt_cupti_cid_blocksPerGrid, 0);
      vt_count(vtThrdID, &stop, vt_cupti_cid_threadsPerBlock, 0);
      vt_count(vtThrdID, &stop, vt_cupti_cid_threadsPerKernel, 0);
    }
    
    vt_exit(vtThrdID, &stop);
    /*vt_warning("KERNexit: %llu (%d)", stop, vtThrdID);*/
    
    if(vtcuptiActivity->vtLastGPUTime < stop) vtcuptiActivity->vtLastGPUTime = stop;
  }

  /*vt_cntl_msg(1, "KERNEL '%s' [%llu ns] device %u, context %u, stream %u, "
                 "correlation %u/r%u\n"
                 "\t grid [%u,%u,%u], block [%u,%u,%u], "
                 "shared memory (static %u, dynamic %u)",
             kernel->name, (unsigned long long)(kernel->end - kernel->start),
             kernel->deviceId, kernel->contextId, kernel->streamId, 
             kernel->correlationId, kernel->runtimeCorrelationId,
             kernel->gridX, kernel->gridY, kernel->gridZ,
             kernel->blockX, kernel->blockY, kernel->blockZ,
             kernel->staticSharedMemory, kernel->dynamicSharedMemory);*/
}

/*
 * Use the CUPTI activity memory copy record to write the corresponding 
 * VampirTrace events.
 * 
 * @param mcpy the CUPTI activity memory copy record
 * @param vtCtx the VampirTrace CUPTI activity context
 */
static void vt_cuptiact_writeMemcpyRecord(CUpti_ActivityMemcpy *mcpy, 
                                          vt_cupti_ctx_t *vtCtx)
{
  vt_cupti_activity_t *vtcuptiActivity = vtCtx->activity;
  vt_gpu_copy_kind_t kind = VT_GPU_COPYDIRECTION_UNKNOWN;

  uint32_t vtThrdID;
  uint64_t start, stop;
  vt_cuptiact_strm_t *vtStrm = NULL;
  
  /*vt_cntl_msg(1,"mcpycopykind: %d (strm %d)", mcpy->copyKind, mcpy->streamId);*/
  if(mcpy->copyKind == CUPTI_ACTIVITY_MEMCPY_KIND_DTOD) return;
  
  start = vtcuptiActivity->sync.hostStart 
                 + (mcpy->start - vtcuptiActivity->sync.gpuStart) * vtcuptiActivity->sync.factor;
  stop = start + (mcpy->end - mcpy->start) * vtcuptiActivity->sync.factor;
  
  VT_SUSPEND_MALLOC_TRACING(vtCtx->ptid);
  /* get VampirTrace thread ID for the kernel's stream */
  vtStrm = vt_cuptiact_checkStream(vtCtx, mcpy->streamId);
  vtThrdID = vtStrm->vtThrdID;
  VT_RESUME_MALLOC_TRACING(vtCtx->ptid);
  
  /* if current activity's start time is before last written timestamp */
  if(start < vtStrm->vtLastTime){
    vt_warning("[CUPTI Activity] Memcpy: start time < last written timestamp! "
               "(CUDA device:stream [%d:%d], Thread ID: %d)", 
               vtCtx->cuDev, vtStrm->strmID, vtStrm->vtThrdID);
      

    if(vtStrm->vtLastTime < stop){
      vt_warning("[CUPTI Activity] Set memcpy start time to sync-point time"
                 "(truncate %.4lf%%)", 
                 (double)(vtStrm->vtLastTime - start)/(double)(stop - start));
      start = vtStrm->vtLastTime;
    }else{
      vt_warning("[CUPTI Activity] Skipping ...");
      return;
    }
  }
  
  /* check if time between start and stop is increasing */
  if(stop < start){
    vt_warning("[CUPTI Activity] Skipping memcpy (start time > stop time) on "
               "CUdevice:Stream %d:%d, Thread ID %d", 
               vtCtx->cuDev, vtStrm->strmID, vtStrm->vtThrdID);
    return;
  }

  /* check if synchronization stop time is before kernel stop time */
  if(vtcuptiActivity->sync.hostStop < stop){
    vt_warning("[CUPTI Activity] Memcpy: sync stop time < stop time! "
               "(CUDA device:stream [%d:%d], Thread ID: %d)", 
               vtCtx->cuDev, vtStrm->strmID, vtStrm->vtThrdID);
      
      /* Write memcpy with sync.hostStop stop time stamp, if possible */
      if(vtcuptiActivity->sync.hostStop > start){
        vt_warning("[CUPTI Activity] Set memcpy-stop-time to sync-point-time "
                   "(truncate %.4lf%%)", 
                   (double)(stop - vtcuptiActivity->sync.hostStop)/
                   (double)(stop - start));
        
        stop = vtcuptiActivity->sync.hostStop;
      }else{
        vt_warning("[CUPTI Activity] Skipping ...");
        return;
      }
  }
  
  /* set the last VampirTrace timestamp, written in this stream */
  vtStrm->vtLastTime = stop;
  
  /* check copy direction */
  if(mcpy->srcKind == CUPTI_ACTIVITY_MEMORY_KIND_DEVICE){
    if(mcpy->dstKind == CUPTI_ACTIVITY_MEMORY_KIND_DEVICE){
      kind = VT_GPU_DEV2DEV;
    }else{
      kind = VT_GPU_DEV2HOST;
    }
  }else{
    if(mcpy->dstKind == CUPTI_ACTIVITY_MEMORY_KIND_DEVICE){
      kind = VT_GPU_HOST2DEV;
    }else{
      kind = VT_GPU_HOST2HOST;
    }
  }
  
  if(vtcuptiActivity->gpuIdleOn == 0 && mcpy->streamId == vtcuptiActivity->defaultStrmID){
    vt_enter(vtcuptiActivity->strmList->vtThrdID, &(vtcuptiActivity->vtLastGPUTime), vt_gpu_rid_idle);
    vtcuptiActivity->gpuIdleOn = 1;
    /*vt_warning("IDLMente: %llu (%d)", vtCtx->vtLastGPUTime, vtCtx->strmList->vtThrdID);*/
  }
  
  /*VT_CUPTI_LOCK();*/
  if(kind != VT_GPU_DEV2DEV) vt_gpu_prop[vtCtx->ptid] |= VTGPU_GPU_COMM;
  vt_gpu_prop[vtThrdID] |= VTGPU_GPU_COMM;
  /*VT_CUPTI_UNLOCK();*/
  /*
  vt_warning("MCPYente: %llu (%d)", start, vtThrdID);
  vt_warning("MCPYexit: %llu (%d)", stop, vtThrdID);
  */
  if(kind == VT_GPU_HOST2DEV){
    vt_mpi_rma_get(vtThrdID, &start, VT_GPU_RANK_ID(vtCtx->ptid),
                   vt_gpu_commCID, 0, mcpy->bytes);
  }else if(kind == VT_GPU_DEV2HOST){
    vt_mpi_rma_put(vtThrdID, &start, VT_GPU_RANK_ID(vtCtx->ptid),
                   vt_gpu_commCID, 0, mcpy->bytes);
  }else if(kind == VT_GPU_DEV2DEV){
    vt_mpi_rma_get(vtThrdID, &start, VT_GPU_RANK_ID(vtThrdID),
                   vt_gpu_commCID, 0, mcpy->bytes);
  }
  
  if(kind != VT_GPU_HOST2HOST){
    vt_mpi_rma_end(vtThrdID, &stop, vt_gpu_commCID, 0);
  }
  
  /*if(vtCtx->vtLastGPUTime < stop) vtCtx->vtLastGPUTime = stop;*/
  
  /*vt_cntl_msg(1, "MEMCPY %llu -> %llu[%llu ns] device %u, context %u, stream %u, "
                     "correlation %u/r%u",
               mcpy->start, mcpy->end, 
               (unsigned long long)(mcpy->end - mcpy->start),
               mcpy->deviceId, mcpy->contextId, mcpy->streamId, 
               mcpy->correlationId, mcpy->runtimeCorrelationId);*/
}

/*
 * Increases the "Allocated CUDA memory" counter.
 *
 * @param ctxUID CUDA context identifier (@see CUPTI callback info)
 * @param devPtr pointer to the allocated memory (needed for vtcudaFree())
 * @param size the number of bytes allocated
 */
void vt_cuptiact_writeMalloc(uint32_t ctxID, CUcontext cuCtx, 
                             void *devPtr, size_t size)
{
  uint64_t vtTime;
  vt_cupti_ctx_t* vtCtx = NULL;
  vt_cupti_activity_t *vtcuptiActivity = NULL;
  vt_cupti_gpumem_t *vtMalloc = NULL;
  
  if(devPtr == NULL) return;
  
  VT_SUSPEND_MALLOC_TRACING(VT_CURRENT_THREAD);
  
  vtMalloc = (vt_cupti_gpumem_t*)malloc(sizeof(vt_cupti_gpumem_t));
  
  vtMalloc->memPtr = devPtr;
  vtMalloc->size = size;
  
  /* check for VampirTrace CUPTI context */
  vtCtx = vt_cupti_getCtx(cuCtx);
  if(vtCtx == NULL){
    vtCtx = vt_cupti_createCtx(cuCtx, VT_CUPTI_NO_CUDA_DEVICE, ctxID, VT_CUPTI_NO_DEVICE_ID);
    vt_cupti_prependCtx(vtCtx);
  }
  
  /* check for VampirTrace CUPTI activity context */
  if(vtCtx->activity == NULL){
    vtCtx->activity = vt_cuptiact_createCtxActivity(cuCtx);
  }
  vtcuptiActivity = vtCtx->activity;

  /* lock the work on the context */
  VT_CUPTI_LOCK();
  
  /* flush activity buffer */
  vt_cuptiact_flushCtxActivities(vtCtx);
  
  /* add malloc entry to list */
  vtMalloc->next = vtcuptiActivity->gpuMemList;
  vtcuptiActivity->gpuMemList = vtMalloc;
  
  /* increase allocated memory counter */
  vtcuptiActivity->gpuMemAllocated += size;

  /* check if first CUDA stream is available */
  if(vtcuptiActivity->strmList == NULL){
    if(vt_gpu_init_time < vt_start_time)
      vt_gpu_init_time = vt_start_time;
        
    vtcuptiActivity->strmList = vt_cuptiact_createStream(vtCtx, vtcuptiActivity->defaultStrmID);
    vt_count(vtcuptiActivity->strmList->vtThrdID, &vt_gpu_init_time, vt_gpu_cid_memusage, 0);
  }
  
  VT_CUPTI_UNLOCK();
  
  VT_RESUME_MALLOC_TRACING(VT_CURRENT_THREAD);
  
  /* write counter value */
  vtTime = vt_pform_wtime();
  vt_count(vtcuptiActivity->strmList->vtThrdID, &vtTime, vt_gpu_cid_memusage, 
           (uint64_t)(vtcuptiActivity->gpuMemAllocated));
}

/*
 * Decreases the "Allocated CUDA memory" counter.
 *
 * @param ctxUID CUDA context identifier (@see CUPTI callback info)
 * @param devPtr pointer to the allocated memory
 */
void vt_cuptiact_writeFree(uint32_t ctxID, CUcontext cuCtx, void *devPtr)
{
  uint64_t vtTime;
  vt_cupti_ctx_t* vtCtx = NULL;
  vt_cupti_activity_t *vtcuptiActivity = NULL;
  vt_cupti_gpumem_t *curMalloc = NULL;
  vt_cupti_gpumem_t *lastMalloc = NULL;

  if(devPtr == NULL) return;
  
  VT_SUSPEND_MALLOC_TRACING(VT_CURRENT_THREAD);
  
  /* check for VampirTrace CUPTI context */
  vtCtx = vt_cupti_getCtx(cuCtx);
  if(vtCtx == NULL){
    
    vtCtx = vt_cupti_createCtx(cuCtx, VT_CUPTI_NO_CUDA_DEVICE, ctxID, VT_CUPTI_NO_DEVICE_ID);
    
    vt_cupti_prependCtx(vtCtx);
  }
  
  /* check for VampirTrace CUPTI activity context */
  if(vtCtx->activity == NULL){
    vtCtx->activity = vt_cuptiact_createCtxActivity(cuCtx);
  }
  vtcuptiActivity = vtCtx->activity;
  
  VT_CUPTI_LOCK();
  
  /* flush activity buffer */
  vt_cuptiact_flushCtxActivities(vtCtx);

  curMalloc = vtcuptiActivity->gpuMemList;
  lastMalloc = curMalloc;

  /* lookup the CUDA malloc entry by its memory pointer */
  while(curMalloc != NULL){
    if(devPtr == curMalloc->memPtr){

      /* decrease allocated counter value and write it */
      vtTime = vt_pform_wtime();
      vtcuptiActivity->gpuMemAllocated -= curMalloc->size;
      vt_count(vtcuptiActivity->strmList->vtThrdID, &vtTime, vt_gpu_cid_memusage,
               (uint64_t)(vtcuptiActivity->gpuMemAllocated));


      /* set pointer over current element to next one */
      lastMalloc->next = curMalloc->next;

      /* if current element is the first list entry, set the list entry */
      if(curMalloc == vtcuptiActivity->gpuMemList){
        vtcuptiActivity->gpuMemList = curMalloc->next;
      }

      /* free VT memory of CUDA malloc */
      curMalloc->next = NULL;
      free(curMalloc);
      curMalloc = NULL;

      /* set mallocList to NULL, if last element freed */
      if(vtcuptiActivity->gpuMemAllocated == 0) {
        vtcuptiActivity->gpuMemList = NULL;
      }
  
      VT_CUPTI_UNLOCK();
      VT_RESUME_MALLOC_TRACING(VT_CURRENT_THREAD);
      return;
    }

    lastMalloc = curMalloc;
    curMalloc = curMalloc->next;
  }

  VT_CUPTI_UNLOCK();
  
  VT_RESUME_MALLOC_TRACING(VT_CURRENT_THREAD);

  vt_warning("[CUPTI Activity] free CUDA memory, which has not been allocated!");
}

#if (defined(CUPTI_API_VERSION) && (CUPTI_API_VERSION >= 3))
void vt_cuptiact_enableConcurrentKernel(vt_cupti_ctx_t* vtCtx)
{
    /* 
     * Disable collection of kernels for the given CUDA context. 
     * !!! does not work yet !!!
     
    VT_CUPTI_CALL(cuptiActivityDisableContext(cuCtx, CUPTI_ACTIVITY_KIND_KERNEL),
                  "cuptiActivityDisableContext");*
  
    * flush the already buffered activities for this CUDA context *
    vt_cuptiact_flushCtxActivities(cuCtx);

    * Enable collection of kernels for the given CUDA context 
    VT_CUPTI_CALL(cuptiActivityEnableContext(cuCtx, CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL), 
                  "cuptiActivityEnableContext");*/
  
  if((vt_gpu_config & VT_GPU_TRACE_CONCURRENT_KERNEL) 
         != VT_GPU_TRACE_CONCURRENT_KERNEL){

    vt_cntl_msg(2, "[CUPTI Activity] Enable concurrent kernel tracing.");
    
    /*
     * Disable normal (lower overhead) kernel tracing.
     */
    VT_CUPTI_CALL(cuptiActivityDisable(CUPTI_ACTIVITY_KIND_KERNEL),
                  "cuptiActivityDisable");
    
    /* 
     * Flush the already buffered activities for this CUDA context.
     */
    VT_CUPTI_LOCK();
    vt_cuptiact_flushCtxActivities(vtCtx);
    VT_CUPTI_UNLOCK();

    /*
     * Enable concurrent kernel tracing (higher overhead).
     */
    VT_CUPTI_CALL(cuptiActivityEnable(CUPTI_ACTIVITY_KIND_CONCURRENT_KERNEL), 
                  "cuptiActivityEnable");
    
    vt_gpu_config |= VT_GPU_TRACE_CONCURRENT_KERNEL;
  }
}
#endif
