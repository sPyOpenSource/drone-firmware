/*
 * arch/ppc/kernel/gen550_kgdb.c
 *
 * Generic 16550 kgdb support intended to be useful on a variety
 * of platforms.  To enable this support, it is necessary to set
 * the CONFIG_GEN550 option.  Any virtual mapping of the serial
 * port(s) to be used can be accomplished by setting
 * ppc_md.early_serial_map to a platform-specific mapping function.
 *
 * Adapted from ppc4xx_kgdb.c.
 *
 * Matt Porter <mporter@mvista.com>
 *
 * Copyright 2002 MontaVista Software Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/config.h>
#include <linux/types.h>
#include <linux/kernel.h>

#include <asm/machdep.h>

extern unsigned long serial_init(int, void *);
extern unsigned long serial_getc(unsigned long);
extern unsigned long serial_putc(unsigned long, unsigned char);

#if defined(CONFIG_KGDB_TTYS0)
#define KGDB_PORT 0
#elif defined(CONFIG_KGDB_TTYS1)
#define KGDB_PORT 1
#elif defined(CONFIG_KGDB_TTYS2)
#define KGDB_PORT 2
#elif defined(CONFIG_KGDB_TTYS3)
#define KGDB_PORT 3
#else
#error "invalid kgdb_tty port"
#endif

static volatile unsigned int kgdb_debugport;

void putDebugChar(unsigned char c)
{
	if (kgdb_debugport == 0)
		kgdb_debugport = serial_init(KGDB_PORT, NULL);

	serial_putc(kgdb_debugport, c);
}

int getDebugChar(void)
{
	if (kgdb_debugport == 0)
		kgdb_debugport = serial_init(KGDB_PORT, NULL);

	return(serial_getc(kgdb_debugport));
}

void kgdb_interruptible(int enable)
{
	return;
}

void putDebugString(char* str)
{
	while (*str != '\0') {
		putDebugChar(*str);
		str++;
	}
	putDebugChar('\r');
	return;
}

void
kgdb_map_scc(void)
{
	printk("kgdb init\n");
	if (ppc_md.early_serial_map)
		ppc_md.early_serial_map();
	kgdb_debugport = serial_init(KGDB_PORT, NULL);
}