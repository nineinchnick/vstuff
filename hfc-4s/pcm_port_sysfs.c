#include <linux/kernel.h>
#include <linux/spinlock.h>

#include "pcm_port.h"
#include "pcm_port_sysfs.h"
#include "card.h"
#include "card_inline.h"

static ssize_t hfc_show_bitrate(
	struct device *device,
	char *buf)
{
	struct visdn_port *visdn_port = to_visdn_port(device);
	struct hfc_pcm_port *port = to_pcm_port(visdn_port);

	int bitrate = 0;

	switch(port->bitrate) {
	case 0: bitrate = 2; break;
	case 1: bitrate = 4; break;
	case 2: bitrate = 8; break;
	}

	return snprintf(buf, PAGE_SIZE, "%d\n", bitrate);

}

static ssize_t hfc_store_bitrate(
	struct device *device,
	const char *buf,
	size_t count)
{
	struct visdn_port *visdn_port = to_visdn_port(device);
	struct hfc_pcm_port *port = to_pcm_port(visdn_port);
	struct hfc_card *card = port->card;

	int value;
	if (sscanf(buf, "%d", &value) < 1)
		return -EINVAL;

	if (value == 2)
		port->bitrate = 0;
	else if (value == 4)
		port->bitrate = 1;
	else if (value == 8)
		port->bitrate = 2;
	return -EINVAL;

	unsigned long flags;
	spin_lock_irqsave(&card->lock, flags);
	mb();
	hfc_outb(card, hfc_R_CIRM, hfc_R_CIRM_V_PCM_RES);
	mb();
	hfc_outb(card, hfc_R_CIRM, 0);
	mb();
	hfc_wait_busy(card);
	hfc_update_pcm_md0(card, 0);
	hfc_update_pcm_md1(card);
	spin_unlock_irqrestore(&card->lock, flags);

	return count;
}

static DEVICE_ATTR(bitrate, S_IRUGO | S_IWUSR,
		hfc_show_bitrate,
		hfc_store_bitrate);

//----------------------------------------------------------------------------

static ssize_t hfc_show_master(
	struct device *device,
	char *buf)
{
	struct visdn_port *visdn_port = to_visdn_port(device);
	struct hfc_pcm_port *port = to_pcm_port(visdn_port);

	return snprintf(buf, PAGE_SIZE, "%d\n", port->master ? 1 : 0);
}

static ssize_t hfc_store_master(
	struct device *device,
	const char *buf,
	size_t count)
{
	struct visdn_port *visdn_port = to_visdn_port(device);
	struct hfc_pcm_port *port = to_pcm_port(visdn_port);
	struct hfc_card *card = port->card;

	int value;
	if (sscanf(buf, "%d", &value) < 1)
		return -EINVAL;

	if (value != 0 && value != 1)
		return -EINVAL;

	unsigned long flags;
	spin_lock_irqsave(&card->lock, flags);
	port->master = value;
	hfc_update_pcm_md0(card, 0);
	spin_unlock_irqrestore(&card->lock, flags);

	return count;
}

static DEVICE_ATTR(master, S_IRUGO | S_IWUSR,
		hfc_show_master,
		hfc_store_master);

//----------------------------------------------------------------------------


static ssize_t hfc_show_f0io_counter(
	struct device *device,
	char *buf)
{
	struct visdn_port *visdn_port = to_visdn_port(device);
	struct hfc_pcm_port *port = to_pcm_port(visdn_port);
	struct hfc_card *card = port->card;

	unsigned long flags;
	spin_lock_irqsave(&card->lock, flags);

	u16 counter;

	counter = hfc_inb(card, hfc_R_F0_CNTL);
	counter += hfc_inb(card, hfc_R_F0_CNTH) << 8;

	spin_unlock_irqrestore(&card->lock, flags);

	return snprintf(buf, PAGE_SIZE, "%d\n", counter);

}

static DEVICE_ATTR(f0io_counter, S_IRUGO,
		hfc_show_f0io_counter,
		NULL);

//----------------------------------------------------------------------------

static ssize_t hfc_show_slots_state(
	struct device *device,
	char *buf)
{
	struct visdn_port *visdn_port = to_visdn_port(device);
	struct hfc_pcm_port *port = to_pcm_port(visdn_port);
	struct hfc_card *card = port->card;

	int len = 0;

	len += snprintf(buf + len, PAGE_SIZE - len,
		"Slot    Chan\n");

	unsigned long flags;
	spin_lock_irqsave(&card->lock, flags);

	int i;
	for (i=0; i<port->num_slots; i++) {
		len += snprintf(buf + len, PAGE_SIZE - len,
			"[%2d,%s]",
			port->slots[i][RX].hw_index,
			port->slots[i][RX].direction == RX ? "RX" : "TX");

		if (port->slots[i][RX].connected_chan) {
			len += snprintf(buf + len, PAGE_SIZE - len,
				" [%2d,%s]",
				port->slots[i][RX].connected_chan->chan->hw_index,
				port->slots[i][RX].connected_chan->direction == RX ?
					"RX" : "TX");
		}

		len += snprintf(buf + len, PAGE_SIZE - len, "\n");

		len += snprintf(buf + len, PAGE_SIZE - len,
			"[%2d,%s]",
			port->slots[i][TX].hw_index,
			port->slots[i][TX].direction == RX ? "RX" : "TX");

		if (port->slots[i][TX].connected_chan) {
			len += snprintf(buf + len, PAGE_SIZE - len,
				" [%2d,%s]",
				port->slots[i][TX].connected_chan->chan->hw_index,
				port->slots[i][TX].connected_chan->direction == RX ?
					"RX" : "TX");
		}

		len += snprintf(buf + len, PAGE_SIZE - len, "\n");
	}

	spin_unlock_irqrestore(&card->lock, flags);

	return len;

}

static DEVICE_ATTR(slots_state, S_IRUGO,
		hfc_show_slots_state,
		NULL);

int hfc_pcm_port_sysfs_create_files(
        struct hfc_pcm_port *port)
{
	int err;

	err = device_create_file(
		&port->visdn_port.device,
		&dev_attr_master);
	if (err < 0)
		goto err_device_create_file_master;

	err = device_create_file(
		&port->visdn_port.device,
		&dev_attr_bitrate);
	if (err < 0)
		goto err_device_create_file_bitrate;

	err = device_create_file(
		&port->visdn_port.device,
		&dev_attr_f0io_counter);
	if (err < 0)
		goto err_device_create_file_f0io_counter;

	err = device_create_file(
		&port->visdn_port.device,
		&dev_attr_slots_state);
	if (err < 0)
		goto err_device_create_file_slots_state;

	return 0;

	device_remove_file(
		&port->visdn_port.device,
		&dev_attr_slots_state);
err_device_create_file_slots_state:
	device_remove_file(
		&port->visdn_port.device,
		&dev_attr_f0io_counter);
err_device_create_file_f0io_counter:
	device_remove_file(
		&port->visdn_port.device,
		&dev_attr_bitrate);
err_device_create_file_bitrate:
	device_remove_file(
		&port->visdn_port.device,
		&dev_attr_master);
err_device_create_file_master:

	return err;

	return 0;
}

void hfc_pcm_port_sysfs_delete_files(
        struct hfc_pcm_port *port)
{
	device_remove_file(
		&port->visdn_port.device,
		&dev_attr_slots_state);
	device_remove_file(
		&port->visdn_port.device,
		&dev_attr_f0io_counter);
	device_remove_file(
		&port->visdn_port.device,
		&dev_attr_bitrate);
	device_remove_file(
		&port->visdn_port.device,
		&dev_attr_master);
}