#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 8UL))) - 1)

void uart_init(void);
unsigned char uart_getc(void);
void uart_putc( unsigned char data);
void uart_puts(const char* s);
void uart_puts_P(const char* s);
void uart_putn(uint16_t n);
void uart_puth(uint8_t *buf, uint16_t len);
void uart_putb(uint8_t *buf, uint16_t len);

void uart_putb(uint8_t *b, uint16_t len){
	char cbuf[8];
	for (; len > 0; len--){	
		itoa(*b++, cbuf, 2);
		uart_puts(cbuf);
		uart_putc(' '); 
	}
}

void uart_puth(uint8_t *b, uint16_t len){
	char cbuf[3];
	for (; len > 0; len--){	
		itoa(*b++, cbuf, 16);
		uart_puts(cbuf);
		uart_putc(' ');
	}
}

void uart_putn(uint16_t n){
	char cbuf[6];
	itoa(n, cbuf, 10);
	uart_puts(cbuf);
}

void uart_init(void){
 UBRR0H = (uint8_t)(BAUD_PRESCALLER>>8);
 UBRR0L = (uint8_t)(BAUD_PRESCALLER);
 UCSR0B = (1<<RXEN0)|(1<<TXEN0);
 UCSR0C = (3<<UCSZ00);
}

unsigned char uart_getc(void){

 while(!(UCSR0A & (1<<RXC0)));
 return UDR0;

}

void uart_putc(unsigned char data)
{
     while(!(UCSR0A & (1<<UDRE0)));
     UDR0 = data;

}

void uart_puts(const char *s )
{
    while (*s) 
      uart_putc(*s++);

}



void uart_puts_P(const char *progmem_s )
{
    register char c;
    
    while ( (c = pgm_read_byte(progmem_s++)) ) 
      uart_putc(c);

}
