#include "header.h"


/*
 Read degrees per second
 
*/
int main(void)
{
    
	int x,y,z;
    USARTInit(129);
	TWI_init_master();
	GyroInit();
	
    while(1)
    {
        x = X_AxisRead();
        y = Y_AxisRead();
        z = Z_AxisRead();
        
        printf("X: %d Y: %d Z: %d\r\n", x, y, z);
        _delay_ms(100);
    }
}

int X_AxisRead() {
	char xh=0;
	char xl=0;
	int x=0;
	
    TWI_start();
    TWI_write(SDA_GY_WRITE);
    TWI_write(X_AXIS_OUT_LOW);
    TWI_start();					//This is an example of a repeated start since we never send a stop condition.
    TWI_write(SDA_GY_READ);	//Notice how the transmission goes. We gave the accel the address for which we wanted access to but since we want to read it we need to start again and send a read request.
    xl = TWI_read_data();			//The accel will send us the value of the register so we write it to our 8-bit variable x low.
    TWI_stop();
    //Much cleaner with the #defines in place huh?
    TWI_start();
    TWI_write(SDA_GY_WRITE);
    TWI_write(X_AXIS_OUT_HIGH);
    TWI_start();
    TWI_write(SDA_GY_READ);
    xh = TWI_read_data();
    TWI_stop();
    
    x = xh<<8 | xl;
    x = x*8.75;
    
    return x;
}

int Y_AxisRead() {
    char yh=0;
    char yl=0;
    int y=0;
    
    TWI_start();
    TWI_write(SDA_GY_WRITE);
    TWI_write(Y_AXIS_OUT_LOW);
    TWI_start();					//This is an example of a repeated start since we never send a stop condition.
    TWI_write(SDA_GY_READ);	//Notice how the transmission goes. We gave the accel the address for which we wanted access to but since we want to read it we need to start again and send a read request.
    yl = TWI_read_data();			//The accel will send us the value of the register so we write it to our 8-bit variable x low.
    TWI_stop();
    //Much cleaner with the #defines in place huh?
    TWI_start();
    TWI_write(SDA_GY_WRITE);
    TWI_write(Y_AXIS_OUT_HIGH);
    TWI_start();
    TWI_write(SDA_GY_READ);
    yh = TWI_read_data();
    TWI_stop();
    
    y = yh<<8 | yl;
    y = y*8.75;
    
    return y;
}


int Z_AxisRead() {
    char zh=0;
    char zl=0;
    int z=0;
    
    TWI_start();
    TWI_write(SDA_GY_WRITE);
    TWI_write(Z_AXIS_OUT_LOW);
    TWI_start();
    TWI_write(SDA_GY_READ);
    zl = TWI_read_data();
    TWI_stop();
    TWI_start();
    TWI_write(SDA_GY_WRITE);
    TWI_write(Z_AXIS_OUT_HIGH);
    TWI_start();
    TWI_write(SDA_GY_READ);
    zh = TWI_read_data();
    TWI_stop();
    
    z = zh<<8 | zl;
    z = z*8.75;
    
    return z;
}


void GyroInit(void){
    TWI_start();
    TWI_write(SDA_GY_WRITE);	//Then we need to talk specifically to our accelerometer device. We do that by writing it's 7-bit address on the SDA line, which is 0011001. Now if we want to write to it we add a 0 at the end or a 1 to read, making it an 8-bit number.
    //In this case we want to write to it. Check table 11 on page 19 of the datasheet.
    TWI_write(GY_CTRL_REG_1);	//Then we send the address of the register we want to write to. In our case that is 0b0100000, control register #1.
    TWI_write(0b00001111);	//Finally we write what we want to that address, which we discussed above to turn on the device, namely 0b01010111.
    TWI_stop();
}

void TWI_init_master(void) // Function to initialize master
{
	TWBR=0x5c;    // Set bit rate
	TWSR=(0<<TWPS1)|(0<<TWPS0);    // Set prescalar bits
	// SCL freq= F_CPU/(16+2(TWBR).(TWPS)) This gives us a frequency of 20MHz/200=100KHz which is within the capabilities of the Accel.
}

void TWI_start(void)
{
	// Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while((TWCR & (1<<TWINT)) == 0); // Wait till start condition is transmitted. This is important or else later we could end up trying to transmit again while this transmission hadn't ended... a collision of data would happen.
}

void TWI_write(char data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
}


char TWI_read_data(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while ((TWCR & (1<<TWINT)) == 0);
	return TWDR;
}

void TWI_stop(void)
{
	// Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	TWCR= (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while(!(TWCR & (1<<TWSTO)));  // Wait till stop condition is transmitted
}

void USARTInit(uint16_t ubrr_value){
    
    //Set Baud rate. Again, the ATmega datasheet has a sample baud rate table for ATmega's running at several frequencies and what UBBR value should be. For asynchronous serial Baud rate=fosc/16(UBBR + 1). For UBBR=21 and fosc=20Mhz, we get 56818 bps... pretty close.
    
    UBRR0L = ubrr_value;
    UBRR0H = (ubrr_value>>8);
    
    /*Set Frame Format. This is the default for Asynchronous serial and it's the protocol the GPS uses.
     
     
     >> Asynchronous mode
     >> No Parity
     >> 1 StopBit
     
     >> char size 8
     */
    
    UCSR0C= (1<<UCSZ00)|(1<<UCSZ01); //8-bit frame
    
    
    UCSR0B=(1<<RXEN0)|(1<<TXEN0); //Enable The receiver and transmitter
    
    stdout = &mystdout; //Sets the stream we created at the top of the code to be the standard stream out. So by default printf function will send chars to this string.
    
}

char USARTReadChar()
{
    //Wait until data is available
    
    while(!(UCSR0A & (1<<RXC0)))
    {
        //Do nothing
    }
    
    //Now USART has got data from device and is available inside the buffer.
    
    return UDR0;
}

void USARTWriteChar(char data)
{
    //Wait until the transmitter is ready. (This is important if, for example, the USART was sending data out from the buffer... we can't just write the buffer in the middle of a transmission.)
    
    while(!(UCSR0A & (1<<UDRE0)))
    {
        //Do nothing
    }
    
    //Now write the data to USART buffer
    
    UDR0=data;
}