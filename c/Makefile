DIR_OBJ = ./lib
DIR_BIN = ./bin
DIR_Config = ./lib/Config
DIR_MotorDriver = ./lib/MotorDriver
DIR_PCA9685 = ./lib/PCA9685
DIR_Examples = ./

OBJ_C = $(wildcard ${DIR_OBJ}/*.c ${DIR_Examples}/*.c ${DIR_Config}/*.c ${DIR_MotorDriver}/*.c ${DIR_PCA9685}/*.c )
OBJ_O = $(patsubst %.c,${DIR_BIN}/%.o,$(notdir ${OBJ_C}))

TARGET = assignment3

CC = gcc

DEBUG = -g -O0 -Wall
CFLAGS += $(DEBUG)

USELIB = USE_DEV_LIB
DEBUG = -D $(USELIB) 
ifeq ($(USELIB), USE_BCM2835_LIB)
    LIB = -lbcm2835 -lm -pthread 
else ifeq ($(USELIB), USE_WIRINGPI_LIB)
    LIB = -lwiringPi -lm 
endif

# New prepare target to ensure the bin directory exists
prepare:
	mkdir -p $(DIR_BIN)

# Ensure main target depends on prepare
${TARGET}: prepare ${OBJ_O}
	$(CC) $(CFLAGS) $(OBJ_O) -o $@ $(LIB) -lm -lpigpio -lrt -pthread

${DIR_BIN}/%.o : $(DIR_Examples)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB) -I $(DIR_OBJ) -I $(DIR_Config) -I $(DIR_MotorDriver) -I $(DIR_PCA9685)

${DIR_BIN}/%.o : $(DIR_OBJ)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB) -I $(DIR_Config)
    
${DIR_BIN}/%.o : $(DIR_Config)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB)

${DIR_BIN}/%.o : $(DIR_PCA9685)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB) -I $(DIR_Config)

${DIR_BIN}/%.o : $(DIR_MotorDriver)/%.c
	$(CC) $(CFLAGS) -c  $< -o $@ $(LIB) -I $(DIR_Config) -I $(DIR_PCA9685)

run: ${TARGET}
	sudo ./$(TARGET)

clean :
	rm -rf $(DIR_BIN)/*.*
	rm -f $(TARGET)
