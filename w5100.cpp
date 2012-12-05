#include <avr/io.h>
#include <avr/pgmspace.h>

#undef PROGMEM 
#define PROGMEM __attribute__(( section(".progmem.data") )) 

//ATMEGA328P 13.3.1 - Alternate Functions of Port B
#define SS (1<<2)	//Slave select
#define MOSI (1<<3)	//Master out slave in
#define MISO (1<<4)	//Master in slave out
#define SCK (1<<5)	//Clock

//ATMEGA328P 18.5.1 - SPCR (SPI Control Register)
#undef SPE
#undef MSTR
#define SPE (1<<6)	//SPI Enable
#define MSTR (1<<4)	//SPI Master mode
#define LSB_FIRST (1<<5)//LSB first

//ATMEGA328P 18.5.2 - SPSR (SPI Status Register)
#undef SPIF
#define SPIF 1<<7	//Interupt, set to 1 after successfull SPI write

//W5100 6.3.2 - Commands - OP Codes
#define WRITE_OP 0xF0	//Read
#define READ_OP 0x0F	//Write

//W5100 3.1 - Common registers
#define MR 0x0000	// Mode				l = 1
#define GAR 0x0001	// Gateway address		l = 4
#define SUBR 0x0005	// Subnet mask			l = 4
#define SHAR 0x0009	// Src hware addr		l = 6
#define SIPR 0x000F	// Src ip addr			l = 4
#define IR 0x0015	// Interupt			l = 1
#define IMR 0x0016	// Interupt mask		l = 1
#define RTE 0x0017	// Retry time			l = 2
#define RCR 0x0019	// Retry count			l = 1 
#define RMSR 0x001A	// RX memory size		l = 1
#define TMSR 0x001B	// TX memory size		l = 1
#define PATR 0x001C	// PPPoE authentication type	l = 2
#define PTIMER 0x0028 	// PPP LCP Request Timer	l = 1
#define PMAGIC 0x0029 	// PPP LCP Magic number		l = 1
#define UIPR 0x002A 	// Unreachable IP addr 		l = 4
#define UPORT 0x002E 	// Unreachable port		l = 2

void setMR(uint8_t value);
void setGAR(uint8_t* buf);
void setSUBR(uint8_t* buf);
void setSHAR(uint8_t* buf);
void setSIPR(uint8_t* buf);
void setIR(uint8_t value);
void setIMR(uint8_t value);
void setRTE(uint8_t* buf);
void setRCR(uint8_t value);
void setRMSR(uint8_t value);
void setTMSR(uint8_t value);
void setPATR(uint8_t* buf);
void setPTIMER(uint8_t value);
void setPMAGIC(uint8_t value);
void setUIPR(uint8_t* buf);
void setUPORT(uint8_t* buf);


//W5100 3.2 - Socket registers
//To access a socket (Sn)'s register add Sn to the register
//e.g. S3_TOS = S3 | Sn_TOS 
#define S0 0x0400	// Socket 0
#define S1 0x0500	// Socket 1
#define S2 0x0600	// Socket 2
#define S3 0x0700	// Socket 3

#define Sn_MR 0x00	// Mode				l = 1
#define Sn_CR 0x01	// Command			l = 1
#define Sn_IR 0x02	// Interrupt			l = 1
#define Sn_SR 0x03	// Status			l = 1
#define Sn_PORT	0x04	// Src port			l = 2
#define Sn_DHAR	0x06	// Dst hware addr		l = 6
#define Sn_DIPR	0x0C	// Dst ip addr			l = 4
#define Sn_DPORT 0x10	// Dst port			l = 2
#define Sn_MSSR 0x12	// Maximum segment size		l = 2
#define Sn_PROTO 0x14	// Protocol for IP Raw		l = 1
#define Sn_TOS 0x15	// IP TOS			l = 1
#define Sn_TTL 0x16	// IP TTL			l = 1
#define Sn_TX_FSR	// TX Free size			l = 2
#define Sn_TX_RD	// TX Read pointer		l = 2
#define Sn_TX_WR	// TX Write pointer		l = 2
#define Sn_RX_RSR	// RX Received size		l = 2
#define Sn_RX_WR	// RX Read pointer		l = 2

#define slaveOn() DDRB |= SS; PORTB &= ~SS
#define slaveOff() DDRB |= SS; PORTB |= SS 


const uint8_t sockets_PGM [] PROGMEM = {0x04, 0x05, 0x06, 0x07};
#define socketNumberToAddress(P) ( pgm_read_byte( sockets_PGM + (P) ) )


void startSPI(void);
void write(uint16_t addr, uint8_t data);
uint8_t read(uint16_t addr);
uint16_t write(uint16_t addr, uint8_t *buf, uint16_t len);
uint16_t read(uint16_t addr, uint8_t *buf, uint16_t len);
void write(uint8_t addr1, uint8_t addr2, uint8_t data);
uint8_t read(uint8_t addr1, uint8_t addr2);
uint16_t write(uint8_t addr1, uint8_t addr2, uint8_t *buf, uint8_t len);
uint16_t read(uint8_t addr1, uint8_t addr2, uint8_t *buf, uint8_t len);
uint8_t transfer(uint8_t data);

void startSPI(void){
	DDRB |= MOSI | SCK | SS; 	//Set outputs
	DDRB &= ~MISO;		 	//Set input
	PORTB |= SS;			//Set SS high - SPI OFF

	SPCR |= SPE | MSTR;		//SPI enable + SPI master mode
	SPCR &= ~LSB_FIRST; 		//NOT LSB_FIRST - HSB first
}

void setMR(uint8_t value)
{
	write(MR, value);
}

void setGAR(uint8_t* buf)
{
	write(GAR, buf, 4);
}

void setSUBR(uint8_t* buf)
{
	write(SUBR, buf, 4);
}

void setSHAR(uint8_t* buf)
{
	write(SHAR, buf, 6);
}

void setSIPR(uint8_t* buf)
{
	write(SIPR, buf, 4);
}

void setRTE(uint16_t time){
	uint8_t val[] = {time >> 8, time & 0xFF};
	write(RTE, val, 2);
}

void setRCR(uint8_t value)
{
	write(RCR, value);
}

void setRMSR(uint8_t value)
{
	write(RMSR, value);
}

void setTMSR(uint8_t value)
{
	write(TMSR, value);
}

void setPTIMER(uint8_t value)
{
	write(PTIMER, value);
}

void setPMAGIC(uint8_t value)
{
	write(PMAGIC, value);
}


void setUPORT(uint8_t* buf)
{
	write(UPORT, buf, 2);
}

//Interupts and getters

uint8_t getIR(void){
	read(IR);
}

void setIRM(uint8_t value){
	write(IRM, value);
}

void getPATR(void){
	uint8_t val[2];
	read(PATR, val, 2);
	return (val[0] << 8) | val[1]
}

void getUIPR(uint8_t* ip){
	read(UIPR, ip, 4);
}

uint16_t getUPORT(void){
	uint8_t val[2];
	read(UPORT, val, 2);
	return (val[0] << 8) | val[1]
}




// When specifying each half of the address seperately, only addr2 can increment.
// To reflet this len is a uint8_t
uint16_t write(uint8_t addr1, uint8_t addr2, uint8_t *buf, uint8_t len){
	slaveOn();
	uint16_t written;
	for (written = 0; written < len; written++){
		transfer(0xF0);
		transfer(addr1);
		transfer(addr2);
		transfer(buf[written]);
		addr2++;
	}
	slaveOff();
	return written;
}	

// When specifying each half of the address seperately, only addr2 can increment.
// To reflet this len is a uint8_t
uint16_t read(uint8_t addr1, uint8_t addr2, uint8_t *buf, uint8_t len)
{
	slaveOn();
	uint16_t read;
	for (read = 0; read < len; read++){
		transfer(0x0F);
		transfer(addr1);
		transfer(addr2);
		buf[read] = transfer(0);
		addr2++;
	}
	slaveOff();
	return read;
}


uint16_t write(uint16_t addr, uint8_t *buf, uint16_t len){
	slaveOn();
	uint16_t written;
	for (written = 0; written < len; written++){
		transfer(0xF0);
		transfer(addr >> 8);
		transfer(addr & 0xFF);
		transfer(buf[written]);
		addr++;
	}
	slaveOff();
	return written;
}	

uint16_t read(uint16_t addr, uint8_t *buf, uint16_t len)
{
	slaveOn();
	uint16_t read;
	for (read = 0; read < len; read++){
		transfer(0x0F);
		transfer(addr >> 8);
		transfer(addr & 0xFF);
		buf[read] = transfer(0);
		addr++;
	}
	slaveOff();
	return read;
}

void write(uint16_t addr, uint8_t data){
	slaveOn();
	transfer(WRITE_OP);
	transfer(addr>>8);
	transfer(addr & 0xFF);
	transfer(data);
	slaveOff();
}
uint8_t read(uint16_t addr){
	slaveOn();
	transfer(WRITE_OP);
	transfer(addr>>8);
	transfer(addr & 0xFF);
	uint8_t val = transfer(0x00);
	slaveOff();
	return val;
}

void write(uint8_t addr1, uint8_t addr2, uint8_t data){
	slaveOn();
	transfer(WRITE_OP);
	transfer(addr1);
	transfer(addr2);
	transfer(data);
	slaveOff();
}
uint8_t read(uint8_t addr1, uint8_t addr2){
	slaveOn();
	transfer(WRITE_OP);
	transfer(addr1);
	transfer(addr2);
	uint8_t val = transfer(0x00);
	slaveOff();
	return val;
}

uint8_t transfer(uint8_t data){
	SPDR = data;
	while (!(SPSR & SPIF));
	return SPDR;
}

