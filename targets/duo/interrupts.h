
#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

typedef void (*isr_fn_t)(void);

isr_fn_t get_exti_isr(uint8_t pin_source);

#endif
