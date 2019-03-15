/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @ingroup     sys_checkpoint
 * @{
 *
 * @file
 * @brief       Sytare implementation
 *
 * @author      Loïc Saos <Loic.Saos@insa-lyon.fr>
 * @author      Gero Müller <gero.mueller@inria.fr>
 * @}
 */

#include "led.h"

#ifdef MODULE_CHECKPOINT

#undef MODULE_PERIPH_DMA
#undef MODULE_PERIPH_GPIO
#undef MODULE_PERIPH_UART
//#define MODULE_PERIPH_TIMER
#include "periph_checkpoint.h"
#include "periph_dma.h"
#include "periph_conf.h"
#include "checkpoint.h"
#include "cpu.h"
#include "string.h"
#include <stdint.h>

#define CHECKPOINT_COUNT 2
#define CHECKPOINT_ORIGIN 0x1c00
#define CHECKPOINT_SIZE 0x0800

typedef struct
{
#ifdef MODULE_PERIPH_DMA
	dma_checkpoint_state_t dma;
#endif /* MODULE_PERIPH_DMA */
#ifdef MODULE_PERIPH_GPIO
	gpio_checkpoint_state_t gpio;
#endif /* MODULE_PERIPH_GPIO */
#ifdef MODULE_PERIPH_UART
	uart_checkpoint_state_t uart;
#endif /* MODULE_PERIPH_UART */
#ifdef MODULE_PERIPH_TIMER
	timer_checkpoint_state_t timer;
#endif /* MODULE_PERIPH_TIMER */
} _checkpoint_periph_states;

__attribute__((section(".persistent")))
                             _checkpoint_periph_states _checkpoint_periphs_state[CHECKPOINT_COUNT];

void _checkpoint_save_periphs_state(uint8_t checkpoint)
{
#ifdef MODULE_PERIPH_DMA
	dma_save_state(&_checkpoint_periphs_state[checkpoint].dma);
#endif /* MODULE_PERIPH_DMA */
#ifdef MODULE_PERIPH_GPIO
	gpio_save_state(&_checkpoint_periphs_state[checkpoint].gpio);
#endif /* MODULE_PERIPH_GPIO */
#ifdef MODULE_PERIPH_UART
	uart_save_state(&_checkpoint_periphs_state[checkpoint].uart);
#endif /* MODULE_PERIPH_UART */
#ifdef MODULE_PERIPH_TIMER
	timer_save_state(&_checkpoint_periphs_state[checkpoint].timer);
#endif /* MODULE_PERIPH_TIMER */
}

void _checkpoint_restore_periphs_state(uint8_t checkpoint)
{
#ifdef MODULE_PERIPH_DMA
	dma_restore_state(&_checkpoint_periphs_state[checkpoint].dma);
#endif /* MODULE_PERIPH_DMA */
#ifdef MODULE_PERIPH_GPIO
	gpio_restore_state(&_checkpoint_periphs_state[checkpoint].gpio);
#endif /* MODULE_PERIPH_GPIO */
#ifdef MODULE_PERIPH_UART
	uart_restore_state(&_checkpoint_periphs_state[checkpoint].uart);
#endif /* MODULE_PERIPH_UART */
#ifdef MODULE_PERIPH_TIMER
	timer_restore_state(&_checkpoint_periphs_state[checkpoint].timer);
#endif /* MODULE_PERIPH_TIMER */

}

static __attribute__((always_inline)) inline void _dma_memcpy(char * dst, const char * src, size_t len)
{
    // Any other case than the one above supports word transfer
    // reset control register
    DMA_CHAN_0->CTL = 0;

     //use DMAREQ as trigger
    DMA_MODULE->CTL0 |= DMA_CTL0_SEL_REQ;

    DMA_CHAN_0->SA = (size_t)src;
    DMA_CHAN_0->DA = (size_t)dst;
    DMA_CHAN_0->SZ = len / 2;

    DMA_CHAN_0->CTL = DMA_CC_DT_BLOCK   // Block transfer mode
                      | DMA_CC_DST_INCR  //Increment dst address
                      | DMA_CC_SRC_INCR  //Increment src address
                      | DMA_CC_EN        //Enable DMA
                      | DMA_CC_REQ       //Request DMA (runs copy)
                      ;      //  Byte or word transfer

}

/* using the high fram section to store the checkpoints. Alternative might be to use
 * LD scripts using INSERT to add persistent sections with size of data and bss
 */
__attribute__((section(".persistent")))
                             char _checkpoint_ram[CHECKPOINT_COUNT][CHECKPOINT_SIZE];

static __attribute__((always_inline)) inline void _checkpoint_save_ram(uint8_t checkpoint)
{
	_dma_memcpy(_checkpoint_ram[checkpoint], (const char *) CHECKPOINT_ORIGIN,
	CHECKPOINT_SIZE);

}

static __attribute__((always_inline)) inline void _checkpoint_restore_ram(uint8_t checkpoint)
{
	_dma_memcpy((char *) CHECKPOINT_ORIGIN, _checkpoint_ram[checkpoint],
	CHECKPOINT_SIZE);
}

__attribute__((section(".persistent")))
                                    volatile uint8_t _checkpoint_checkpoint = 0xff;

__attribute__((always_inline)) inline uint8_t  _checkpoint_next_checkpoint(void)
{
	if (_checkpoint_checkpoint == 0xff) {
		return 0;
	} else {
		return (_checkpoint_checkpoint + 1) % CHECKPOINT_COUNT;
	}
}

// returns the next checkpoint index
__attribute__((always_inline)) inline  uint8_t _checkpoint_current_checkpoint(void)
{
	return _checkpoint_checkpoint;
}

void checkpoint_reset(void)
{
	_checkpoint_checkpoint = 0xff;
}

extern void board_init(void);


__attribute__((naked)) void checkpoint_save_irq(void)
{
    __disable_irq();
    __save_context();

    const uint8_t checkpoint = _checkpoint_next_checkpoint();

	_checkpoint_save_periphs_state(checkpoint);

	_checkpoint_save_ram(checkpoint);

	__exit_isr();

	UNREACHABLE();
}

__attribute__((naked)) void checkpoint_save(void)
{
    __asm__("push r2"); /* save SR */
    __disable_irq();
    __enter_isr();

	const uint8_t checkpoint = _checkpoint_next_checkpoint();

	_checkpoint_save_periphs_state(checkpoint);

	_checkpoint_save_ram(checkpoint);

	_checkpoint_checkpoint = checkpoint;

	__exit_isr();

	UNREACHABLE();
}

__attribute__((naked)) void checkpoint_restore(void)
{
    __disable_irq();

    _checkpoint_restore_ram(_checkpoint_current_checkpoint());
	_checkpoint_restore_periphs_state(_checkpoint_current_checkpoint());

	__restore_context();

    UNREACHABLE();
}

void __attribute__ ((section (".crt_0001_checkpoint_start"), used, naked)) _checkpoint_start(
		void)
{
	// Check for checkpoint
	//TODO: check for button press to force normal boot
	if (_checkpoint_checkpoint != 0xff)
	{
		board_init();
		checkpoint_restore();
	}
}

#endif // MODULE_SYTARE
