/*
 * (C) Copyright 2017-2024 Intel Corporation.
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include "crt_perf.h"

#include <sched.h>

/********************/
/* Local Prototypes */
/********************/

static int
crt_perf_loop_thread_set_affinity(struct crt_perf_context_info *info);

static int
crt_perf_loop(struct crt_perf_context_info *info);

static void *
crt_perf_loop_thread(void *arg)
{
	int rc;

	(void)crt_perf_loop_thread_set_affinity((struct crt_perf_context_info *)arg);

	rc = crt_perf_loop((struct crt_perf_context_info *)arg);
	if (rc != 0) {
		DL_ERROR(rc, "crt_perf_loop() failed");
		goto out;
	}

out:
	return NULL;
}

static int
crt_perf_loop_thread_set_affinity(struct crt_perf_context_info *info)
{
	cpu_set_t orig_cpu_set, new_cpu_set;
	size_t    cpu, i = 0;
	int       rc;

	/* Retrive affinity set on main process */
	CPU_ZERO(&orig_cpu_set);
	rc = pthread_getaffinity_np(pthread_self(), sizeof(orig_cpu_set), &orig_cpu_set);
	if (rc != 0) {
		DS_ERROR(rc, "pthread_getaffinity_np() failed");
		goto error;
	}

	if (info->context_id > CPU_COUNT(&orig_cpu_set)) {
		rc = -DER_INVAL;
		DL_ERROR(rc, "Could not set affinity, class ID (%d) > CPU count (%d)",
			 info->context_id, CPU_COUNT(&orig_cpu_set));
		goto error;
	}

	CPU_ZERO(&new_cpu_set);
	for (cpu = 0; cpu < CPU_SETSIZE; cpu++) {
		if (CPU_ISSET(cpu, &orig_cpu_set)) {
			if (i == (size_t)info->context_id) {
				CPU_SET(cpu, &new_cpu_set);
				break;
			}
			i++;
		}
	}

	rc = pthread_setaffinity_np(pthread_self(), sizeof(new_cpu_set), &new_cpu_set);
	if (rc != 0) {
		DS_ERROR(rc, "pthread_setaffinity_np() failed");
		goto error;
	}

	CPU_ZERO(&orig_cpu_set);
	rc = pthread_getaffinity_np(pthread_self(), sizeof(orig_cpu_set), &orig_cpu_set);
	if (rc != 0) {
		DS_ERROR(rc, "pthread_getaffinity_np() failed");
		goto error;
	}
	for (cpu = 0; cpu < CPU_SETSIZE; cpu++)
		if (CPU_ISSET(cpu, &orig_cpu_set))
			D_INFO("Class ID %d bound to CPU %zu\n", info->context_id, cpu);

	return 0;

error:
	return rc;
}

static int
crt_perf_loop(struct crt_perf_context_info *info)
{
	int rc;

	do {
		rc = crt_progress(info->context, 1000 * 1000);
		if (rc != 0 && rc != -DER_TIMEDOUT) {
			DL_ERROR(rc, "crt_progress() failed");
			goto error;
		}
	} while (!info->done);

	return 0;

error:
	return rc;
}

int
main(int argc, char **argv)
{
	struct crt_perf_info info;
	pthread_t           *progress_threads = NULL;
	int                  rc;

	rc = crt_perf_init(argc, argv, true, &info);
	if (rc != 0) {
		DL_ERROR(rc, "crt_perf_init() failed");
		goto error;
	}

	if (info.mpi_info.rank == 0) {
		printf("# %d server process(es)\n", info.mpi_info.size);
		printf("# Waiting for client...\n");
	}

	if (info.opts.context_max > 1) {
		size_t i;

		progress_threads =
		    (pthread_t *)malloc(sizeof(*progress_threads) * info.opts.context_max);
		if (progress_threads == NULL) {
			DL_ERROR(-DER_NOMEM, "Could not allocate progress threads");
			goto error;
		}

		for (i = 0; i < info.opts.context_max; i++) {
			rc = pthread_create(&progress_threads[i], NULL, crt_perf_loop_thread,
					    &info.context_info[i]);
			if (rc != 0) {
				DS_ERROR(rc, "pthread_create() failed");
				goto error;
			}
		}

		for (i = 0; i < info.opts.context_max; i++) {
			rc = pthread_join(progress_threads[i], NULL);
			if (rc != 0) {
				DS_ERROR(rc, "pthread_join() failed");
				goto error;
			}
		}
	} else {
		rc = crt_perf_loop(&info.context_info[0]);
		if (rc != 0) {
			DL_ERROR(rc, "crt_perf_loop() failed");
			goto error;
		}
	}

	if (info.mpi_info.rank == 0)
		printf("Finalizing...\n");
	crt_perf_cleanup(&info);
	free(progress_threads);

	return EXIT_SUCCESS;

error:
	crt_perf_cleanup(&info);
	free(progress_threads);

	return EXIT_FAILURE;
}
