/*
 * Copyright (c) 2004-2005 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2005 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart, 
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "btl_openib_frag.h" 
#include "mca/mpool/openib/mpool_openib.h" 



static void mca_btl_openib_frag_common_constructor( mca_btl_openib_frag_t* frag) 
{
    mca_mpool_openib_registration_t* registration = 
        (mca_mpool_openib_registration_t*) frag->base.super.user_data;
    
    frag->hdr = (mca_btl_openib_header_t*) (frag+1);    /* initialize the btl header to start at end of frag */ 
    frag->segment.seg_addr.pval = ((unsigned char* )frag->hdr) + sizeof(mca_btl_openib_header_t);  
    /* init the segment address to start after the btl header */ 
    
    frag->mr = registration->mr; 
    frag->segment.seg_len = frag->size;
    frag->segment.seg_key.key32[0] = (uint32_t) frag->mr->lkey; 
    frag->sg_entry.addr = (uint64_t) frag->hdr; 
    frag->sg_entry.length = frag->size; 
    frag->sg_entry.lkey = frag->mr->lkey; 
    frag->base.des_flags = 0; 
}

 
static void mca_btl_openib_send_frag_common_constructor(mca_btl_openib_frag_t* frag) 
{ 
    
    mca_btl_openib_frag_common_constructor(frag); 
    frag->base.des_src = &frag->segment;
    frag->base.des_src_cnt = 1;
    frag->base.des_dst = NULL;
    frag->base.des_dst_cnt = 0;
    
    frag->wr_desc.sr_desc.wr_id = (uint64_t) frag; 
    frag->wr_desc.sr_desc.sg_list = &frag->sg_entry; 
    frag->wr_desc.sr_desc.num_sge = 1; 
    frag->wr_desc.sr_desc.opcode = IBV_WR_SEND; 
    frag->wr_desc.sr_desc.send_flags = IBV_SEND_SIGNALED; 
    frag->wr_desc.sr_desc.next = NULL; 
}

static void mca_btl_openib_recv_frag_common_constructor(mca_btl_openib_frag_t* frag) 
{ 
    
    mca_btl_openib_frag_common_constructor(frag); 
    frag->base.des_dst = &frag->segment;
    frag->base.des_dst_cnt = 1;
    frag->base.des_src = NULL;
    frag->base.des_src_cnt = 0;
   
    frag->wr_desc.rd_desc.wr_id = (uint64_t) frag; 
    frag->wr_desc.rd_desc.sg_list = &frag->sg_entry; 
    frag->wr_desc.rd_desc.num_sge = 1; 
    frag->wr_desc.rd_desc.next = NULL; 
}

static void mca_btl_openib_send_frag_eager_constructor(mca_btl_openib_frag_t* frag) 
{ 
    
    frag->size = mca_btl_openib_component.eager_limit;  
    mca_btl_openib_send_frag_common_constructor(frag); 
}


static void mca_btl_openib_send_frag_max_constructor(mca_btl_openib_frag_t* frag) 
{ 
    
    frag->size = mca_btl_openib_component.max_send_size; 
    mca_btl_openib_send_frag_common_constructor(frag); 
}

static void mca_btl_openib_recv_frag_max_constructor(mca_btl_openib_frag_t* frag) 
{
    frag->size = mca_btl_openib_component.max_send_size; 
    mca_btl_openib_recv_frag_common_constructor(frag); 
    
}


static void mca_btl_openib_recv_frag_eager_constructor(mca_btl_openib_frag_t* frag) 
{
    frag->size = mca_btl_openib_component.eager_limit; 
    mca_btl_openib_recv_frag_common_constructor(frag); 
    
}

static void mca_btl_openib_send_frag_frag_constructor(mca_btl_openib_frag_t* frag) 
{ 
    
    frag->size = 0; 
    mca_btl_openib_send_frag_common_constructor(frag); 
}


OBJ_CLASS_INSTANCE(
                   mca_btl_openib_frag_t, 
                   mca_btl_base_descriptor_t, 
                   NULL, 
                   NULL); 

OBJ_CLASS_INSTANCE(
                   mca_btl_openib_send_frag_eager_t, 
                   mca_btl_base_descriptor_t, 
                   mca_btl_openib_send_frag_eager_constructor, 
                   NULL); 


OBJ_CLASS_INSTANCE(
                   mca_btl_openib_send_frag_max_t, 
                   mca_btl_base_descriptor_t, 
                   mca_btl_openib_send_frag_max_constructor, 
                   NULL); 

OBJ_CLASS_INSTANCE(
                   mca_btl_openib_send_frag_frag_t, 
                   mca_btl_base_descriptor_t, 
                   mca_btl_openib_send_frag_frag_constructor, 
                   NULL); 

OBJ_CLASS_INSTANCE(
                   mca_btl_openib_recv_frag_eager_t, 
                   mca_btl_base_descriptor_t, 
                   mca_btl_openib_recv_frag_eager_constructor, 
                   NULL); 


OBJ_CLASS_INSTANCE(
                   mca_btl_openib_recv_frag_max_t, 
                   mca_btl_base_descriptor_t, 
                   mca_btl_openib_recv_frag_max_constructor, 
                   NULL); 


