#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart.c"
#include "w5100.cpp"


#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 

#undef PSTR 
#define PSTR(s) (__extension__({static prog_char __c[] PROGMEM = (s); &__c[0];})) 

#define halt() for(;;)

#define delay_ms(n) _delay_ms(n)

const uint16_t var [] PROGMEM = {1};

int main(void){
	

	uart_init();
	uart_puts_P(PSTR("\nETHERNET TEST > PROGRAM STARTED\n"));
	uart_puts_P(PSTR("UART initiated\n"));
	
	startSPI();

	uint16_t v  = socketNumberToAddress(2);
	uint8_t a[] = {v<<8, v|0xFF};
	uart_puth(a 	, 2);


	uart_puts_P(PSTR("\n\n"));
	uart_putn(pgm_read_byte(var) & 0xFF);
	halt();
	return 0;
}


