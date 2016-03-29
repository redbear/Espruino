#include "wiring.h"

/* See jspininfo.c, 0xFF means invalid on Duo */
static const Pin halpin[JSH_PIN_COUNT] = {
/*PORTA*/  D15,D14,D9,D8,D10,D11,D12,D13,0xFF,D17,D16,0xFF,0xFF,D7,D6,D5,
/*PORTB*/  RGBB,RGBG,BTN,D4,D3,D2,D1,D0,0xFF,0xFF,0xFF,RGBR,0xFF,0xFF,0xFF,0xFF,
/*PORTC*/  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
/*PORTD*/  0xFF,
/*PORTH*/  0xFF,0xFF
};

void js_setup() {
  jshInit();
  jsvInit();
  jsiInit(true);
}

void js_loop() {
  jsiLoop();

  // js*Kill()
}


PinMode jspinmode_to_hal(JshPinState state)
{
  if(state == JSHPINSTATE_GPIO_OUT) return AF_OUTPUT_PUSHPULL;
  if(state == JSHPINSTATE_GPIO_OUT_OPENDRAIN) return AF_OUTPUT_DRAIN;
  if(state == JSHPINSTATE_GPIO_IN) return INPUT;
  if(state == JSHPINSTATE_GPIO_IN_PULLUP) return INPUT_PULLUP;
  if(state == JSHPINSTATE_GPIO_IN_PULLDOWN) return INPUT_PULLDOWN;
  if(state == JSHPINSTATE_ADC_IN) return AN_INPUT;
  if(state == JSHPINSTATE_DAC_OUT) return AN_OUTPUT;

  return PIN_MODE_NONE;
}

Pin jspin_to_hal(Pin pin)
{
  return halpin[pin];
}

