#ifndef FLASHMEM_H
#define FLASHMEM_H

/*
* The first page of the memory will be used to store meta data
*/

#include<string.h>

//#define DEBUG_FLASH

#define CHAR_MODE 0
#define BYTE_MODE 1

char itoa_buff[20];
int16_t light_value;
int temperature;
void divide_int_print(int16_t value)
{
	serial_print_char(value & 255);
	//serial_print_char(' ');
	serial_print_char((value >> 8) & 255);
	//serial_print_char(' ');
}

void print_accelgyro(int mode)
{
	if (mode == CHAR_MODE) {
		serial_print_str(itoa(accelgyro.ax, itoa_buff, 10));
		serial_print_char(' ');
		serial_print_str(itoa(accelgyro.ay, itoa_buff, 10));
		serial_print_char(' ');
		serial_print_str(itoa(accelgyro.az, itoa_buff, 10));
		serial_print_char(' ');
		serial_print_str(itoa(accelgyro.gx, itoa_buff, 10));
		serial_print_char(' ');
		serial_print_str(itoa(accelgyro.gy, itoa_buff, 10));
		serial_print_char(' ');
		serial_println_str(itoa(accelgyro.gz, itoa_buff, 10));
	} else if (mode == BYTE_MODE) {
		divide_int_print(accelgyro.ax);
		divide_int_print(accelgyro.ay);
		divide_int_print(accelgyro.az);
		divide_int_print(accelgyro.gx);
		divide_int_print(accelgyro.gy);
		divide_int_print(accelgyro.gz);
		serial_print_char('\n');
	}
}

void print_botanical(int mode)
{
	sensors_event_t event;
	tsl.getEvent(&event);
	/* serial_print_str(itoa(event.light,itoa_buff,10));
	   serial_print_char(' ');
	   int c=tempsensor.readTempC();
	   serial_print_str(itoa(c,itoa_buff,10));
	   serial_print_char(' '); */
	/*serial_print_str(itoa(accelgyro.ax,itoa_buff,10));
	   serial_print_char(' ');
	   serial_print_str(itoa(accelgyro.ay,itoa_buff,10));
	   serial_print_char(' ');
	   serial_print_str(itoa(accelgyro.az,itoa_buff,10));
	   serial_print_char(' ');
	   serial_print_str(itoa(accelgyro.gx,itoa_buff,10));
	   serial_print_char(' ');
	   serial_print_str(itoa(accelgyro.gy,itoa_buff,10));
	   serial_print_char(' ');
	   serial_println_str(itoa(accelgyro.gz,itoa_buff,10)); */
	temperature = tempsensor.readTempC();
	divide_int_print(event.light);
	divide_int_print(temperature);
	//divide_int_print(accelgyro.az);
	//divide_int_print(accelgyro.gx);
	//divide_int_print(accelgyro.gy);
	//divide_int_print(accelgyro.gz);
	//byte test=2014;
	//  Serial.print(test);
	// Serial.print(test,DEC);  
	//serial_print_str(test);
	serial_print_char('\n');
}

#define FLASH_MAGIC 2765

struct FlashMem {
	DataFlash dataflash;
	int16_t page;
	int16_t offset;
	int16_t n;
	int16_t buffered_bytes;
	int16_t sampling;
	char gyro_scale;
	char acce_scale;
	byte Hour;
	byte Minute;
	byte Second;
	byte Day;
	byte DayofWeek;		// Sunday is day 0 
	byte Month;		// Jan is month 0
	byte Year;		// the Year minus 1900  
} flashMem;


#ifdef DEBUG_FLASH
inline uint16_t FLASH_READ_INT16(void)
{
	uint16_t value;
	value = SPI.transfer(0xff);
	value = value << 8;
	value += SPI.transfer(0xff);
	Serial1.print(">>");
	Serial1.println(value);
	Serial1.flush();
	return value;
}

inline void FLASH_WRITE_INT16(uint16_t value)
{
	Serial1.print("<<");
	Serial1.println(value);
	Serial1.flush();
	SPI.transfer((value >> 8) & 0xff);
	SPI.transfer(value & 0xff);
}

inline uint8_t FLASH_READ_INT8(void)
{
	uint8_t value;
	value = SPI.transfer(0xff);
	Serial1.print(">");
	Serial1.println(value);
	Serial1.flush();
}

inline void FLASH_WRITE_INT8(uint8_t value)
{
	Serial1.print("<<");
	Serial1.println(value);
	Serial1.flush();
	SPI.transfer((uint8_t) value);
}

#else 
inline uint16_t FLASH_READ_INT16(void)
{
	uint16_t value;
	value = SPI.transfer(0xff);
	value = value << 8;
	value += SPI.transfer(0xff);
	return value;
}

inline void FLASH_WRITE_INT16(uint16_t value)
{
	SPI.transfer((value >> 8) & 0xff);
	SPI.transfer(value & 0xff);
}

inline uint8_t FLASH_READ_INT8(void)
{
	uint8_t value;
	value = SPI.transfer(0xff);
}

inline void FLASH_WRITE_INT8(uint8_t value)
{
	SPI.transfer((uint8_t) value);
}
#endif


void flash_write_meta_data()
{
//	flashMem.dataflash.pageToBuffer(0, 0);
	flashMem.dataflash.bufferWrite(0, 0);
	FLASH_WRITE_INT16(FLASH_MAGIC);
	FLASH_WRITE_INT16(flashMem.n);
	FLASH_WRITE_INT16(flashMem.sampling);
	FLASH_WRITE_INT8(flashMem.acce_scale);
	FLASH_WRITE_INT8(flashMem.gyro_scale);
	//added
	FLASH_WRITE_INT8(flashMem.Hour);
	FLASH_WRITE_INT8(flashMem.Minute);
	FLASH_WRITE_INT8(flashMem.Second);
	FLASH_WRITE_INT8(flashMem.Day);
	FLASH_WRITE_INT8(flashMem.DayofWeek);	// Sunday is day 0 
	FLASH_WRITE_INT8(flashMem.Month);	// Jan is month 0
	FLASH_WRITE_INT8(flashMem.Year);	// the Year minus 1900 
	flashMem.dataflash.bufferToPage(0, 0);
}

void flash_write_config(char acce_scale, char gyro_scale, int sampling)
{
	flashMem.dataflash.pageToBuffer(0, 0);
	flashMem.dataflash.bufferWrite(0, 8);
	FLASH_WRITE_INT16(FLASH_MAGIC);
	FLASH_WRITE_INT16(sampling);
	FLASH_WRITE_INT8(acce_scale);
	FLASH_WRITE_INT8(gyro_scale);
	//added
	FLASH_WRITE_INT8(flashMem.Hour);
	FLASH_WRITE_INT8(flashMem.Minute);
	FLASH_WRITE_INT8(flashMem.Second);
	FLASH_WRITE_INT8(flashMem.Day);
	FLASH_WRITE_INT8(flashMem.DayofWeek);	// Sunday is day 0
	FLASH_WRITE_INT8(flashMem.Month);	// Jan is month 0
	FLASH_WRITE_INT8(flashMem.Year);	// the Year minus 1900
	flashMem.dataflash.bufferToPage(0, 0);
}

int flash_read_meta_data(void)
{
	int16_t magic_value;
	flashMem.dataflash.pageToBuffer(0, 0);
	flashMem.dataflash.bufferRead(0, 0);

	magic_value = FLASH_READ_INT16();
	flashMem.n = FLASH_READ_INT16();
	flashMem.sampling = FLASH_READ_INT16();
	flashMem.acce_scale = FLASH_READ_INT8();
	flashMem.gyro_scale = FLASH_READ_INT8();
	//added
	flashMem.Hour = FLASH_READ_INT8();
	flashMem.Minute = FLASH_READ_INT8();
	flashMem.Second = FLASH_READ_INT8();
	flashMem.Day = FLASH_READ_INT8();
	flashMem.DayofWeek = FLASH_READ_INT8();
	flashMem.Month = FLASH_READ_INT8();
	flashMem.Year = FLASH_READ_INT8();
	//flashMem.Year= FLASH_READ_INT8();
	if (magic_value != FLASH_MAGIC)
		return 1;
	else
		return 0;
}

int flash_read_config(char *acce_scale, char *gyro_scale, int *sampling)
{
	int16_t magic_value;
	flashMem.dataflash.pageToBuffer(0, 0);
	flashMem.dataflash.bufferRead(0, 8);
	magic_value = FLASH_READ_INT16();
	*sampling = FLASH_READ_INT16();
	*acce_scale = FLASH_READ_INT8();
	*gyro_scale = FLASH_READ_INT8();
	flashMem.dataflash.bufferToPage(0, 0);
	if (magic_value != FLASH_MAGIC)
		return 1;
	else
		return 0;
}

int flash_setup(int cs_pin, int reset_pin, int wp_pin)
{
	//Start SPI for the external flash
	SPI.begin();
	flashMem.dataflash.setup(cs_pin, reset_pin, wp_pin);
	delay(10);
	flashMem.dataflash.autoErase();

	if (flash_read_meta_data()) {
		Serial.println("M problem to read memory");
		flash_write_meta_data();
		return 1;
	}
	return 0;
}

void flash_erase(void)
{
	flashMem.page = 1;
	flashMem.offset = 0;
	flashMem.n = 0;
	flash_write_meta_data();
}

void flash_write_mode_start(void)
{
	flashMem.sampling = accelgyro.sampling_rate;
	flashMem.acce_scale = accelgyro.acc_range;
	flashMem.gyro_scale = accelgyro.gyro_range;
	//time data
	flashMem.Hour = 21;
	flashMem.Minute = 45;
	flashMem.Second = 0;
	flashMem.Day = 15;
	flashMem.DayofWeek = 6;	// Sunday is day 0 
	flashMem.Month = 11;	// Jan is month 0
	flashMem.Year = 114;	// the Year minus 1900 
	//
	flash_erase();
//	flashMem.dataflash.bufferWrite(0, 0);
	flashMem.offset = 0;
}

void flash_write_buffer(void)
{
	flashMem.dataflash.bufferToPage(0, flashMem.page);
	flashMem.page++;
	flashMem.n += (flashMem.offset / 12);
	flashMem.offset = 0;
	flash_write_meta_data();
}

void flash_write_accelgyro(void)
{
	//print_accelgyro(0);
	FLASH_WRITE_INT16(accelgyro.ax);
	FLASH_WRITE_INT16(accelgyro.ay);
	FLASH_WRITE_INT16(accelgyro.az);
	FLASH_WRITE_INT16(accelgyro.gx);
	FLASH_WRITE_INT16(accelgyro.gy);
	FLASH_WRITE_INT16(accelgyro.gz);
	flashMem.offset += 12;
	if (flashMem.offset == 528)
		flash_write_buffer();
}

void flash_write_botanical(void)
{
	tsl.getEvent(&event);
	temperature = tempsensor.readTempC();
	flashMem.dataflash.bufferWrite(0, flashMem.offset);
	FLASH_WRITE_INT16((uint16_t) event.light);
	FLASH_WRITE_INT8(temperature);
	//dummy value to get even numb of bits
	FLASH_WRITE_INT8(0);
	flashMem.offset += 4;
	if (flashMem.offset == 528)
		flash_write_buffer();
	flashMem.dataflash.disable();
}

void read_botanical(void)
{
	light_value = FLASH_READ_INT16();
	temperature = FLASH_READ_INT8();
	FLASH_READ_INT8();
}

void read_accelgyro(void)
{
	accelgyro.ax = FLASH_READ_INT16();
	accelgyro.ay = FLASH_READ_INT16();
	accelgyro.az = FLASH_READ_INT16();
	accelgyro.gx = FLASH_READ_INT16();
	accelgyro.gy = FLASH_READ_INT16();
	accelgyro.gz = FLASH_READ_INT16();
}

void flash_read_accelgyro(int mode)
{
	if (flash_read_meta_data()) {
		Serial.println("M corrupted memory");
		return;
	}
	serial_print_char('i');
	serial_print_char(' ');
	serial_print_int((int)flashMem.acce_scale);
	serial_print_char(' ');
	serial_print_int((int)flashMem.gyro_scale);
	serial_print_char(' ');
	serial_print_int(flashMem.sampling);
	serial_print_char(' ');
	serial_print_int(flashMem.Hour);
	serial_print_char(' ');
	serial_print_int(flashMem.Minute);
	serial_print_char(' ');
	serial_print_int(flashMem.Second);
	serial_print_char(' ');
	serial_print_int(flashMem.Day);
	serial_print_char(' ');
	serial_print_int(flashMem.DayofWeek);
	serial_print_char(' ');
	serial_print_int(flashMem.Month);
	serial_print_char(' ');
	serial_println_int(flashMem.Year);
	int i, j;
	flashMem.page = (flashMem.n * 12) / 528;

	//start from page 1 because page 0 is used to store variables
	for (i = 1; i <= flashMem.page; i++) {
		flashMem.dataflash.pageToBuffer(i, 0);
		flashMem.dataflash.bufferRead(0, 0);
		for (j = 0; j < 528; j += 12) {
			read_accelgyro();
			if (mode == CHAR_MODE)
				serial_print_str("r ");
			else
				serial_print_str("R ");
			print_accelgyro(mode);
			if (used_serial)
				delay(30);
		}
	}
	flashMem.dataflash.pageToBuffer(i, 0);
	flashMem.dataflash.bufferRead(0, 0);
	j = (flashMem.n * 12) % 528;
	for (i = 1; i < j; i += 12) {
		if (mode == CHAR_MODE)
			serial_print_str("r ");
		else
			serial_print_str("R ");
		read_accelgyro();
		print_accelgyro(mode);
		delay(10);
	}
}

void flash_read_botanical(int mode)
{
	if (flash_read_meta_data()) {
		Serial.println("M corrupted memory");
		return;
	}
	serial_print_char('i');
	serial_print_char(' ');
	serial_print_int((int)flashMem.acce_scale);
	serial_print_char(' ');
	serial_print_int((int)flashMem.gyro_scale);
	serial_print_char(' ');
	serial_print_int(flashMem.sampling);
	serial_print_char(' ');
	serial_print_int(flashMem.Hour);
	serial_print_char(' ');
	serial_print_int(flashMem.Minute);
	serial_print_char(' ');
	serial_print_int(flashMem.Second);
	serial_print_char(' ');
	serial_print_int(flashMem.Day);
	serial_print_char(' ');
	serial_print_int(flashMem.DayofWeek);
	serial_print_char(' ');
	serial_print_int(flashMem.Month);
	serial_print_char(' ');
	serial_println_int(flashMem.Year);
	int i, j;
	flashMem.page = (flashMem.n * 4) / 528;

	//start from page 1 because page 0 is used to store variables
	for (i = 1; i <= flashMem.page; i++) {
		flashMem.dataflash.pageToBuffer(i, 0);
		flashMem.dataflash.bufferRead(0, 0);
		for (j = 0; j < 528; j += 4) {
			read_botanical();
			if (mode == CHAR_MODE)
				serial_print_str("b ");
			else
				serial_print_str("B ");
			print_botanical(mode);
			if (used_serial)
				delay(30);
		}
	}
	flashMem.dataflash.pageToBuffer(i, 0);
	flashMem.dataflash.bufferRead(0, 0);
	j = (flashMem.n * 4) % 528;
	for (i = 1; i < j; i += 4) {
		if (mode == CHAR_MODE)
			serial_print_str("b ");
		else
			serial_print_str("B ");
		read_botanical();
		print_botanical(mode);
		delay(10);
	}
}
#endif
