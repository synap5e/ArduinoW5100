avr-g++ -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o main.o main.cpp
avr-g++ -mmcu=atmega328p main.o -o main
avr-objcopy -O ihex -R .eeprom main main.hex

