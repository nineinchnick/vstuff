#ifndef _PORT_H
#define _PORT_H

#define VISDN_PORT_HASHBITS 8

extern struct hlist_head visdn_port_index_hash[];

#define to_visdn_port(class) container_of(class, struct visdn_port, device)

struct visdn_port_ops
{
	int (*set_role)(struct visdn_port *port, int nt_mode);
};

struct visdn_port
{
	void *priv;

	struct device device;

	struct hlist_node index_hlist;
	int index;

	char port_name[BUS_ID_SIZE];

	struct visdn_port_ops *ops;

	int nt_mode;
};

int visdn_port_modinit(void);
void visdn_port_modexit(void);

void visdn_port_init(
	struct visdn_port *visdn_port,
	struct visdn_port_ops *ops);

struct visdn_port *visdn_port_alloc(void);

int visdn_port_register(
	struct visdn_port *visdn_port,
	const char *name,
	struct device *parent_device);

void visdn_port_unregister(
	struct visdn_port *visdn_port);

#endif
