/*
 * Copyright (c) 2003-2005, Bernard Leach <leachbj@bouncycastle.org>
 */

#ifndef __ASM_ARCH_IRQS_H__
#define __ASM_ARCH_IRQS_H__

#define NR_IRQS	64

/* PP5002 */
#define PP5002_IDE_IRQ		1
#define PP5002_SER0_IRQ		4
#define PP5002_I2S_IRQ		5
#define PP5002_SER1_IRQ		7
#define PP5002_TIMER1_IRQ	11
#define PP5002_GPIO_IRQ		14
#define PP5002_DMA_OUT_IRQ	30
#define PP5002_DMA_IN_IRQ	31

#define PP5002_VALID_IRQ(x)	(x==PP5002_IDE_IRQ||x==PP5002_SER0_IRQ||x==PP5002_I2S_IRQ||x==PP5002_SER1_IRQ||x==PP5002_TIMER1_IRQ||x==PP5002_GPIO_IRQ||x==PP5002_DMA_OUT_IRQ||x==PP5002_DMA_IN_IRQ)

#define PP5002_IDE_MASK		(1 << PP5002_IDE_IRQ)
#define PP5002_SER0_MASK	(1 << PP5002_SER0_IRQ)
#define PP5002_I2S_MASK		(1 << PP5002_I2S_IRQ)
#define PP5002_SER1_MASK	(1 << PP5002_SER1_IRQ)
#define PP5002_TIMER1_MASK	(1 << PP5002_TIMER1_IRQ)
#define PP5002_GPIO_MASK	(1 << PP5002_GPIO_IRQ)
#define PP5002_DMA_OUT_MASK	(1 << PP5002_DMA_OUT_IRQ)

/* PP5020 */
#define PP5020_TIMER1_IRQ	0
#define PP5020_TIMER2_IRQ	1
#define PP5020_I2S_IRQ		10
#define PP5020_IDE_IRQ		23
#define PP5020_GPIO_IRQ		(32+0)
#define PP5020_SER0_IRQ		(32+4)
#define PP5020_SER1_IRQ		(32+5)
#define PP5020_I2C_IRQ		(32+8)

#define PP5020_VALID_IRQ(x)	(x==PP5020_TIMER1_IRQ||x==PP5020_I2S_IRQ||x==PP5020_GPIO_IRQ||x==PP5020_SER0_IRQ||x==PP5020_SER1_IRQ||x==PP5020_I2C_IRQ||x==PP5020_IDE_IRQ)

#define PP5020_TIMER1_MASK	(1 << PP5020_TIMER1_IRQ)
#define PP5020_I2S_MASK		(1 << PP5020_I2S_IRQ)
#define PP5020_IDE_MASK		(1 << PP5020_IDE_IRQ)
#define PP5020_GPIO_MASK	(1 << (PP5020_GPIO_IRQ-32))
#define PP5020_SER0_MASK	(1 << (PP5020_SER0_IRQ-32))
#define PP5020_SER1_MASK	(1 << (PP5020_SER1_IRQ-32))
#define PP5020_I2C_MASK		(1 << (PP5020_I2C_IRQ-32))

#endif
