
#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph_dma.h"
#include "periph_checkpoint.h"

void dma_save_state(dma_checkpoint_state_t* state)
{
	state->SA = DMA_CHAN_0->SA;
	state->DA = DMA_CHAN_0->DA;
	state->SZ = DMA_CHAN_0->SZ;
}

void dma_restore_state(const dma_checkpoint_state_t* state)
{
	DMA_CHAN_0->CTL = 0;
	DMA_MODULE->CTL0 |= DMA_CTL0_SEL_REQ;

	DMA_CHAN_0->SA = state->SA;
	DMA_CHAN_0->DA = state->DA;
	DMA_CHAN_0->SZ = state->SZ;
}

void gpio_save_state(gpio_checkpoint_state_t* state)
{
	state->OD[0] = P1OUT;
	state->OD[1] = P2OUT;
	state->OD[2] = P3OUT;
	state->OD[3] = P4OUT;
	state->OD[4] = PJOUT;

	state->DIR[0] = P1DIR;
	state->DIR[1] = P2DIR;
	state->DIR[2] = P3DIR;
	state->DIR[3] = P4DIR;
	state->DIR[4] = PJDIR;

	state->REN[0] = P1REN;
	state->REN[1] = P2REN;
	state->REN[2] = P3REN;
	state->REN[3] = P4REN;
	state->REN[4] = PJREN;

	state->SEL0[0] = P1SEL0;
	state->SEL0[1] = P2SEL0;
	state->SEL0[2] = P3SEL0;
	state->SEL0[3] = P4SEL0;
	state->SEL0[4] = PJSEL0;

	state->SEL1[0] = P1SEL1;
	state->SEL1[1] = P2SEL1;
	state->SEL1[2] = P3SEL1;
	state->SEL1[3] = P4SEL1;
	state->SEL1[4] = PJSEL1;

	state->SELC[0] = P1SELC;
	state->SELC[1] = P2SELC;
	state->SELC[2] = P3SELC;
	state->SELC[3] = P4SELC;
	state->SELC[4] = PJSELC;

	state->IES[0] = P1IES;
	state->IES[1] = P2IES;
	state->IES[2] = P3IES;
	state->IES[3] = P4IES;

	state->IE[0] = P1IE;
	state->IE[1] = P2IE;
	state->IE[2] = P3IE;
	state->IE[3] = P4IE;
}

void gpio_restore_state(const gpio_checkpoint_state_t* state)
{
	P1OUT = state->OD[0];
	P2OUT = state->OD[1];
	P3OUT = state->OD[2];
	P4OUT = state->OD[3];
	PJOUT = state->OD[4];

	P1REN = state->REN[0];
	P2REN = state->REN[1];
	P3REN = state->REN[2];
	P4REN = state->REN[3];
	PJREN = state->REN[4];

	P1DIR = state->DIR[0];
	P2DIR = state->DIR[1];
	P3DIR = state->DIR[2];
	P4DIR = state->DIR[3];
	PJDIR = state->DIR[4];

	P1IES = state->IES[0];
	P2IES = state->IES[1];
	P3IES = state->IES[2];
	P4IES = state->IES[3];

	P1IE = state->IE[0];
	P2IE = state->IE[1];
	P3IE = state->IE[2];
	P4IE = state->IE[3];

	P1IFG = 0;
	P2IFG = 0;
	P3IFG = 0;
	P4IFG = 0;

	P1SEL0 = state->SEL0[0];
	P2SEL0 = state->SEL0[1];
	P3SEL0 = state->SEL0[2];
	P4SEL0 = state->SEL0[3];
	PJSEL0 = state->SEL0[4];

	P1SEL1 = state->SEL1[0];
	P2SEL1 = state->SEL1[1];
	P3SEL1 = state->SEL1[2];
	P4SEL1 = state->SEL1[3];
	PJSEL1 = state->SEL1[4];

	P1SELC = state->SELC[0];
	P2SELC = state->SELC[1];
	P3SELC = state->SELC[2];
	P4SELC = state->SELC[3];
	PJSELC = state->SELC[4];
}

void uart_save_state(uart_checkpoint_state_t* state)
{
	state->BR0 = UCA0BR0;
	state->BR1 = UCA0BR1;
	state->MCTLW = UCA0MCTLW;
}

void uart_restore_state(const uart_checkpoint_state_t* state)
{
	UCA0CTLW0 = USCI_CTL0_SWRST;

	UCA0CTLW0 |= USCI_CTL0_SSEL_SMCLK;
	UCA0BR0 = state->BR0;
	UCA0BR1 = state->BR1;
	UCA0MCTLW |= state->MCTLW;

	UCA0CTLW0 &= ~USCI_CTL0_SWRST;

	UART_IF &= ~UART_IE_RX_BIT;
	UART_IF |= UART_IE_TX_BIT;
	UART_IE |= UART_IE_RX_BIT;
	UART_IE &= ~UART_IE_TX_BIT;
}

void timer_save_state(timer_checkpoint_state_t* state)
{
	// TA0
	state->R[0] = TA0R;
	state->CTL[0] = TA0CTL;

	state->CCR[0][0] = TA0CCR0;
	state->CCR[0][1] = TA0CCR1;
	state->CCR[0][2] = TA0CCR2;

	state->CCTL[0][0] = TA0CCTL0;
	state->CCTL[0][1] = TA0CCTL1;
	state->CCTL[0][2] = TA0CCTL2;

	// TA1
	state->R[1] = TA1R;
	state->CTL[1] = TA1CTL;

	state->CCR[1][0] = TA1CCR0;
	state->CCR[1][1] = TA1CCR1;
	state->CCR[1][2] = TA1CCR2;

	state->CCTL[1][0] = TA1CCTL0;
	state->CCTL[1][1] = TA1CCTL1;
	state->CCTL[1][2] = TA1CCTL2;

	// TA2
	state->R[2] = TA2R;
	state->CTL[2] = TA2CTL;

	state->CCR[2][0] = TA2CCR0;
	state->CCR[2][1] = TA2CCR1;

	state->CCTL[2][0] = TA2CCTL0;
	state->CCTL[2][1] = TA2CCTL1;

	// TA3
	state->R[3] = TA3R;
	state->CTL[3] = TA3CTL;

	state->CCR[3][0] = TA3CCR0;
	state->CCR[3][1] = TA3CCR1;

	state->CCTL[3][0] = TA3CCTL0;
	state->CCTL[3][1] = TA3CCTL1;

	// TB0
	state->R[4] = TB0R;
	state->CTL[4] = TB0CTL;

	state->CCR[4][0] = TB0CCR0;
	state->CCR[4][1] = TB0CCR1;
	state->CCR[4][2] = TB0CCR2;
	state->CCR[4][3] = TB0CCR3;
	state->CCR[4][4] = TB0CCR4;
	state->CCR[4][5] = TB0CCR4;
	state->CCR[4][6] = TB0CCR6;

	state->CCTL[4][0] = TB0CCTL0;
	state->CCTL[4][1] = TB0CCTL1;
	state->CCTL[4][2] = TB0CCTL2;
	state->CCTL[4][3] = TB0CCTL3;
	state->CCTL[4][4] = TB0CCTL4;
	state->CCTL[4][5] = TB0CCTL5;
	state->CCTL[4][6] = TB0CCTL6;
}

void timer_restore_state(const timer_checkpoint_state_t* state)
{
	// TA0
	TA0CTL = 0;

	TA0CCR0 = state->CCR[0][0];
	TA0CCR1 = state->CCR[0][1];
	TA0CCR2 = state->CCR[0][2];

	TA0CCTL0 = state->CCTL[0][0];
	TA0CCTL1 = state->CCTL[0][1];
	TA0CCTL2 = state->CCTL[0][2];

	TA0R = state->R[0];
	TA0CTL = state->CTL[0];

	// TA1
	TA1CTL = 0;

	TA1CCR0 = state->CCR[1][0];
	TA1CCR1 = state->CCR[1][1];
	TA1CCR2 = state->CCR[1][2];

	TA1CCTL0 = state->CCTL[1][0];
	TA1CCTL1 = state->CCTL[1][1];
	TA1CCTL2 = state->CCTL[1][2];

	TA1R = state->R[1];
	TA1CTL = state->CTL[1];

	// TA2
	TA2CTL = 0;
	TA2CCR0 = state->CCR[2][0];
	TA2CCR1 = state->CCR[2][1];

	TA2CCTL0 = state->CCTL[2][0];
	TA2CCTL1 = state->CCTL[2][1];

	TA2R = state->R[2];
	TA2CTL = state->CTL[2];

	// TA3
	TA3CTL = 0;
	TA3CCR0 = state->CCR[3][0];
	TA3CCR1 = state->CCR[3][1];

	TA3CCTL0 = state->CCTL[3][0];
	TA3CCTL1 = state->CCTL[3][1];

	TA3R = state->R[3];
	TA3CTL = state->CTL[3];

	// TB0
	TB0CTL = 0;
	TB0CCR0 = state->CCR[4][0];
	TB0CCR1 = state->CCR[4][1];
	TB0CCR2 = state->CCR[4][2];
	TB0CCR3 = state->CCR[4][3];
	TB0CCR4 = state->CCR[4][4];
	TB0CCR5 = state->CCR[4][5];
	TB0CCR6 = state->CCR[4][6];

	TB0CCTL0 = state->CCTL[4][0];
	TB0CCTL1 = state->CCTL[4][1];
	TB0CCTL2 = state->CCTL[4][2];
	TB0CCTL3 = state->CCTL[4][3];
	TB0CCTL4 = state->CCTL[4][4];
	TB0CCTL5 = state->CCTL[4][5];
	TB0CCTL6 = state->CCTL[4][6];

	TB0R = state->R[4];
	TB0CTL = state->CTL[4];
}
