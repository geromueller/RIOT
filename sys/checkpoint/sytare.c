/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @ingroup     sys_sytare
 * @{
 *
 * @file
 * @brief       Sytare implementation
 *
 * @author      Gero Müller <gero.mueller@inria.fr>
 * @}
 */

#include "irq.h"
#include <stdint.h>

/**
 * @brief   Save all active periphs state
 * 
 * @param[out] state    buffer to save the state
 */
extern void _sytare_save_periphs_state(uint8_t checkpoint);

/**
 * @brief   Restore all active periphs state
 * 
 * @param[in] state    buffer storing the state
 */
extern void _sytare_restore_periphs_state(uint8_t checkpoint);

extern void _sytare_save_ram(uint8_t checkpoint);
extern void _sytare_restore_ram(uint8_t checkpoint);

// returns the next checkpoint index
extern uint8_t _sytare_next_checkpoint(void);
extern uint8_t _sytare_current_checkpoint(void);

// returns the next checkpoint index

void sytare_save(void)
{
	unsigned irq_status = irq_disable();

	const uint8_t checkpoint = _sytare_next_checkpoint();

	_sytare_save_periphs_state(checkpoint);

	_sytare_save_ram(checkpoint);

	irq_restore(irq_status);
}

void sytare_restore(void)
{
	unsigned irq_status = irq_disable();

	const uint8_t checkpoint = _sytare_current_checkpoint();

	_sytare_restore_periphs_state(checkpoint);

	_sytare_restore_ram(checkpoint);

	irq_restore(irq_status);
}
