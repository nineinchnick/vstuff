/*
 * vISDN LAPD/q.931 protocol implementation
 *
 * Copyright (C) 2004-2005 Daniele Orlandi
 *
 * Authors: Daniele "Vihai" Orlandi <daniele@orlandi.com>
 *
 * This program is free software and may be modified and distributed
 * under the terms and conditions of the GNU General Public License.
 *
 */

#ifndef _LAPD_MULTI_H
#define _LAPD_MULTI_H

void lapd_datalink_state_init(struct lapd_sock *lapd_sock);

int lapd_dl_establish_request(struct lapd_sock *lapd_sock);
int lapd_dl_release_request(struct lapd_sock *lapd_sock);
void lapd_dl_data_request(struct lapd_sock *lapd_sock, struct sk_buff *skb);
void lapd_dl_unit_data_request(struct lapd_sock *lapd_sock, struct sk_buff *skb);

void lapd_persistent_deactivation(struct lapd_sock *lapd_sock);
void lapd_mdl_assign_request(struct lapd_sock *lapd_sock, int tei);
void lapd_mdl_remove_request(struct lapd_sock *lapd_sock);
void lapd_mdl_error_response(struct lapd_sock *lapd_sock);

int lapd_process_frame(
	struct lapd_sock *lapd_sock,
	struct sk_buff *skb);

#endif