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
 * Copyright (c) 2009      Cisco Systems, Inc.  All rights reserved.
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */
#ifndef OMPI_F77_PROFILE_DEFINES_H
#define OMPI_F77_PROFILE_DEFINES_H

#define mpi_abort_f pmpi_abort_f
#define mpi_accumulate_f pmpi_accumulate_f
#define mpi_add_error_class_f pmpi_add_error_class_f
#define mpi_add_error_code_f pmpi_add_error_code_f
#define mpi_add_error_string_f pmpi_add_error_string_f
#define mpi_address_f pmpi_address_f
#define mpi_allgather_f pmpi_allgather_f
#define mpi_allgatherv_f pmpi_allgatherv_f
#define mpi_alloc_mem_f pmpi_alloc_mem_f
#define mpi_allreduce_f pmpi_allreduce_f
#define mpi_alltoall_f pmpi_alltoall_f
#define mpi_alltoallv_f pmpi_alltoallv_f
#define mpi_alltoallw_f pmpi_alltoallw_f
#define mpi_attr_delete_f pmpi_attr_delete_f
#define mpi_attr_get_f pmpi_attr_get_f
#define mpi_attr_put_f pmpi_attr_put_f
#define mpi_barrier_f pmpi_barrier_f
#define mpi_bcast_f pmpi_bcast_f
#define mpi_bsend_f pmpi_bsend_f
#define mpi_bsend_init_f pmpi_bsend_init_f
#define mpi_buffer_attach_f pmpi_buffer_attach_f
#define mpi_buffer_detach_f pmpi_buffer_detach_f
#define mpi_cancel_f pmpi_cancel_f
#define mpi_cart_coords_f pmpi_cart_coords_f
#define mpi_cart_create_f pmpi_cart_create_f
#define mpi_cart_get_f pmpi_cart_get_f
#define mpi_cart_map_f pmpi_cart_map_f
#define mpi_cart_rank_f pmpi_cart_rank_f
#define mpi_cart_shift_f pmpi_cart_shift_f
#define mpi_cart_sub_f pmpi_cart_sub_f
#define mpi_cartdim_get_f pmpi_cartdim_get_f
#define mpi_close_port_f pmpi_close_port_f
#define mpi_comm_accept_f pmpi_comm_accept_f
#define mpi_comm_call_errhandler_f pmpi_comm_call_errhandler_f
#define mpi_comm_compare_f pmpi_comm_compare_f
#define mpi_comm_connect_f pmpi_comm_connect_f
#define mpi_comm_create_errhandler_f pmpi_comm_create_errhandler_f
#define mpi_comm_create_keyval_f pmpi_comm_create_keyval_f
#define mpi_comm_create_f pmpi_comm_create_f
#define mpi_comm_delete_attr_f pmpi_comm_delete_attr_f
#define mpi_comm_disconnect_f pmpi_comm_disconnect_f
#define mpi_comm_dup_f pmpi_comm_dup_f
#define mpi_comm_free_keyval_f pmpi_comm_free_keyval_f
#define mpi_comm_free_f pmpi_comm_free_f
#define mpi_comm_get_attr_f pmpi_comm_get_attr_f
#define mpi_comm_get_errhandler_f pmpi_comm_get_errhandler_f
#define mpi_comm_get_name_f pmpi_comm_get_name_f
#define mpi_comm_get_parent_f pmpi_comm_get_parent_f
#define mpi_comm_group_f pmpi_comm_group_f
#define mpi_comm_join_f pmpi_comm_join_f
#define mpi_comm_rank_f pmpi_comm_rank_f
#define mpi_comm_remote_group_f pmpi_comm_remote_group_f
#define mpi_comm_remote_size_f pmpi_comm_remote_size_f
#define mpi_comm_set_attr_f pmpi_comm_set_attr_f
#define mpi_comm_set_errhandler_f pmpi_comm_set_errhandler_f
#define mpi_comm_set_name_f pmpi_comm_set_name_f
#define mpi_comm_size_f pmpi_comm_size_f
#define mpi_comm_spawn_f pmpi_comm_spawn_f
#define mpi_comm_spawn_multiple_f pmpi_comm_spawn_multiple_f
#define mpi_comm_split_f pmpi_comm_split_f
#define mpi_comm_test_inter_f pmpi_comm_test_inter_f
#define mpi_dims_create_f pmpi_dims_create_f
#define mpi_errhandler_create_f pmpi_errhandler_create_f
#define mpi_errhandler_free_f pmpi_errhandler_free_f
#define mpi_errhandler_get_f pmpi_errhandler_get_f
#define mpi_errhandler_set_f pmpi_errhandler_set_f
#define mpi_error_class_f pmpi_error_class_f
#define mpi_error_string_f pmpi_error_string_f
#define mpi_exscan_f pmpi_exscan_f
#define mpi_file_call_errhandler_f pmpi_file_call_errhandler_f
#define mpi_file_create_errhandler_f pmpi_file_create_errhandler_f
#define mpi_file_set_errhandler_f pmpi_file_set_errhandler_f
#define mpi_file_get_errhandler_f pmpi_file_get_errhandler_f
#define mpi_file_open_f pmpi_file_open_f
#define mpi_file_close_f pmpi_file_close_f
#define mpi_file_delete_f pmpi_file_delete_f
#define mpi_file_set_size_f pmpi_file_set_size_f
#define mpi_file_preallocate_f pmpi_file_preallocate_f
#define mpi_file_get_size_f pmpi_file_get_size_f
#define mpi_file_get_group_f pmpi_file_get_group_f
#define mpi_file_get_amode_f pmpi_file_get_amode_f
#define mpi_file_set_info_f pmpi_file_set_info_f
#define mpi_file_get_info_f pmpi_file_get_info_f
#define mpi_file_set_view_f pmpi_file_set_view_f
#define mpi_file_get_view_f pmpi_file_get_view_f
#define mpi_file_read_at_f pmpi_file_read_at_f
#define mpi_file_read_at_all_f pmpi_file_read_at_all_f
#define mpi_file_write_at_f pmpi_file_write_at_f
#define mpi_file_write_at_all_f pmpi_file_write_at_all_f
#define mpi_file_iread_at_f pmpi_file_iread_at_f
#define mpi_file_iwrite_at_f pmpi_file_iwrite_at_f
#define mpi_file_read_f pmpi_file_read_f
#define mpi_file_read_all_f pmpi_file_read_all_f
#define mpi_file_write_f pmpi_file_write_f
#define mpi_file_write_all_f pmpi_file_write_all_f
#define mpi_file_iread_f pmpi_file_iread_f
#define mpi_file_iwrite_f pmpi_file_iwrite_f
#define mpi_file_seek_f pmpi_file_seek_f
#define mpi_file_get_position_f pmpi_file_get_position_f
#define mpi_file_get_byte_offset_f pmpi_file_get_byte_offset_f
#define mpi_file_read_shared_f pmpi_file_read_shared_f
#define mpi_file_write_shared_f pmpi_file_write_shared_f
#define mpi_file_iread_shared_f pmpi_file_iread_shared_f
#define mpi_file_iwrite_shared_f pmpi_file_iwrite_shared_f
#define mpi_file_read_ordered_f pmpi_file_read_ordered_f
#define mpi_file_write_ordered_f pmpi_file_write_ordered_f
#define mpi_file_seek_shared_f pmpi_file_seek_shared_f
#define mpi_file_get_position_shared_f pmpi_file_get_position_shared_f
#define mpi_file_read_at_all_begin_f pmpi_file_read_at_all_begin_f
#define mpi_file_read_at_all_end_f pmpi_file_read_at_all_end_f
#define mpi_file_write_at_all_begin_f pmpi_file_write_at_all_begin_f
#define mpi_file_write_at_all_end_f pmpi_file_write_at_all_end_f
#define mpi_file_read_all_begin_f pmpi_file_read_all_begin_f
#define mpi_file_read_all_end_f pmpi_file_read_all_end_f
#define mpi_file_write_all_begin_f pmpi_file_write_all_begin_f
#define mpi_file_write_all_end_f pmpi_file_write_all_end_f
#define mpi_file_read_ordered_begin_f pmpi_file_read_ordered_begin_f
#define mpi_file_read_ordered_end_f pmpi_file_read_ordered_end_f
#define mpi_file_write_ordered_begin_f pmpi_file_write_ordered_begin_f
#define mpi_file_write_ordered_end_f pmpi_file_write_ordered_end_f
#define mpi_file_get_type_extent_f pmpi_file_get_type_extent_f
#define mpi_file_set_atomicity_f pmpi_file_set_atomicity_f
#define mpi_file_get_atomicity_f pmpi_file_get_atomicity_f
#define mpi_file_sync_f pmpi_file_sync_f
#define mpi_finalize_f pmpi_finalize_f
#define mpi_finalized_f pmpi_finalized_f
#define mpi_free_mem_f pmpi_free_mem_f
#define mpi_gather_f pmpi_gather_f
#define mpi_gatherv_f pmpi_gatherv_f
#define mpi_get_address_f pmpi_get_address_f
#define mpi_get_count_f pmpi_get_count_f
#define mpi_get_elements_f pmpi_get_elements_f
#define mpi_get_f pmpi_get_f
#define mpi_get_processor_name_f pmpi_get_processor_name_f
#define mpi_get_version_f pmpi_get_version_f
#define mpi_graph_create_f pmpi_graph_create_f
#define mpi_graph_get_f pmpi_graph_get_f
#define mpi_graph_map_f pmpi_graph_map_f
#define mpi_graph_neighbors_count_f pmpi_graph_neighbors_count_f
#define mpi_graph_neighbors_f pmpi_graph_neighbors_f
#define mpi_graphdims_get_f pmpi_graphdims_get_f
#define mpi_grequest_complete_f pmpi_grequest_complete_f
#define mpi_grequest_start_f pmpi_grequest_start_f
#define mpi_group_compare_f pmpi_group_compare_f
#define mpi_group_difference_f pmpi_group_difference_f
#define mpi_group_excl_f pmpi_group_excl_f
#define mpi_group_free_f pmpi_group_free_f
#define mpi_group_incl_f pmpi_group_incl_f
#define mpi_group_intersection_f pmpi_group_intersection_f
#define mpi_group_range_excl_f pmpi_group_range_excl_f
#define mpi_group_range_incl_f pmpi_group_range_incl_f
#define mpi_group_rank_f pmpi_group_rank_f
#define mpi_group_size_f pmpi_group_size_f
#define mpi_group_translate_ranks_f pmpi_group_translate_ranks_f
#define mpi_group_union_f pmpi_group_union_f
#define mpi_ibsend_f pmpi_ibsend_f
#define mpi_info_create_f pmpi_info_create_f
#define mpi_info_delete_f pmpi_info_delete_f
#define mpi_info_dup_f pmpi_info_dup_f
#define mpi_info_free_f pmpi_info_free_f
#define mpi_info_get_f pmpi_info_get_f
#define mpi_info_get_nkeys_f pmpi_info_get_nkeys_f
#define mpi_info_get_nthkey_f pmpi_info_get_nthkey_f
#define mpi_info_get_valuelen_f pmpi_info_get_valuelen_f
#define mpi_info_set_f pmpi_info_set_f
#define mpi_init_f pmpi_init_f
#define mpi_initialized_f pmpi_initialized_f
#define mpi_init_thread_f pmpi_init_thread_f
#define mpi_intercomm_create_f pmpi_intercomm_create_f
#define mpi_intercomm_merge_f pmpi_intercomm_merge_f
#define mpi_iprobe_f pmpi_iprobe_f
#define mpi_irecv_f pmpi_irecv_f
#define mpi_irsend_f pmpi_irsend_f
#define mpi_isend_f pmpi_isend_f
#define mpi_issend_f pmpi_issend_f
#define mpi_is_thread_main_f pmpi_is_thread_main_f
#define mpi_keyval_create_f pmpi_keyval_create_f
#define mpi_keyval_free_f pmpi_keyval_free_f
#define mpi_lookup_name_f pmpi_lookup_name_f
#define mpi_op_commutative_f pmpi_op_commutative_f
#define mpi_op_create_f pmpi_op_create_f
#define mpi_open_port_f pmpi_open_port_f
#define mpi_op_free_f pmpi_op_free_f
#define mpi_pack_external_f pmpi_pack_external_f
#define mpi_pack_external_size_f pmpi_pack_external_size_f
#define mpi_pack_f pmpi_pack_f
#define mpi_pack_size_f pmpi_pack_size_f
#define mpi_pcontrol_f pmpi_pcontrol_f
#define mpi_probe_f pmpi_probe_f
#define mpi_publish_name_f pmpi_publish_name_f
#define mpi_put_f pmpi_put_f
#define mpi_query_thread_f pmpi_query_thread_f
#define mpi_recv_init_f pmpi_recv_init_f
#define mpi_recv_f pmpi_recv_f
#define mpi_reduce_f pmpi_reduce_f
#define mpi_reduce_local_f pmpi_reduce_local_f
#define mpi_reduce_scatter_f pmpi_reduce_scatter_f
#define mpi_register_datarep_f pmpi_register_datarep_f
#define mpi_request_free_f pmpi_request_free_f
#define mpi_request_get_status_f pmpi_request_get_status_f
#define mpi_rsend_f pmpi_rsend_f
#define mpi_rsend_init_f pmpi_rsend_init_f
#define mpi_scan_f pmpi_scan_f
#define mpi_scatter_f pmpi_scatter_f
#define mpi_scatterv_f pmpi_scatterv_f
#define mpi_send_init_f pmpi_send_init_f
#define mpi_send_f pmpi_send_f
#define mpi_sendrecv_f pmpi_sendrecv_f
#define mpi_sendrecv_replace_f pmpi_sendrecv_replace_f
#define mpi_ssend_init_f pmpi_ssend_init_f
#define mpi_ssend_f pmpi_ssend_f
#define mpi_start_f pmpi_start_f
#define mpi_startall_f pmpi_startall_f
#define mpi_status_set_cancelled_f pmpi_status_set_cancelled_f
#define mpi_status_set_elements_f pmpi_status_set_elements_f
#define mpi_testall_f pmpi_testall_f
#define mpi_testany_f pmpi_testany_f
#define mpi_test_f pmpi_test_f
#define mpi_test_cancelled_f pmpi_test_cancelled_f
#define mpi_testsome_f pmpi_testsome_f
#define mpi_topo_test_f pmpi_topo_test_f
#define mpi_type_commit_f pmpi_type_commit_f
#define mpi_type_contiguous_f pmpi_type_contiguous_f
#define mpi_type_create_darray_f pmpi_type_create_darray_f
#define mpi_type_create_f90_complex_f pmpi_type_create_f90_complex_f
#define mpi_type_create_f90_integer_f pmpi_type_create_f90_integer_f
#define mpi_type_create_f90_real_f pmpi_type_create_f90_real_f
#define mpi_type_create_hindexed_f pmpi_type_create_hindexed_f
#define mpi_type_create_hvector_f pmpi_type_create_hvector_f
#define mpi_type_create_keyval_f pmpi_type_create_keyval_f
#define mpi_type_create_indexed_block_f pmpi_type_create_indexed_block_f
#define mpi_type_create_struct_f pmpi_type_create_struct_f
#define mpi_type_create_subarray_f pmpi_type_create_subarray_f
#define mpi_type_create_resized_f pmpi_type_create_resized_f
#define mpi_type_delete_attr_f pmpi_type_delete_attr_f
#define mpi_type_dup_f pmpi_type_dup_f
#define mpi_type_extent_f pmpi_type_extent_f
#define mpi_type_free_f pmpi_type_free_f
#define mpi_type_free_keyval_f pmpi_type_free_keyval_f
#define mpi_type_get_attr_f pmpi_type_get_attr_f
#define mpi_type_get_contents_f pmpi_type_get_contents_f
#define mpi_type_get_envelope_f pmpi_type_get_envelope_f
#define mpi_type_get_extent_f pmpi_type_get_extent_f
#define mpi_type_get_name_f pmpi_type_get_name_f
#define mpi_type_get_true_extent_f pmpi_type_get_true_extent_f
#define mpi_type_hindexed_f pmpi_type_hindexed_f
#define mpi_type_hvector_f pmpi_type_hvector_f
#define mpi_type_indexed_f pmpi_type_indexed_f
#define mpi_type_lb_f pmpi_type_lb_f
#define mpi_type_match_size_f pmpi_type_match_size_f
#define mpi_type_set_attr_f pmpi_type_set_attr_f
#define mpi_type_set_name_f pmpi_type_set_name_f
#define mpi_type_size_f pmpi_type_size_f
#define mpi_type_struct_f pmpi_type_struct_f
#define mpi_type_ub_f pmpi_type_ub_f
#define mpi_type_vector_f pmpi_type_vector_f
#define mpi_unpack_f pmpi_unpack_f
#define mpi_unpublish_name_f pmpi_unpublish_name_f
#define mpi_unpack_external_f pmpi_unpack_external_f
#define mpi_waitall_f pmpi_waitall_f
#define mpi_waitany_f pmpi_waitany_f
#define mpi_wait_f pmpi_wait_f
#define mpi_waitsome_f pmpi_waitsome_f
#define mpi_win_call_errhandler_f pmpi_win_call_errhandler_f
#define mpi_win_complete_f pmpi_win_complete_f
#define mpi_win_create_f pmpi_win_create_f
#define mpi_win_create_errhandler_f pmpi_win_create_errhandler_f
#define mpi_win_create_keyval_f pmpi_win_create_keyval_f
#define mpi_win_delete_attr_f pmpi_win_delete_attr_f
#define mpi_win_fence_f pmpi_win_fence_f
#define mpi_win_free_f pmpi_win_free_f
#define mpi_win_free_keyval_f pmpi_win_free_keyval_f
#define mpi_win_get_attr_f pmpi_win_get_attr_f
#define mpi_win_get_errhandler_f pmpi_win_get_errhandler_f
#define mpi_win_get_group_f pmpi_win_get_group_f
#define mpi_win_get_name_f pmpi_win_get_name_f
#define mpi_win_lock_f pmpi_win_lock_f
#define mpi_win_post_f pmpi_win_post_f
#define mpi_win_set_attr_f pmpi_win_set_attr_f
#define mpi_win_set_errhandler_f pmpi_win_set_errhandler_f
#define mpi_win_set_name_f pmpi_win_set_name_f
#define mpi_win_start_f pmpi_win_start_f
#define mpi_win_test_f pmpi_win_test_f
#define mpi_win_unlock_f pmpi_win_unlock_f
#define mpi_win_wait_f pmpi_win_wait_f
#define mpi_wtick_f pmpi_wtick_f
#define mpi_wtime_f pmpi_wtime_f
#endif

