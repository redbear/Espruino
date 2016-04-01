#include "jsdevices.h"
#include "interrupts.h"

static void exti0_isr(void) {
    jshPushIOWatchEvent(EV_EXTI0);
}

static void exti1_isr(void) {
    jshPushIOWatchEvent(EV_EXTI1);
}

static void exti2_isr(void) {
    jshPushIOWatchEvent(EV_EXTI2);
}

static void exti3_isr(void) {
    jshPushIOWatchEvent(EV_EXTI3);
}

static void exti4_isr(void) {
    jshPushIOWatchEvent(EV_EXTI4);
}

static void exti5_isr(void) {
    jshPushIOWatchEvent(EV_EXTI5);
}

static void exti6_isr(void) {
    jshPushIOWatchEvent(EV_EXTI6);
}

static void exti7_isr(void) {
    jshPushIOWatchEvent(EV_EXTI7);
}

static void exti8_isr(void) {
    jshPushIOWatchEvent(EV_EXTI8);
}

static void exti9_isr(void) {
    jshPushIOWatchEvent(EV_EXTI9);
}

static void exti10_isr(void) {
    jshPushIOWatchEvent(EV_EXTI10);
}

static void exti11_isr(void) {
    jshPushIOWatchEvent(EV_EXTI11);
}

static void exti12_isr(void) {
    jshPushIOWatchEvent(EV_EXTI12);
}

static void exti13_isr(void) {
    jshPushIOWatchEvent(EV_EXTI13);
}

static void exti14_isr(void) {
      jshPushIOWatchEvent(EV_EXTI14);
}

static void exti15_isr(void) {
      jshPushIOWatchEvent(EV_EXTI15);
}

static isr_fn_t exti_isr[16] = {
    exti0_isr,
    exti1_isr,
    exti2_isr,
    exti3_isr,
    exti4_isr,
    exti5_isr,
    exti6_isr,
    exti7_isr,
    exti8_isr,
    exti9_isr,
    exti10_isr,
    exti11_isr,
    exti12_isr,
    exti13_isr,
    exti14_isr,
    exti15_isr,
};

isr_fn_t get_exti_isr(uint8_t pin_source)
{
    return exti_isr[pin_source];
}

