if [ $1 ] 
then
	p="$1"
else
	p="ACM0"
fi
echo avrdude -F -V -c arduino -p ATMEGA328P -P /dev/tty$p -b 115200 -U flash:w:main.hex
avrdude -F -V -c arduino -p ATMEGA328P -P /dev/tty$p -b 115200 -U flash:w:main.hex
