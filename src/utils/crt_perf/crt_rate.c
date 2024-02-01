/*
 * (C) Copyright 2017-2023 Intel Corporation.
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "crt_perf.h"

/****************/
/* Local Macros */
/****************/

#define BENCHMARK_NAME "RPC rate"

/********************/
/* Local Prototypes */
/********************/

static int
crt_perf_run(const struct crt_perf_info *perf_info, struct crt_perf_context_info *info,
	     size_t buf_size, size_t skip);

static int
crt_perf_rpc_verify(crt_rpc_t *rpc);

static int
crt_perf_run(const struct crt_perf_info *perf_info, struct crt_perf_context_info *info,
	     size_t buf_size, size_t skip)
{
	struct timespec             t1, t2;
	size_t                      i;
	int                         rc;
	const struct crt_perf_opts *opts = &perf_info->opts;

	/* Warm up for RPC */
	for (i = 0; i < skip + (size_t)opts->loop; i++) {
		struct crt_perf_request args = {
		    .expected_count = (int32_t)opts->request_max,
		    .complete_count = 0,
		    .rc             = 0,
		    .done           = false,
		    .cb             = (opts->verify && opts->bidir) ? crt_perf_rpc_verify : NULL};
		crt_endpoint_t target_ep = {
		    .ep_grp = perf_info->ep_group, .ep_rank = 0, .ep_tag = 0};
		unsigned int j;

		if (i == skip) {
			if (perf_info->mpi_info.size > 1)
				crt_perf_barrier(perf_info);
			d_gettime(&t1);
		}

		for (j = 0; j < opts->request_max; j++) {
			struct iovec *in_iov;

			rc = crt_req_create(info->context, &target_ep, CRT_PERF_RATE_ID,
					    &info->requests[j]);
			if (rc != 0) {
				DL_ERROR(rc, "crt_req_create() failed");
				goto error;
			}

			in_iov           = crt_req_get(info->requests[j]);
			in_iov->iov_base = info->rpc_buf;
			in_iov->iov_len  = buf_size;

			rc = crt_req_send(info->requests[j], crt_perf_request_complete, &args);
			if (rc != 0) {
				DL_ERROR(rc, "crt_req_send() failed");
				goto error;
			}
		}

		while (!args.done) {
			rc = crt_progress(info->context, 1000 * 1000);
			if (rc != 0 && rc != -DER_TIMEDOUT) {
				DL_ERROR(rc, "crt_progress() failed");
				goto error;
			}
		}
	}

	if (perf_info->mpi_info.size > 1)
		crt_perf_barrier(perf_info);

	d_gettime(&t2);

	if (perf_info->mpi_info.rank == 0)
		crt_perf_print_lat(perf_info, info, buf_size, d_timediff(t1, t2));

	return 0;

error:
	return rc;
}

static int
crt_perf_rpc_verify(crt_rpc_t *rpc)
{
	struct iovec *out_iov;
	int           rc;

	out_iov = crt_reply_get(rpc);
	if (out_iov == NULL) {
		rc = -DER_INVAL;
		DL_ERROR(rc, "crt_reply_get() failed");
		goto error;
	}

	rc = crt_perf_verify_data(out_iov->iov_base, out_iov->iov_len);
	if (rc != 0) {
		DL_ERROR(rc, "crt_perf_verify_data() failed");
		goto error;
	}

	return 0;

error:
	return rc;
}

int
main(int argc, char **argv)
{
	struct crt_perf_info          perf_info;
	struct crt_perf_context_info *info;
	size_t                        size;
	int                           rc;

	/* Initialize the interface */
	rc = crt_perf_init(argc, argv, false, &perf_info);
	if (rc != 0) {
		DL_ERROR(rc, "crt_perf_init() failed");
		goto error;
	}
	info = &perf_info.context_info[0];

	/* Allocate RPC buffers */
	rc = crt_perf_rpc_buf_init(&perf_info, info);
	if (rc != 0) {
		DL_ERROR(rc, "crt_perf_rpc_buf_init() failed");
		goto error;
	}

	/* Header info */
	if (perf_info.mpi_info.rank == 0)
		crt_perf_print_header_lat(&perf_info, info, BENCHMARK_NAME);

	/* NULL RPC */
	if (perf_info.opts.buf_size_min == 0) {
		rc = crt_perf_run(&perf_info, info, 0, CRT_PERF_LAT_SKIP_SMALL);
		if (rc != 0) {
			DL_ERROR(rc, "crt_perf_run() failed");
			goto error;
		}
	}

	/* RPC with different sizes */
	for (size = MAX(1, perf_info.opts.buf_size_min); size <= perf_info.opts.buf_size_max;
	     size *= 2) {
		rc = crt_perf_run(&perf_info, info, size,
				  (size > CRT_PERF_LARGE_SIZE) ? CRT_PERF_LAT_SKIP_LARGE
							       : CRT_PERF_LAT_SKIP_SMALL);
		if (rc != 0) {
			DL_ERROR(rc, "crt_perf_run() failed");
			goto error;
		}
	}

	/* Finalize interface */
	if (perf_info.mpi_info.rank == 0)
		crt_perf_send_done(&perf_info, info);

	crt_perf_cleanup(&perf_info);

	return EXIT_SUCCESS;

error:
	crt_perf_cleanup(&perf_info);

	return EXIT_FAILURE;
}
