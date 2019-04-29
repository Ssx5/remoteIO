# build executable on typing make
TARGET_LDFLAGS:= -lpthread -lmosquitto -lmodbus -lm
LDFLAGS = -lpthread -lmosquitto -lmodbus -lm

remoteIO: remoteIO_main.o remoteIO_config.o remoteIO_modbus.o remoteIO_mqtt.o cJSON.o
	$(CC) $(LDFLAGS) -o $@ $^ 

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -I. -Iinclude -o $@ $^ -std=c99

clean:
	rm -f *.o remateIO
