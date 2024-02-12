/*
 * (C) Copyright 2023-2024 Intel Corporation.
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef __CRT_PERF_H__
#define __CRT_PERF_H__

#include "crt_perf_mpi.h"

#include <cart/api.h>

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

struct crt_perf_info {
	struct crt_perf_opts          opts;
	struct crt_perf_context_info *context_info;
	struct crt_perf_mpi_info      mpi_info;
	crt_group_t                  *ep_group;
	uint32_t                      ep_ranks;
	uint32_t                      ep_tags;
};

struct crt_perf_rpc {
	crt_endpoint_t endpoint; /* Destination endpoint */
	crt_rpc_t     *rpc;      /* RPC request */
};

struct crt_perf_context_info {
	crt_context_t        context;
	struct crt_perf_rpc *requests;
	void                *rpc_buf;
	int                  context_id;
	bool                 done;
};

struct crt_perf_request {
	int32_t expected_count;         /* Expected count */
	int32_t complete_count;         /* Completed count */
	int     rc;                     /* Callback return code */
	bool    done;                   /* Request */
	int (*cb)(crt_rpc_t *, void *); /* Callback */
	void *arg;                      /* Callback arg */
};

enum crt_perf_rpc_id { CRT_PERF_RATE, CRT_PERF_DONE, CRT_PERF_TAGS };

/*****************/
/* Public Macros */
/*****************/

#define CRT_PERF_LAT_SKIP_SMALL 100
#define CRT_PERF_LAT_SKIP_LARGE 10
#define CRT_PERF_LARGE_SIZE     8192

#define CRT_PERF_TIMEOUT        (1000 * 1000) /* us */

#define CRT_PERF_BASE_OPC       0x010000000
#define CRT_PERF_RPC_VERSION    0

#define CRT_PERF_ID(x)          CRT_PROTO_OPC(CRT_PERF_BASE_OPC, CRT_PERF_RPC_VERSION, x)

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
crt_perf_rpc_set_req(const struct crt_perf_info *perf_info, struct crt_perf_context_info *info);

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

#endif /* __CRT_PERF_H__ */
