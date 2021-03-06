/*
 * kstreamer's controlling program
 *
 * Copyright (C) 2005-2007 Daniele Orlandi
 *
 * Authors: Daniele "Vihai" Orlandi <daniele@orlandi.com>
 *
 * This program is free software and may be modified and distributed
 * under the terms and conditions of the GNU General Public License.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <linux/types.h>
#include <assert.h>
#include <limits.h>
#include <libgen.h>

#include <getopt.h>

#include <linux/kstreamer/hdlc_framer.h>
#include <linux/kstreamer/octet_reverser.h>

#include <list.h>

#include <libskb.h>
#include <libkstreamer/libkstreamer.h>

#include "kstool.h"
#include "connect.h"
#include "disconnect.h"
#include "pipeline_start.h"
#include "pipeline_stop.h"
#include "pipeline_open.h"
#include "pipeline_close.h"
#include "monitor.h"
#include "dump.h"

struct global_state glob;

void print_usage(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);

	fprintf(stderr,
		"%s: [options] <command> [parameters]\n"
		"\n"
		"Options:"
		"    -v --verbose: Increase verbosity level\n"
		"\n"
		"Commands:\n\n",
		glob.argv[0]);

	struct module *module;
	list_for_each_entry(module, &glob.modules, node) {
		printf("\n");

		if (module->usage)
			module->usage();
	}

	if (fmt) {
		fprintf(stderr, "\n\n=========> ");
		vfprintf(stderr, fmt, ap);
	}

	exit(1);
}

static void ks_report_func(int level, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);

	switch(level) {
//	case KS_LOG_DEBUG:
//        break;

	default:
		vfprintf(stderr, format, ap);
	break;
	}

	va_end(ap);
}

int main(int argc, char *argv[])
{
	const char *command = NULL;
	int c;
	int optidx;

	memset(&glob, 0, sizeof(glob));
	glob.argc = argc;
	glob.argv = argv;
	INIT_LIST_HEAD(&glob.modules);

	setvbuf(stdout, (char *)NULL, _IONBF, 0);
	setvbuf(stderr, (char *)NULL, _IONBF, 0);

	list_add_tail(&module_connect.node, &glob.modules);
	list_add_tail(&module_disconnect.node, &glob.modules);
	list_add_tail(&module_pipeline_open.node, &glob.modules);
	list_add_tail(&module_pipeline_close.node, &glob.modules);
	list_add_tail(&module_pipeline_start.node, &glob.modules);
	list_add_tail(&module_pipeline_stop.node, &glob.modules);
	list_add_tail(&module_monitor.node, &glob.modules);
	list_add_tail(&module_dump.node, &glob.modules);

	glob.conn = ks_conn_create();
	if (!glob.conn) {
		fprintf(stderr, "Cannot initialize kstreamer library\n");
		return 1;
	}

	struct option options[] = {
		{ "verbose", no_argument, 0, 0 },
		{ "debug-state", no_argument, 0, 0 },
		{ "debug-netlink", no_argument, 0, 0 },
		{ "debug-router", no_argument, 0, 0 },
		{ }
	};

	for(;;) {
		struct option no_opt ={ "", no_argument, 0, 0 };
		struct option *opt;

		c = getopt_long(argc, argv, "v", options, &optidx);

		if (c == -1)
			break;

		opt = c ? &no_opt : &options[optidx];

		if (c == 'v' || !strcmp(opt->name, "verbose"))
			glob.verbosity++;
		else if (!strcmp(opt->name, "debug-state"))
			glob.conn->debug_state = TRUE;
		else if (!strcmp(opt->name, "debug-netlink"))
			glob.conn->debug_netlink = TRUE;
		else if (!strcmp(opt->name, "debug-router"))
			glob.conn->debug_router = TRUE;
		else {
			if (c) {
				print_usage("Unknow option '%c'\n", c);
			} else {
				print_usage("Unknow option %s\n",
					options[optidx].name);
			}
		}
	}

	if (argc <= optind) {
		print_usage("Missing required command\n");
	}

	command = argv[optind];

	glob.conn->report_func = ks_report_func;

	int err;
	err = ks_conn_establish(glob.conn);
	if (err < 0) {
		fprintf(stderr, "Cannot connect kstreamer library\n");
		goto err_conn_establish;
	}

	ks_update_topology(glob.conn);

	glob.hdlc_framer =
		ks_feature_get_by_name(glob.conn, "hdlc_framer");
	glob.hdlc_deframer =
		ks_feature_get_by_name(glob.conn, "hdlc_deframer");
	glob.octet_reverser =
		ks_feature_get_by_name(glob.conn, "octet_reverser");

	int ret;
	struct module *module;
	list_for_each_entry(module, &glob.modules, node) {
		if (!strcasecmp(command, module->cmd) &&
		    module->do_it) {
			ret = module->do_it(optind);
			goto found;
		}
	}

	print_usage("Unknown command '%s'\n", command);
	goto err_command_not_found;

found:

	if (glob.hdlc_framer)
		ks_feature_put(glob.hdlc_framer);

	if (glob.hdlc_deframer)
		ks_feature_put(glob.hdlc_deframer);

	if (glob.octet_reverser)
		ks_feature_put(glob.octet_reverser);

	ks_conn_destroy(glob.conn);

	return ret;

err_command_not_found:
	ks_conn_destroy(glob.conn);
err_conn_establish:

	return err;
}
