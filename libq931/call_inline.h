/*
 * vISDN DSSS-1/q.931 signalling library
 *
 * Copyright (C) 2004-2005 Daniele Orlandi
 *
 * Authors: Daniele "Vihai" Orlandi <daniele@orlandi.com>
 *
 * This program is free software and may be modified and distributed
 * under the terms and conditions of the GNU Lesser General Public License.
 *
 */

#ifndef _LIBQ931_CALL_INLINE_H
#define _LIBQ931_CALL_INLINE_H

#include <libq931/call.h>
#include <libq931/ie.h>
#include <libq931/message.h>
#include <libq931/output.h>
#include <libq931/intf.h>


#ifdef Q931_PRIVATE

static inline int q931_call_send_alerting(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_ALERTING, ies);
}

static inline int q931_call_send_call_proceeding(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_CALL_PROCEEDING, ies);
}

static inline int q931_call_send_connect(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_CONNECT, ies);
}

static inline int q931_call_send_connect_acknowledge(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_CONNECT_ACKNOWLEDGE, ies);
}

static inline int q931_call_send_disconnect(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_DISCONNECT, ies);
}

static inline int q931_call_send_information(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_INFORMATION, ies);
}

static inline int q931_call_send_notify(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_NOTIFY, ies);
}

static inline int q931_call_send_progress(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_PROGRESS, ies);
}

static inline int q931_call_send_release(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_RELEASE, ies);
}

static inline int q931_call_send_release_complete(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_RELEASE_COMPLETE, ies);
}

static inline int q931_call_send_resume(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_RESUME, ies);
}

static inline int q931_call_send_resume_acknowledge(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_RESUME_ACKNOWLEDGE, ies);
}

static inline int q931_call_send_resume_reject(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_RESUME_REJECT, ies);
}

static inline int q931_call_send_setup(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_SETUP, ies);
}

static inline int q931_call_send_setup_bc(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message_bc(call, &call->intf->bc_dlc, Q931_MT_SETUP, ies);
}

static inline int q931_call_send_setup_acknowledge(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_SETUP_ACKNOWLEDGE, ies);
}

static inline int q931_call_send_status(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_STATUS, ies);
}

static inline int q931_call_send_status_enquiry(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_STATUS_ENQUIRY, ies);
}

static inline int q931_call_send_suspend(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_SUSPEND, ies);
}

static inline int q931_call_send_suspend_acknowledge(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_SUSPEND_ACKNOWLEDGE, ies);
}

static inline int q931_call_send_suspend_reject(
	struct q931_call *call,
	const struct q931_ies *ies)
{
	return q931_call_send_message(call, call->dlc, Q931_MT_SUSPEND_REJECT, ies);
}

#endif
#endif
