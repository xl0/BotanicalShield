/* Stolen from CitizenWatt sensor code */
void setup_watchdog(uint8_t prescalar)
{
	prescalar = min(9, prescalar);
	uint8_t wdtcsr = prescalar & 7;
	if (prescalar & 8)
		wdtcsr |= _BV(WDP3);

	MCUSR &= ~_BV(WDRF);
	WDTCSR = _BV(WDCE) | _BV(WDE);
	WDTCSR = _BV(WDCE) | wdtcsr | _BV(WDIE);
}
