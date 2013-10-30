#ifndef HEADER_H_
#define HEADER_H_

#define F_CPU 20000000

#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h> //SDA is also known as Two Wire Interface, TWI.
#include <stdio.h>

void TWI_init_master(void);	//Initializes the I2C as a master device
void TWI_start(void);	//Sends a start condition. Check Sparkfun's explanation on the timing diagram to better understand the protocol: https://learn.sparkfun.com/tutorials/i2c/protocol
void TWI_write(char data);	//Requests to write to the slave device.
char TWI_read_data(void);		//Requests to read from the slave device.
void TWI_stop(void);

void USARTInit(unsigned int ubrr_value);        //Initializes the USART with the standard protocol required by the GPS.
char USARTReadChar(void);       //Reads a character from the buffer of the USART, which was received through the RX pin. Note that when you read the buffer it gets cleared and the next character received is written to it.
void USARTWriteChar(char data);        //Writes a character to the USART send buffer. The character will get sent as soon as possible.

static FILE mystdout = FDEV_SETUP_STREAM(USARTWriteChar, NULL, _FDEV_SETUP_WRITE); //Don't worry too much about this line. It is needed for the printf function. It is basically declaring a stream for the printf to use, which just uses the USARTWriteChar function.


int X_AxisRead(void);
int Y_AxisRead(void);

void GyroInit(void);

//Definitions exclusive to the GY

#define SDA_GY_WRITE 0xd6

#define SDA_GY_READ 0xd7

#define GY_CTRL_REG_1 0x20

#define X_AXIS_OUT_LOW 0x28
#define X_AXIS_OUT_HIGH 0x29

#define Y_AXIS_OUT_LOW 0x2A
#define Y_AXIS_OUT_HIGH 0x2B

#define Z_AXIS_OUT_LOW 0x2C
#define Z_AXIS_OUT_HIGH 0x2D

#endif /* HEADER_H_ */