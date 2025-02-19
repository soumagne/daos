/*
 * (C) Copyright 2019-2023 Intel Corporation.
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 */

/**
 * ds_sec: Security Framework Server Internal Declarations
 */

#ifndef __SECURITY_SRV_INTERNAL_H__
#define __SECURITY_SRV_INTERNAL_H__

#include <daos_types.h>
#include <daos_srv/daos_engine.h>
#include "auth.pb-c.h"

#define DAOS_SEC_VERSION 1

extern char *ds_sec_server_socket_path;

/*
 * Pool capabilities bits, derived from user-requested flags and user access
 * permissions.
 */
#define POOL_CAPA_READ		(1U << 0)
#define POOL_CAPA_CREATE_CONT	(1U << 1)
#define POOL_CAPA_DEL_CONT	(1U << 2)

#define POOL_CAPAS_RO_MASK	(POOL_CAPA_READ)
#define POOL_CAPAS_ALL		(POOL_CAPA_READ |			\
				 POOL_CAPA_CREATE_CONT |		\
				 POOL_CAPA_DEL_CONT)

/*
 * Container capabilities bits. Derived from user-requested flags and user's
 * access permissions.
 */
#define CONT_CAPA_READ_DATA	(1U << 0)
#define CONT_CAPA_WRITE_DATA	(1U << 1)
#define CONT_CAPA_GET_PROP	(1U << 2)
#define CONT_CAPA_SET_PROP	(1U << 3)
#define CONT_CAPA_GET_ACL	(1U << 4)
#define CONT_CAPA_SET_ACL	(1U << 5)
#define CONT_CAPA_SET_OWNER	(1U << 6)
#define CONT_CAPA_DELETE	(1U << 7)

#define CONT_CAPAS_RO_MASK	(CONT_CAPA_READ_DATA |			\
				 CONT_CAPA_GET_PROP |			\
				 CONT_CAPA_GET_ACL)
#define CONT_CAPAS_W_MASK	(CONT_CAPA_WRITE_DATA |			\
				 CONT_CAPA_SET_PROP |			\
				 CONT_CAPA_SET_ACL |			\
				 CONT_CAPA_SET_OWNER)
#define CONT_CAPAS_ALL		(CONT_CAPAS_RO_MASK |			\
				 CONT_CAPAS_W_MASK |			\
				 CONT_CAPA_DELETE)

int ds_sec_validate_credentials(d_iov_t *creds, Auth__Token **token);

#endif /* __SECURITY_SRV_INTERNAL_H__ */
