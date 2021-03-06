/*
 * Userland Kstreamer interface
 *
 * Copyright (C) 2006-2007 Daniele Orlandi
 *
 * Authors: Daniele "Vihai" Orlandi <daniele@orlandi.com>
 *
 * This program is free software and may be modified and distributed
 * under the terms and conditions of the GNU General Public License.
 *
 */

#ifndef _LIBKSTREAMER_H
#define _LIBKSTREAMER_H

#include "conn.h"
#include "node.h"
#include "channel.h"
#include "pipeline.h"
#include "feature.h"
#include "router.h"
#include "req.h"
#include "logging.h"
#include "pd_grammar.h"
#include "pd_parser.h"

#define KS_LIB_VERSION_MAJOR 1
#define KS_LIB_VERSION_MINOR 0
#define KS_LIB_VERSION_SERVICE 0

int ks_update_topology(struct ks_conn *conn);

#ifdef _LIBKSTREAMER_PRIVATE_

extern pthread_mutex_t refcnt_lock;

int ks_send_noop(struct ks_conn *conn);

void ks_topology_update(
	struct ks_conn *conn,
	struct nlmsghdr *nlh);

#endif

#endif
