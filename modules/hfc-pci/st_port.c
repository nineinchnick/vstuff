/*
 * Cologne Chip's HFC-S PCI A vISDN driver
 *
 * Copyright (C) 2004-2005 Daniele Orlandi
 *
 * Authors: Daniele "Vihai" Orlandi <daniele@orlandi.com>
 *
 * This program is free software and may be modified and distributed
 * under the terms and conditions of the GNU General Public License.
 *
 */

#include <linux/kernel.h>

#include "st_port.h"
#include "card.h"
#include "card_inline.h"

//----------------------------------------------------------------------------

static ssize_t hfc_show_role(
	struct visdn_port *visdn_port,
	struct visdn_port_attribute *attr,
	char *buf)
{
	struct hfc_st_port *port = to_st_port(visdn_port);

	return snprintf(buf, PAGE_SIZE, "%s\n",
		port->nt_mode ? "NT" : "TE");
}

static ssize_t hfc_store_role(
	struct visdn_port *visdn_port,
	struct visdn_port_attribute *attr,
	const char *buf,
	size_t count)
{
	struct hfc_st_port *port = to_st_port(visdn_port);
	struct hfc_card *card = port->card;

	if (count < 2)
		return count;

	hfc_card_lock(card);

	if (!strncmp(buf, "NT", 2) && !port->nt_mode) {
		port->nt_mode = TRUE;
		port->clock_delay = HFC_DEF_NT_CLK_DLY;
		port->sampling_comp = HFC_DEF_NT_SAMPL_COMP;

		hfc_st_port_update_sctrl(port);
		hfc_st_port_update_st_clk_dly(port);

	} else if (!strncmp(buf, "TE", 2) && port->nt_mode) {
		port->nt_mode = FALSE;
		port->clock_delay = HFC_DEF_TE_CLK_DLY;
		port->sampling_comp = HFC_DEF_TE_SAMPL_COMP;

		hfc_st_port_update_sctrl(port);
		hfc_st_port_update_st_clk_dly(port);
	}

	hfc_card_unlock(card);

	hfc_debug_port(port, 1,
		"role set to %s\n",
		port->nt_mode ? "NT" : "TE");

	return count;
}

static VISDN_PORT_ATTR(role, S_IRUGO | S_IWUSR,
		hfc_show_role,
		hfc_store_role);

//----------------------------------------------------------------------------

static ssize_t hfc_show_l1_state(
	struct visdn_port *visdn_port,
	struct visdn_port_attribute *attr,
	char *buf)
{
	struct hfc_st_port *port = to_st_port(visdn_port);

	u8 l1_state = hfc_inb(port->card, hfc_STATES) & hfc_STATES_STATE_MASK;

	return snprintf(buf, PAGE_SIZE, "%c%d\n",
		port->nt_mode ? 'G' : 'F',
		l1_state);
}

static ssize_t hfc_store_l1_state(
	struct visdn_port *visdn_port,
	struct visdn_port_attribute *attr,
	const char *buf,
	size_t count)
{
	struct hfc_st_port *port = to_st_port(visdn_port);
	struct hfc_card *card = port->card;
	int err;

	hfc_card_lock(card);

	if (count >= 8 && !strncmp(buf, "activate", 8)) {
		hfc_outb(card, hfc_STATES,
			hfc_STATES_ACTIVATE|
			hfc_STATES_NT_G2_G3);
	} else if (count >= 10 && !strncmp(buf, "deactivate", 10)) {
		hfc_outb(card, hfc_STATES,
			hfc_STATES_DEACTIVATE);
	} else {
		int state;
		if (sscanf(buf, "%d", &state) < 1) {
			err = -EINVAL;
			goto err_invalid_scanf;
		}

		if (state < 0 ||
		    (port->nt_mode && state > 7) ||
		    (!port->nt_mode && state > 3)) {
			err = -EINVAL;
			goto err_invalid_state;
		}

		hfc_outb(card, hfc_STATES,
			hfc_STATES_STATE(state) |
			hfc_STATES_LOAD_STATE);
	}

	hfc_card_unlock(card);

	return count;

err_invalid_scanf:
err_invalid_state:

	hfc_card_unlock(card);

	return err;
}

static VISDN_PORT_ATTR(l1_state, S_IRUGO | S_IWUSR,
		hfc_show_l1_state,
		hfc_store_l1_state);

//----------------------------------------------------------------------------

static ssize_t hfc_show_st_clock_delay(
	struct visdn_port *visdn_port,
	struct visdn_port_attribute *attr,
	char *buf)
{
	struct hfc_st_port *port = to_st_port(visdn_port);

	return snprintf(buf, PAGE_SIZE, "%02x\n", port->clock_delay);
}

static ssize_t hfc_store_st_clock_delay(
	struct visdn_port *visdn_port,
	struct visdn_port_attribute *attr,
	const char *buf,
	size_t count)
{
	struct hfc_st_port *port = to_st_port(visdn_port);
	struct hfc_card *card = port->card;

	unsigned int value;
	if (sscanf(buf, "%02x", &value) < 1)
		return -EINVAL;

	if (value > 0x0f)
		return -EINVAL;

	hfc_card_lock(card);
	port->clock_delay = value;
	hfc_st_port_update_st_clk_dly(port);
	hfc_card_unlock(card);

	return count;
}

static VISDN_PORT_ATTR(st_clock_delay, S_IRUGO | S_IWUSR,
		hfc_show_st_clock_delay,
		hfc_store_st_clock_delay);

//----------------------------------------------------------------------------
static ssize_t hfc_show_st_sampling_comp(
	struct visdn_port *visdn_port,
	struct visdn_port_attribute *attr,
	char *buf)
{
	struct hfc_st_port *port = to_st_port(visdn_port);

	return snprintf(buf, PAGE_SIZE, "%02x\n", port->sampling_comp);
}

static ssize_t hfc_store_st_sampling_comp(
	struct visdn_port *visdn_port,
	struct visdn_port_attribute *attr,
	const char *buf,
	size_t count)
{
	struct hfc_st_port *port = to_st_port(visdn_port);
	struct hfc_card *card = port->card;

	unsigned int value;
	if (sscanf(buf, "%u", &value) < 1)
		return -EINVAL;

	if (value > 0x7)
		return -EINVAL;

	hfc_card_lock(card);
	port->sampling_comp = value;
	hfc_st_port_update_st_clk_dly(port);
	hfc_card_unlock(card);

	return count;
}

static VISDN_PORT_ATTR(st_sampling_comp, S_IRUGO | S_IWUSR,
		hfc_show_st_sampling_comp,
		hfc_store_st_sampling_comp);

int hfc_st_port_sysfs_create_files(
	struct hfc_st_port *port)
{
	int err;

	err = visdn_port_create_file(
		&port->visdn_port,
		&visdn_port_attr_role);
	if (err < 0)
		goto err_create_file_role;

	err = visdn_port_create_file(
		&port->visdn_port,
		&visdn_port_attr_l1_state);
	if (err < 0)
		goto err_create_file_l1_state;

	err = visdn_port_create_file(
		&port->visdn_port,
		&visdn_port_attr_st_clock_delay);
	if (err < 0)
		goto err_create_file_st_clock_delay;

	err = visdn_port_create_file(
		&port->visdn_port,
		&visdn_port_attr_st_sampling_comp);
	if (err < 0)
		goto err_create_file_st_sampling_comp;

	return 0;

	visdn_port_remove_file(
		&port->visdn_port,
		&visdn_port_attr_role);
err_create_file_role:
	visdn_port_remove_file(
		&port->visdn_port,
		&visdn_port_attr_st_sampling_comp);
err_create_file_st_sampling_comp:
	visdn_port_remove_file(
		&port->visdn_port,
		&visdn_port_attr_st_clock_delay);
err_create_file_st_clock_delay:
	visdn_port_remove_file(
		&port->visdn_port,
		&visdn_port_attr_l1_state);
err_create_file_l1_state:

	return err;
}

void hfc_st_port_sysfs_delete_files(
	struct hfc_st_port *port)
{
	visdn_port_remove_file(
		&port->visdn_port,
		&visdn_port_attr_role);
	visdn_port_remove_file(
		&port->visdn_port,
		&visdn_port_attr_st_sampling_comp);
	visdn_port_remove_file(
		&port->visdn_port,
		&visdn_port_attr_st_clock_delay);
	visdn_port_remove_file(
		&port->visdn_port,
		&visdn_port_attr_l1_state);
}

void hfc_st_port_update_sctrl(struct hfc_st_port *port)
{
	u8 sctrl = 0;

	// Select the non-capacitive line mode for the S/T interface */
	sctrl = hfc_SCTRL_NONE_CAP;

	if (port->nt_mode)
		sctrl |= hfc_SCTRL_MODE_NT;
	else
		sctrl |= hfc_SCTRL_MODE_TE;

	if (port->sq_enabled)
		sctrl |= hfc_SCTRL_SQ_ENA;

	if (port->chans[B1].status != HFC_CHAN_STATUS_FREE)
		sctrl |= hfc_SCTRL_B1_ENA;

	if (port->chans[B2].status != HFC_CHAN_STATUS_FREE)
		sctrl |= hfc_SCTRL_B2_ENA;

	hfc_outb(port->card, hfc_SCTRL, sctrl);
}

void hfc_st_port_update_sctrl_r(struct hfc_st_port *port)
{
	u8 sctrl_r = 0;

	if (port->chans[B1].status != HFC_CHAN_STATUS_FREE)
		sctrl_r |= hfc_SCTRL_R_B1_ENA;

	if (port->chans[B2].status != HFC_CHAN_STATUS_FREE)
		sctrl_r |= hfc_SCTRL_R_B2_ENA;

	hfc_outb(port->card, hfc_SCTRL_R, sctrl_r);
}

void hfc_st_port_update_st_clk_dly(struct hfc_st_port *port)
{
	hfc_outb(port->card, hfc_CLKDEL,
		hfc_CLKDEL_ST_CLK_DLY(port->clock_delay) |
		hfc_CLKDEL_ST_SMPL(port->sampling_comp));
}

static void hfc_st_port_state_change_work(void *data)
{
	struct hfc_st_port *port = data;
	struct hfc_card *card = port->card;

	hfc_card_lock(card);

	u8 new_state = hfc_inb(card, hfc_STATES) & hfc_STATES_STATE_MASK;

	hfc_debug_port(port, 1,
			"layer 1 state = %c%d\n",
			port->nt_mode?'G':'F',
			new_state);

	if (port->nt_mode) {
		// NT mode

		if (new_state == 2) {
			// Allows transition from G2 to G3
			hfc_outb(card, hfc_STATES,
				hfc_STATES_ACTIVATE |
				hfc_STATES_NT_G2_G3);
		} else if (new_state == 3) {
			// fix to G3 state (see specs)
			hfc_outb(card, hfc_STATES, hfc_STATES_LOAD_STATE | 3);
		}

		if (new_state == 3 && port->l1_state != 3) {
			//hfc_resume_fifo(card);
		}

		if (new_state != 3 && port->l1_state == 3) {
			//hfc_suspend_fifo(card);
		}
	} else {
		if (new_state == 3) {
		}

		if (new_state == 7 && port->l1_state != 7) {
			// TE is now active, schedule FIFO activation after
			// some time, otherwise the first frames are lost

			card->regs.ctmt |= hfc_CTMT_TIMER_50 | hfc_CTMT_TIMER_CLEAR;
			hfc_outb(card, hfc_CTMT, card->regs.ctmt);

			// Activating the timer firest an interrupt immediately, we
			// obviously need to ignore it
			card->ignore_first_timer_interrupt = TRUE;
		}

		if (new_state != 7 && port->l1_state == 7) {
			// TE has become inactive, disable FIFO
			//hfc_suspend_fifo(card);
		}
	}

	port->l1_state = new_state;

	hfc_card_unlock(card);
}

void hfc_st_port_check_l1_up(struct hfc_st_port *port)
{
	struct hfc_card *card = port->card;

	if (port->visdn_port.enabled &&
		((!port->nt_mode && port->l1_state != 7) ||
		(port->nt_mode && port->l1_state != 3))) {

		hfc_debug_port(port, 1,
			"L1 is down, bringing up L1.\n");

       		hfc_outb(card, hfc_STATES, hfc_STATES_ACTIVATE);
       	}
}

static void hfc_st_port_release(
	struct visdn_port *port)
{
	printk(KERN_DEBUG "hfc_st_port_release()\n");

	// FIXME
}

static int hfc_st_port_enable(
	struct visdn_port *visdn_port)
{
	struct hfc_st_port *port = to_st_port(visdn_port);

	hfc_outb(port->card, hfc_STATES,
		hfc_STATES_STATE(0));

	hfc_debug_port(port, 2, "enabled\n");

	return 0;
}

static int hfc_st_port_disable(
	struct visdn_port *visdn_port)
{
	struct hfc_st_port *port = to_st_port(visdn_port);

	hfc_outb(port->card, hfc_STATES,
		hfc_STATES_STATE(0) |
		hfc_STATES_LOAD_STATE);

	hfc_debug_port(port, 2, "disabled\n");

	return 0;
}

struct visdn_port_ops hfc_st_port_ops = {
	.owner		= THIS_MODULE,
	.release	= hfc_st_port_release,
	.enable		= hfc_st_port_enable,
	.disable	= hfc_st_port_disable,
};

void hfc_st_port_init(
	struct hfc_st_port *port,
	struct hfc_card *card,
	const char *name)
{
	port->card = card;

	INIT_WORK(&port->state_change_work,
		hfc_st_port_state_change_work,
		port);

	port->nt_mode = FALSE;
	port->clock_delay = HFC_DEF_TE_CLK_DLY;
	port->sampling_comp = HFC_DEF_TE_SAMPL_COMP;

	visdn_port_init(&port->visdn_port);
	port->visdn_port.ops = &hfc_st_port_ops;
	port->visdn_port.driver_data = port;
	port->visdn_port.device = &card->pcidev->dev;
	strncpy(port->visdn_port.name, name, sizeof(port->visdn_port.name));;

	// Note: Bitrates must be in increasing order
	int bitrates_d[] = { 16000 };
	int bitrates_b[] = { 64000 };
	int bitrates_s[] = { 4000 };

	hfc_chan_init(&port->chans[D], port, "D", D, hfc_D_CHAN_OFF,
		bitrates_d, ARRAY_SIZE(bitrates_d));
	hfc_chan_init(&port->chans[B1], port, "B1", B1, hfc_B1_CHAN_OFF,
		bitrates_b, ARRAY_SIZE(bitrates_b));
	hfc_chan_init(&port->chans[B2], port, "B2", B2, hfc_B2_CHAN_OFF,
		bitrates_b, ARRAY_SIZE(bitrates_b));
	hfc_chan_init(&port->chans[E], port, "E", E, hfc_E_CHAN_OFF,
		bitrates_d, ARRAY_SIZE(bitrates_d));
	hfc_chan_init(&port->chans[SQ], port, "SQ", SQ, 0,
		bitrates_s, ARRAY_SIZE(bitrates_s));
}
