/*
 * (C) Copyright 2023-2024 Intel Corporation.
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#include "perf_test.h"

#include <unistd.h>
#include <getopt.h>

/****************/
/* Local Macros */
/****************/

#define STRING(s)  #s
#define XSTRING(s) STRING(s)
#define VERSION_NAME                                                                               \
	XSTRING(HG_VERSION_MAJOR)                                                                  \
	"." XSTRING(HG_VERSION_MINOR) "." XSTRING(HG_VERSION_PATCH)

#define NDIGITS           2
#define NWIDTH            27

#define CRT_PERF_GROUP_ID "perf_test"

/************************************/
/* Local Type and Struct Definition */
/************************************/

/********************/
/* Local Prototypes */
/********************/

static void
crt_perf_parse_options(int argc, char *argv[], struct crt_perf_opts *opts);

static size_t
crt_perf_parse_size(const char *str);

static void
crt_perf_usage(const char *execname);

static int
crt_perf_context_init(const struct crt_perf_info *perf_info, int context_id,
		      struct crt_perf_context_info *info);

static void
crt_perf_context_cleanup(struct crt_perf_context_info *info);

static void
crt_perf_init_data(void *buf, size_t buf_size);

static int
crt_perf_proc_iovec(crt_proc_t proc, crt_proc_op_t proc_op, struct iovec *iov);

static void
crt_perf_rpc_rate_init_cb(crt_rpc_t *rpc);

static void
crt_perf_rpc_rate_cb(crt_rpc_t *rpc);

static void
		     crt_perf_done_cb(crt_rpc_t *rpc);

/*******************/
/* Local Variables */
/*******************/

static const char   *crt_perf_short_options = "hc:d:p:H:P:l:bC:Z:y:z:w:x:BvMf:";

static struct option crt_perf_long_options[] = {
    {"help", no_argument, NULL, 'h'},
    {"comm", required_argument, NULL, 'c'},
    {"domain", required_argument, NULL, 'd'},
    {"protocol", required_argument, NULL, 'p'},
    {"hostname", required_argument, NULL, 'H'},
    {"port", required_argument, NULL, 'P'},
    {"loop", required_argument, NULL, 'l'},
    {"busy", no_argument, NULL, 'b'},
    {"contexts", required_argument, NULL, 'C'},
    {"msg_size", required_argument, NULL, 'Z'},
    {"buf_size_min", required_argument, NULL, 'y'},
    {"buf_size_max", required_argument, NULL, 'z'},
    {"buf_count", required_argument, NULL, 'w'},
    {"handle", required_argument, NULL, 'x'},
    {"bidirectional", no_argument, NULL, 'B'},
    {"verify", no_argument, NULL, 'v'},
    {"millionbps", no_argument, NULL, 'M'},
    {"hostfile", required_argument, NULL, 'f'},
};

/* TODO keep global until we can retrieve user data from cart context */
static struct crt_perf_info *perf_info_g;

/* clang-format off */
static struct crt_req_format crt_perf_no_arg = {
    .crf_proc_in  = NULL,
	.crf_proc_out = NULL,
	.crf_size_in  = 0,
	.crf_size_out = 0
};

static struct crt_req_format crt_perf_rate = {
    .crf_proc_in  = crt_perf_proc_iovec,
	.crf_proc_out = NULL,
	.crf_size_in  = sizeof(struct iovec),
	.crf_size_out = 0
};

static struct crt_req_format crt_perf_rate_bidir = {
    .crf_proc_in  = crt_perf_proc_iovec,
	.crf_proc_out = crt_perf_proc_iovec,
	.crf_size_in  = sizeof(struct iovec),
	.crf_size_out = sizeof(struct iovec)
};

static struct crt_proto_rpc_format crt_perf_rpcs[] = {
	{
        .prf_req_fmt = &crt_perf_no_arg,
		.prf_hdlr    = crt_perf_rpc_rate_init_cb,
		.prf_co_ops  = NULL,
		.prf_flags   = 0
	},
    {
        .prf_req_fmt = &crt_perf_rate,
		.prf_hdlr    = crt_perf_rpc_rate_cb,
		.prf_co_ops  = NULL,
		.prf_flags   = 0
    },
    {
        .prf_req_fmt = &crt_perf_no_arg,
		.prf_hdlr    = crt_perf_done_cb,
		.prf_co_ops  = NULL,
		.prf_flags   = 0
    }
};

static struct crt_proto_format crt_perf_protocol = {
    .cpf_name  = "crt_perf_protocol",
	.cpf_ver   = CRT_PERF_RPC_VERSION,
	.cpf_count = ARRAY_SIZE(crt_perf_rpcs),
	.cpf_prf   = &crt_perf_rpcs[0],
	.cpf_base  = CRT_PERF_BASE_OPC
};
/* clang-format on */

static void
crt_perf_parse_options(int argc, char *argv[], struct crt_perf_opts *opts)
{
	int opt;

	while ((opt = getopt_long(argc, argv, crt_perf_short_options, crt_perf_long_options,
				  NULL)) != -1) {
		switch (opt) {
		case 'c': /* comm */
			opts->comm = strdup(optarg);
			break;

		case 'd': /* domain */
			opts->domain = strdup(optarg);
			break;

		case 'p': /* protocol */
			opts->protocol = strdup(optarg);
			break;

		case 'H': /* hostname */
			opts->hostname = strdup(optarg);
			break;

		case 'P': /* port */
			opts->port = atoi(optarg);
			break;

		case 'l': /* loop */
			opts->loop = atoi(optarg);
			break;

		case 'b': /* busy wait */
			opts->busy_wait = true;
			break;

		case 'C': /* context max */
			opts->context_max = (size_t)atoi(optarg);
			break;

		case 'Z': /* msg size */
			opts->msg_size_max = crt_perf_parse_size(optarg);
			break;

		case 'y': /* min buffer size */
			opts->buf_size_min = crt_perf_parse_size(optarg);
			break;

		case 'z': /* max buffer size */
			opts->buf_size_max = crt_perf_parse_size(optarg);
			break;

		case 'w': /* buffer count */
			opts->buf_count = (size_t)atol(optarg);
			break;

		case 'x': /* request max */
			opts->request_max = (size_t)atoi(optarg);
			break;

		case 'B': /* bidirectional */
			opts->bidir = true;
			break;

		case 'v': /* verify */
			opts->verify = true;
			break;

		case 'M': /* OSU-style output MB/s */
			opts->mbps = true;
			break;

		case 'f': /* hostfile */
			opts->attach_path = strdup(optarg);
			break;

		case 'h':
		default:
			crt_perf_usage(argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	if ((argc - optind) > 1) {
		crt_perf_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
}

static size_t
crt_perf_parse_size(const char *str)
{
	size_t size;
	char   prefix;

	if (sscanf(str, "%zu%c", &size, &prefix) == 2) {
		switch (prefix) {
		case 'k':
			size *= 1024;
			break;
		case 'm':
			size *= (1024 * 1024);
			break;
		case 'g':
			size *= (1024 * 1024 * 1024);
			break;
		default:
			break;
		}
		return size;
	} else if (sscanf(str, "%zu", &size) == 1)
		return size;
	else
		return 0;
}

static void
crt_perf_usage(const char *execname)
{
	printf("usage: %s [OPTIONS] [<class+protocol>]\n", execname);
	printf("    OPTIONS\n");
	printf("    -h, --help           Print a usage message and exit\n");
	printf("    -c, --comm           Select NA plugin\n"
	       "                         NA plugins: ofi, ucx, etc\n");
	printf("    -d, --domain         Select NA OFI domain\n");
	printf("    -p, --protocol       Select plugin protocol\n"
	       "                         Available protocols: tcp, verbs, etc\n");
	printf("    -H, --hostname       Select hostname / IP address to use\n"
	       "                         Default: any\n");
	printf("    -P, --port           Select port to use\n"
	       "                         Default: any\n");
	printf("    -l, --loop           Number of loops (default: 1)\n");
	printf("    -b, --busy           Busy wait\n");
	printf("    -y  --buf_size_min   Min buffer size (in bytes)\n");
	printf("    -z, --buf_size_max   Max buffer size (in bytes)\n");
	printf("    -w  --buf_count      Number of buffers used\n");
	printf("    -M, --mbps           Output in MB/s instead of MiB/s\n");
	printf("    -f, --hostfile       Specify attach info path\n");
	printf("    -V, --verbose        Print verbose output\n");
	printf("    -x, --handle        Max number of handles\n");
	printf("    -B, --bidirectional Bidirectional communication\n");
}

static int
crt_perf_context_init(const struct crt_perf_info *perf_info, int context_id,
		      struct crt_perf_context_info *info)
{
	int ctx_idx;
	int rc;

	rc = crt_context_create(&info->context);
	if (rc != 0) {
		DL_ERROR(rc, "crt_context_create() failed");
		goto error;
	}

	rc = crt_context_idx(info->context, &ctx_idx);
	if (rc != 0) {
		DL_ERROR(rc, "crt_context_idx() failed");
		goto error;
	}
	if (context_id != ctx_idx) {
		rc = -DER_MISMATCH;
		DL_ERROR(rc, "context_id %d != ctx_idx %d", context_id, ctx_idx);
		goto error;
	}
	info->context_id = context_id;

	info->requests = calloc(perf_info->opts.request_max, sizeof(*info->requests));
	if (info->requests == NULL) {
		rc = -DER_NOMEM;
		DL_ERROR(rc, "Could not allocate request array of size %zu",
			 perf_info->opts.request_max);
		goto error;
	}

	return 0;

error:
	crt_perf_context_cleanup(info);
	return rc;
}

static void
crt_perf_context_cleanup(struct crt_perf_context_info *info)
{
	if (info->context != NULL) {
		(void)crt_context_destroy(info->context, 1);
		info->context = NULL;
	}
	if (info->requests != NULL) {
		free(info->requests);
		info->requests = NULL;
	}
	if (info->rpc_buf != NULL) {
		free(info->rpc_buf);
		info->rpc_buf = NULL;
	}
	if (info->rpc_verify_buf != NULL) {
		free(info->rpc_verify_buf);
		info->rpc_verify_buf = NULL;
	}
}

static void
crt_perf_init_data(void *buf, size_t buf_size)
{
	char  *buf_ptr = (char *)buf;
	size_t i;

	for (i = 0; i < buf_size; i++)
		buf_ptr[i] = (char)i;
}

static int
crt_proc_iovec(crt_proc_t proc, crt_proc_op_t proc_op, struct iovec *iov)
{
	uint32_t len = (uint32_t)iov->iov_len;
	int      rc;

	if (FREEING(proc_op)) {
		iov->iov_base = NULL;
		iov->iov_len  = 0;
		return 0;
	}

	rc = crt_proc_uint32_t(proc, proc_op, &len);
	if (rc != 0) {
		DL_ERROR(rc, "crt_proc_uint32_t() failed");
		goto error;
	}

	if (len == 0)
		return 0;

	if (DECODING(proc_op)) {
		iov->iov_len = (size_t)len;
		/**
		 * Don't allocate/memcpy like we do for others.
		 * Just point at memory in request buffer instead.
		 */
		iov->iov_base = hg_proc_save_ptr(proc, iov->iov_len);
		if (iov->iov_base == NULL) {
			rc = -DER_INVAL;
			DL_ERROR(rc, "hg_proc_save_ptr() failed");
			goto error;
		}
	} else { /* ENCODING(proc_op) */
		rc = crt_proc_memcpy(proc, proc_op, iov->iov_base, iov->iov_len);
		if (rc != 0) {
			DL_ERROR(rc, "crt_proc_memcpy() failed");
			goto error;
		}
	}

	return 0;

error:
	return rc;
}

static void
crt_perf_rpc_rate_init_cb(crt_rpc_t *rpc)
{
	size_t                        page_size = sysconf(_SC_PAGE_SIZE);
	const struct crt_perf_opts   *opts;
	struct crt_perf_context_info *info;
	int                           ctx_idx;
	int                           rc;

	rc = crt_context_idx(rpc->cr_ctx, &ctx_idx);
	if (rc != 0) {
		DL_ERROR(rc, "crt_context_idx() failed");
		goto error;
	}
	info = (struct crt_perf_context_info *)&perf_info_g->context_info[ctx_idx];
	opts = &perf_info_g->opts;

	/* Prepare buf */
	info->rpc_buf = aligned_alloc(page_size, opts->buf_size_max);
	if (info->rpc_buf == NULL) {
		rc = -DER_NOMEM;
		DL_ERROR(rc, "aligned_alloc(%zu, %zu) failed", page_size, opts->buf_size_max);
		goto error;
	}

	if (opts->verify) {
		info->rpc_verify_buf = aligned_alloc(page_size, opts->buf_size_max);
		if (info->rpc_verify_buf == NULL) {
			rc = -DER_NOMEM;
			DL_ERROR(rc, "aligned_alloc(%zu, %zu) failed", page_size,
				 opts->buf_size_max);
			goto error;
		}
	}

	/* Send response back */
	rc = crt_reply_send(rpc);
	if (rc != 0) {
		DL_ERROR(rc, "crt_reply_send() failed");
		goto error;
	}

	return;

error:
	if (info->rpc_buf != NULL) {
		free(info->rpc_buf);
		info->rpc_buf = NULL;
	}
	if (info->rpc_verify_buf != NULL) {
		free(info->rpc_verify_buf);
		info->rpc_verify_buf = NULL;
	}

	return;
}

static void
crt_perf_rpc_rate_cb(crt_rpc_t *rpc)
{
	const struct crt_perf_opts   *opts;
	struct crt_perf_context_info *info;
	struct iovec                 *in_iov;
	int                           ctx_idx;
	int                           rc;

	rc = crt_context_idx(rpc->cr_ctx, &ctx_idx);
	if (rc != 0) {
		DL_ERROR(rc, "crt_context_idx() failed");
		goto error;
	}
	info = (struct crt_perf_context_info *)&perf_info_g->context_info[ctx_idx];
	opts = &perf_info_g->opts;

	/* Get input struct */
	in_iov = crt_req_get(rpc);
	if (in_iov == NULL) {
		rc = -DER_INVAL;
		DL_ERROR(rc, "crt_req_get() failed");
		goto error;
	}

	if (opts->verify) {
		rc = crt_perf_verify_data(in_iov->iov_base, in_iov->iov_len);
		if (rc != 0) {
			DL_ERROR(rc, "crt_perf_verify_data() failed");
			goto error;
		}
	}

	/* Send response back */
	if (opts->bidir) {
		struct iovec *out_iov = (struct iovec *)crt_reply_get(rpc);
		if (out_iov == NULL) {
			rc = -DER_INVAL;
			DL_ERROR(rc, "crt_reply_get() failed");
			goto error;
		}
		out_iov->iov_base = in_iov->iov_base;
		out_iov->iov_len  = in_iov->iov_len;
	}

	rc = crt_reply_send(rpc);
	if (rc != 0) {
		DL_ERROR(rc, "crt_reply_send() failed");
		goto error;
	}

	return;

error:
	return;
}

static void
crt_perf_done_cb(crt_rpc_t *rpc)
{
	const struct crt_perf_opts   *opts;
	struct crt_perf_context_info *info;
	int                           ctx_idx;
	int                           rc;

	rc = crt_context_idx(rpc->cr_ctx, &ctx_idx);
	if (rc != 0) {
		DL_ERROR(rc, "crt_context_idx() failed");
		goto error;
	}
	info = (struct crt_perf_context_info *)&perf_info_g->context_info[ctx_idx];
	opts = &perf_info_g->opts;

	/* Set done for context data */
	info->done = true;

	/* Send response back */
	rc = crt_reply_send(rpc);
	if (rc != 0) {
		DL_ERROR(rc, "crt_reply_send() failed");
		goto error;
	}

	return;

error:
	return;
}

int
crt_perf_init(int argc, char *argv[], bool listen, struct crt_perf_info *info)
{
	struct crt_init_options crt_init_options;
	const char             *attach_info_path = NULL;
	uint32_t                crt_init_flags   = 0;
	size_t                  i;
	int                     rc;

	rc = d_log_init();
	if (rc != 0) {
		DL_ERROR(rc, "d_log_init() failed");
		goto error;
	}

	// TODO parse options
	crt_perf_parse_options(argc, argv, &info->opts);
	// crt_init_options.cio_provider = ;
	// crt_init_options.cio_domain   = ;
	// crt_init_options.cio_port     = ;
	// crt_init_options.cio_max_expected_size = ;

	if (listen)
		crt_init_flags |= CRT_FLAG_BIT_SERVER | CRT_FLAG_BIT_AUTO_SWIM_DISABLE;
	rc = crt_init_opt(CRT_PERF_GROUP_ID, crt_init_flags, &crt_init_options);
	if (rc != 0) {
		DL_ERROR(rc, "crt_init() failed");
		goto error;
	}

	if (attach_info_path) {
		rc = crt_group_config_path_set(attach_info_path);
		if (rc != 0) {
			DL_ERROR(rc, "crt_group_config_path_set() failed");
			goto error;
		}
	}

	if (listen) {
		rc = crt_rank_self_set(info->mpi_info.rank, 1);
		if (rc != 0) {
			DL_ERROR(rc, "crt_rank_self_set() failed");
			goto error;
		}
	}

	if (info->opts.bidir) {
		crt_perf_rpcs[1].prf_req_fmt = &crt_perf_rate_bidir;
	}

	rc = crt_proto_register(&crt_perf_protocol);
	if (rc != 0) {
		DL_ERROR(rc, "crt_proto_register() failed");
		goto error;
	}

	info->context_info = (struct crt_perf_context_info *)calloc(info->opts.context_max,
								    sizeof(*info->context_info));
	if (info->context_info == NULL) {
		rc = -DER_NOMEM;
		DL_ERROR(rc, "Could not allocate context info");
		goto error;
	}
	for (i = 0; i < info->opts.context_max; i++) {
		rc = crt_perf_context_init(info, i, &info->context_info[i]);
		if (rc != 0) {
			DL_ERROR(rc, "crt_perf_context_init() failed");
			goto error;
		}
	}

	if (listen) {
		rc = crt_group_config_save(NULL, true);
		if (rc != 0) {
			D_ERROR("crt_group_config_save() failed", rc);
			goto error;
		}
	} else {
		rc = crt_group_attach(CRT_PERF_GROUP_ID, &info->ep_group);
		if (rc != 0) {
			D_ERROR("crt_group_attach() failed", rc);
			goto error;
		}
	}

	perf_info_g = info;

	return 0;

error:
	return rc;
}

void
crt_perf_cleanup(struct crt_perf_info *info)
{
	size_t i;

	if (info->ep_group != NULL) {
		(void)crt_group_detach(info->ep_group);
		info->ep_group = NULL;
	}

	if (info->context_info != NULL) {
		for (i = 0; i < info->opts.context_max; i++)
			crt_perf_context_cleanup(&info->context_info[i]);
		free(info->context_info);
		info->context_info = NULL;
	}

	(void)crt_finalize();

	d_log_fini();

	perf_info_g = NULL;
}

int
crt_perf_rpc_buf_init(const struct crt_perf_info *perf_info, struct crt_perf_context_info *info)
{
	const struct crt_perf_opts *opts      = &perf_info->opts;
	size_t                      page_size = sysconf(_SC_PAGE_SIZE);
	bool                        barrier   = false;
	int                         rc;

	/* Prepare buf */
	info->rpc_buf = aligned_alloc(page_size, opts->buf_size_max);
	if (info->rpc_buf == NULL) {
		rc = -DER_NOMEM;
		DL_ERROR(rc, "aligned_alloc(%zu, %zu) failed", page_size, opts->buf_size_max);
		goto error;
	}

	/* Init data */
	crt_perf_init_data(info->rpc_buf, opts->buf_size_max);

	if (opts->verify) {
		info->rpc_verify_buf = aligned_alloc(page_size, opts->buf_size_max);
		if (info->rpc_verify_buf == NULL) {
			rc = -DER_NOMEM;
			DL_ERROR(rc, "aligned_alloc(%zu, %zu) failed", page_size,
				 opts->buf_size_max);
			goto error;
		}
	}

	barrier = true;

	if (perf_info->mpi_info.rank == 0) {
		size_t ep_rank, ep_tag;

		for (ep_rank = 0; ep_rank < perf_info->ep_rank_max; ep_rank++) {
			for (ep_tag = 0; ep_tag < perf_info->ep_tag_max; ep_tag++) {
				struct crt_perf_request args      = {.expected_count = 1,
								     .complete_count = 0,
								     .rc             = 0,
								     .done           = false,
								     .cb             = NULL};
				crt_endpoint_t          target_ep = {.ep_grp  = perf_info->ep_group,
								     .ep_rank = ep_rank,
								     .ep_tag  = ep_tag};
				crt_rpc_t              *request;

				rc = crt_req_create(info->context, &target_ep,
						    CRT_PERF_RATE_INIT_ID, &request);
				if (rc != 0) {
					DL_ERROR(rc, "crt_req_create() failed");
					goto error;
				}

				rc = crt_req_send(request, crt_perf_request_complete, &args);
				if (rc != 0) {
					DL_ERROR(rc, "crt_req_send() failed");
					goto error;
				}

				while (!args.done) {
					rc = crt_progress(info->context, 1000 * 1000);
					if (rc != 0 && rc != -DER_TIMEDOUT) {
						DL_ERROR(rc, "crt_progress() failed");
						goto error;
					}
				}
			}
		}
	}
	crt_perf_barrier(perf_info);

	return 0;

error:
	if (info->rpc_buf != NULL) {
		free(info->rpc_buf);
		info->rpc_buf = NULL;
	}
	if (info->rpc_verify_buf != NULL) {
		free(info->rpc_verify_buf);
		info->rpc_verify_buf = NULL;
	}
	if (barrier)
		crt_perf_barrier(perf_info);

	return rc;
}

void
crt_perf_print_header_lat(const struct crt_perf_info         *perf_info,
			  const struct crt_perf_context_info *info, const char *benchmark)
{
	const struct crt_perf_opts *opts = &perf_info->opts;

	printf("# %s v%s\n", benchmark, VERSION_NAME);
	printf("# %d client process(es)\n", perf_info->mpi_info.size);
	printf("# Loop %d times from size %zu to %zu byte(s) with %zu handle(s) "
	       "in-flight\n",
	       opts->loop, opts->buf_size_min, opts->buf_size_max, opts->request_max);
	if (opts->request_max * (size_t)perf_info->mpi_info.size <
	    perf_info->ep_rank_max * perf_info->ep_tag_max)
		printf("# WARNING number of handles in flight less than number of "
		       "targets\n");
	if (opts->verify)
		printf("# WARNING verifying data, output will be slower\n");
	printf("%-*s%*s%*s\n", 10, "# Size", NWIDTH, "Avg time (us)", NWIDTH, "Avg rate (RPC/s)");
	fflush(stdout);
}

void
crt_perf_print_lat(const struct crt_perf_info *perf_info, const struct crt_perf_context_info *info,
		   size_t buf_size, struct timespec t)
{
	const struct crt_perf_opts *opts = &perf_info->opts;
	double                      rpc_time;
	size_t                      loop = (size_t)opts->loop, request_max = opts->request_max,
	       dir           = (size_t)(opts->bidir ? 2 : 1),
	       mpi_comm_size = (size_t)perf_info->mpi_info.size;

	rpc_time = d_time2s(t) * 1e6 / (double)(loop * request_max * dir * mpi_comm_size);

	printf("%-*zu%*.*f%*lu\n", 10, buf_size, NWIDTH, NDIGITS, rpc_time, NWIDTH,
	       (long unsigned int)(1e6 / rpc_time));
}

int
crt_perf_verify_data(const void *buf, size_t buf_size)
{
	const char *buf_ptr = (const char *)buf;
	size_t      i;
	int         rc;

	for (i = 0; i < buf_size; i++) {
		if (buf_ptr[i] != (char)i) {
			rc = -DER_INVAL;
			DL_ERROR(rc,
				 "Error detected in bulk transfer, buf[%zu] = %d, "
				 "was expecting %d!",
				 i, buf_ptr[i], (char)i);
			goto error;
		}
	}

	return 0;

error:
	return rc;
}

void
crt_perf_request_complete(const struct crt_cb_info *cb_info)
{
	struct crt_perf_request *info = (struct crt_perf_request *)cb_info->cci_arg;

	if (cb_info->cci_rc != 0) {
		DL_ERROR(cb_info->cci_rc, "crt_perf_request_complete() failed");
		info->rc = cb_info->cci_rc;
	}
	if (info->cb) {
		info->rc = info->cb(cb_info->cci_rpc);
	}

	if ((++info->complete_count) == info->expected_count)
		info->done = true;
}

int
crt_perf_send_done(const struct crt_perf_info *perf_info, struct crt_perf_context_info *info)
{
	size_t ep_rank, ep_tag;
	int    rc;

	for (ep_rank = 0; ep_rank < perf_info->ep_rank_max; ep_rank++) {
		for (ep_tag = 0; ep_tag < perf_info->ep_tag_max; ep_tag++) {
			struct crt_perf_request args      = {.expected_count = 1,
							     .complete_count = 0,
							     .rc             = 0,
							     .done           = false,
							     .cb             = NULL};
			crt_endpoint_t          target_ep = {
				     .ep_grp = perf_info->ep_group, .ep_rank = ep_rank, .ep_tag = ep_tag};
			crt_rpc_t *request;

			rc = crt_req_create(info->context, &target_ep, CRT_PERF_DONE_ID, &request);
			if (rc != 0) {
				DL_ERROR(rc, "crt_req_create() failed");
				goto error;
			}

			rc = crt_req_send(request, crt_perf_request_complete, &args);
			if (rc != 0) {
				DL_ERROR(rc, "crt_req_send() failed");
				goto error;
			}

			while (!args.done) {
				rc = crt_progress(info->context, 1000 * 1000);
				if (rc != 0 && rc != -DER_TIMEDOUT) {
					DL_ERROR(rc, "crt_progress() failed");
					goto error;
				}
			}
		}
	}

	return 0;

error:
	return rc;
}

void
crt_perf_barrier(const struct crt_perf_info *perf_info)
{
}