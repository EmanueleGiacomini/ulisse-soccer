#include "kinetis.h"

// Stealing this one from teensy's eeprom lib ;)

#define EEPROM_MAX  2048
#define EEPARTITION 0x03  // all 32K dataflash for EEPROM, none for Data
#define EEESPLIT    0x30  // must be 0x30 on these chips
#define E2END 0x7FF
#define EEPROM_SIZE 2048
#define EEESIZE 0x03

// Writing unaligned 16 or 32 bit data is handled automatically when
// this is defined, but at a cost of extra code size.  Without this,
// any unaligned write will cause a hard fault exception!  If you're
// absolutely sure all 16 and 32 bit writes will be aligned, you can
// remove the extra unnecessary code.
//


void EEPROM_init(void) {
  uint32_t count=0;
	uint16_t do_flash_cmd[] = {
		0xf06f, 0x037f, 0x7003, 0x7803,
		0xf013, 0x0f80, 0xd0fb, 0x4770};
	uint8_t status;
	if (FTFL_FCNFG & FTFL_FCNFG_RAMRDY) {
		uint8_t stat = FTFL_FSTAT & 0x70;
		if (stat) FTFL_FSTAT = stat;
		// FlexRAM is configured as traditional RAM
		// We need to reconfigure for EEPROM usage
		kinetis_hsrun_disable();
		FTFL_FCCOB0 = 0x80; // PGMPART = Program Partition Command
		FTFL_FCCOB3 = 0;
		FTFL_FCCOB4 = EEESPLIT | EEESIZE;
		FTFL_FCCOB5 = EEPARTITION;
		__disable_irq();
		// do_flash_cmd() must execute from RAM.  Luckily the C syntax is simple...
		(*((void (*)(volatile uint8_t *))((uint32_t)do_flash_cmd | 1)))(&FTFL_FSTAT);
		__enable_irq();
		kinetis_hsrun_enable();
		status = FTFL_FSTAT;
		if (status & 0x70) {
			FTFL_FSTAT = (status & 0x70);
			return; // error
		}
	}
	// wait for eeprom to become ready (is this really necessary?)
	while (!(FTFL_FCNFG & FTFL_FCNFG_EEERDY)) {
		if (++count > 200000) break;
	}
}

#define FlexRAM ((volatile uint8_t *)0x14000000)

static void flexram_wait(void)
{
	while (!(FTFL_FCNFG & FTFL_FCNFG_EEERDY)) {
		// TODO: timeout
	}
}

// reads in dest size bytes starting from eeprom src
void EEPROM_read(void* dest, const uint16_t src, uint16_t size) {
  uint32_t offset = (uint32_t)src;
	uint8_t *buf = (uint8_t *)dest;
	uint32_t end = offset + size;
	
	if (!(FTFL_FCNFG & FTFL_FCNFG_EEERDY)) EEPROM_init();
	if (end > EEPROM_SIZE) end = EEPROM_SIZE;
	while (offset < end) {
		*buf++ = FlexRAM[offset++];
	}
}

// writes starting from eeprom dest cell 'size' bytes from src
void EEPROM_write(uint16_t dest, const void* buf, uint16_t size) {
  uint32_t offset = (uint32_t)size;
	const uint8_t *src = (const uint8_t *)buf;

	if (offset >= EEPROM_SIZE) return;
	if (!(FTFL_FCNFG & FTFL_FCNFG_EEERDY)) EEPROM_init();
	if (size >= EEPROM_SIZE) size = EEPROM_SIZE;
	if (offset + size >= EEPROM_SIZE) size = EEPROM_SIZE - offset;

  while (size > 0) {
		uint32_t lsb = offset & 3;
		if (lsb == 0 && size >= 4) {
			// write aligned 32 bits
			uint32_t val32;
			val32 = *src++;
			val32 |= (*src++ << 8);
			val32 |= (*src++ << 16);
			val32 |= (*src++ << 24);
			if (*(uint32_t *)(&FlexRAM[offset]) != val32) {
				kinetis_hsrun_disable();
				uint8_t stat = FTFL_FSTAT & 0x70;
				if (stat) FTFL_FSTAT = stat;
				*(uint32_t *)(&FlexRAM[offset]) = val32;
				flexram_wait();
				kinetis_hsrun_enable();
			}
			offset += 4;
			size -= 4;
		} else if ((lsb == 0 || lsb == 2) && size >= 2) {
			// write aligned 16 bits
			uint16_t val16;
			val16 = *src++;
			val16 |= (*src++ << 8);
			if (*(uint16_t *)(&FlexRAM[offset]) != val16) {
				kinetis_hsrun_disable();
				uint8_t stat = FTFL_FSTAT & 0x70;
				if (stat) FTFL_FSTAT = stat;
				*(uint16_t *)(&FlexRAM[offset]) = val16;
				flexram_wait();
				kinetis_hsrun_enable();
			}
			offset += 2;
			size -= 2;
		} else {
			// write 8 bits
			uint8_t val8 = *src++;
			if (FlexRAM[offset] != val8) {
				kinetis_hsrun_disable();
				uint8_t stat = FTFL_FSTAT & 0x70;
				if (stat) FTFL_FSTAT = stat;
				FlexRAM[offset] = val8;
				flexram_wait();
				kinetis_hsrun_enable();
			}
			offset++;
			size--;
		}
	}
}