/*
 * Prescale values:
 * 0 2K (2048) cycles 16 ms
 * 1 4K (4096) cycles 32 ms
 * 2 8K (8192) cycles 64 ms
 * 3 16K (16384) cycles 0.125 s
 * 4 32K (32768) cycles 0.25 s
 * 5 64K (65536) cycles 0.5 s
 * 6 128K (131072) cycles 1.0 s
 * 7 256K (262144) cycles 2.0 s
 * 8 512K (524288) cycles 4.0 s
 * 9 1024K (1048576) cycles 8.0 s
 * Other - reserved.
 */

/* function originates in the CitizenWatt project */
void watchdog_setup(uint8_t prescale)
{
  prescale = min(9,prescale);

  /* 3 least significant bits of prescalar go to WDTCSR firectly,
   * first bit goes to WDP3 */
  uint8_t wdtcsr = prescale & ((1 << 0) | (1 << 1) | (1 << 2));
  if (prescale & (1 << 3))
    wdtcsr |= _BV(WDP3);

  /* allow watchdog configuration change */
  WDTCSR = _BV(WDCE) | _BV(WDE);
  /* Set prescale */
  WDTCSR = wdtcsr;
}

void watchdog_int_enable()
{
  uint8_t wdtcsr = WDTCSR;
  WDTCSR = _BV(WDCE) | _BV(WDE);
  /* clear the WD interrupt enable bit */
  WDTCSR = wdtcsr | _BV(WDIE);

}

void watchdog_int_disable()
{
  uint8_t wdtcsr = WDTCSR;
  WDTCSR = _BV(WDCE) | _BV(WDE);
  /* clear the WD interrupt enable bit */
  WDTCSR = wdtcsr & ~_BV(WDIE);
}


