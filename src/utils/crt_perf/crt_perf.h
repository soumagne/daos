/*
 * (C) Copyright 2023-2024 Intel Corporation.
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef __CRT_PERF_H__
#define __CRT_PERF_H__

#include <cart/api.h>
#include <gurt/atomic.h>
#include <gurt/common.h>
#include <stdio.h>

#include <sys/uio.h>

/*************************************/
/* Public Type and Struct Definition */
/*************************************/

struct crt_perf_opts {
	char  *comm;
	char  *domain;
	char  *protocol;
	char  *hostname;
	char  *port;
	char  *attach_path;
	size_t msg_size_max;
	size_t buf_size_min;
	size_t buf_size_max;
	size_t context_max;
	size_t request_max;
	size_t buf_count;
	int    loop;
	bool   busy_wait;
	bool   bidir;
	bool   verify;
	bool   mbps;
};

struct crt_mpi_info {
	int rank;
	int size;
};

struct crt_perf_info {
	struct crt_perf_opts          opts;
	struct crt_perf_context_info *context_info;
	struct crt_mpi_info           mpi_info;
	crt_group_t                  *ep_group;
	uint32_t                      ep_ranks;
	uint32_t                      ep_tags;
};

struct crt_perf_context_info {
	crt_context_t context;
	crt_rpc_t   **requests;
	void         *rpc_buf;
	int           context_id;
	bool          done;
};

struct crt_perf_request {
	int32_t expected_count; /* Expected count */
	int32_t complete_count; /* Completed count */
	int     rc;             /* Callback return code */
	bool    done;           /* Request */
	int (*cb)(crt_rpc_t *); /* Callback */
};

/*****************/
/* Public Macros */
/*****************/

#define CRT_PERF_LAT_SKIP_SMALL 100
#define CRT_PERF_LAT_SKIP_LARGE 10
#define CRT_PERF_LARGE_SIZE     8192

#define CRT_PERF_BASE_OPC       0x010000000
#define CRT_PERF_RPC_VERSION    0

#define CRT_PERF_RATE_ID        CRT_PROTO_OPC(CRT_PERF_BASE_OPC, CRT_PERF_RPC_VERSION, 0)
#define CRT_PERF_DONE_ID        CRT_PROTO_OPC(CRT_PERF_BASE_OPC, CRT_PERF_RPC_VERSION, 1)

/* Check for D_ ret value and goto label */
#define CRT_PERF_CHECK_D_ERROR(label, rc, ...)                                                     \
	do {                                                                                       \
		if (unlikely(rc != 0)) {                                                           \
			DL_ERROR(rc, __VA_ARGS__);                                                 \
			goto label;                                                                \
		}                                                                                  \
	} while (0)

/* Check for cond, set ret to err_val and goto label */
#define CRT_PERF_CHECK_ERROR(cond, label, rc, err_val, ...)                                        \
	do {                                                                                       \
		if (unlikely(cond)) {                                                              \
			rc = err_val;                                                              \
			DL_ERROR(rc, __VA_ARGS__);                                                 \
			goto label;                                                                \
		}                                                                                  \
	} while (0)

/*********************/
/* Public Prototypes */
/*********************/

int
crt_perf_init(int argc, char *argv[], bool listen, struct crt_perf_info *info);

void
crt_perf_cleanup(struct crt_perf_info *info);

int
crt_perf_rpc_buf_init(const struct crt_perf_info *perf_info, struct crt_perf_context_info *info);

void
crt_perf_print_header_lat(const struct crt_perf_info         *perf_info,
			  const struct crt_perf_context_info *info, const char *benchmark);

void
crt_perf_print_lat(const struct crt_perf_info *perf_info, const struct crt_perf_context_info *info,
		   size_t buf_size, struct timespec t);

int
crt_perf_verify_data(const void *buf, size_t buf_size);

void
crt_perf_request_complete(const struct crt_cb_info *cb_info);

int
crt_perf_send_done(const struct crt_perf_info *perf_info, struct crt_perf_context_info *info);

int
crt_perf_mpi_init(struct crt_mpi_info *mpi_info);

void
crt_perf_barrier(const struct crt_perf_info *perf_info);

#endif /* __CRT_PERF_H__ */
