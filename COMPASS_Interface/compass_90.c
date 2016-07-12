/*
 * compass.c
 *
 *  Created on: 15-Jan-2016
 *      Author: root
 */
/*

to get a heading out of the HMC6352 compass module with the Arduino using it's default I2C address, 0x42 .

STEP1 :-  set up unsigned char variable c to hold the command to send   c = 0x41; (u08 c=0;)

STEP2 :- set up  2 byte array in for the returned data  (u08 in[2];)

*/
#include <avr/io.h>
#include <util/delay.h>
#include "usart.h"
#define clk 0b01010101
#define anticlk 0b10101010
#define kp 20/10
int16_t compass_val();

void i2c_write(unsigned char data)
{
	TWDR=data;
	TWCR = (1 << TWINT) | (1<< TWEN);
	while((TWCR & (1 << TWINT) ) == 0); // when TWI hardware finishes its job , it sets TWINT bit to one
    // stays here till zero
	 //TWSR returns 0x18 for write
	 //TWSR returns 0x40 for read
	 // if (TWSR & 0xF8)! = 0x40)  -->then go to the error
}
void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while((TWCR & (1<< TWINT)) == 0);

}
void i2c_stop()
{
	TWCR = (1<< TWINT) | (1<<TWEN) | (1<<TWSTO);
}
void i2c_init(void)
{
	TWSR=0x00;
	TWBR=0x47;
	TWCR=0x04;
}
unsigned char i2c_rec(unsigned char isLast)
{
	if (isLast == 0) //read more than 1 byte
	{
		TWCR = (1<< TWINT) | (1<<TWEN) | (1<<TWEA); // 1 in TWEA so slave knows reading option is open
	}
    else
	{
		TWCR = (1<< TWINT) | (1<<TWEN) ;
	}
	while((TWCR & (1<<TWINT)) == 0);
	return TWDR;
	/*
		TWCR = (1<< TWINT) | (1<<TWEN) | (isLast<<TWEA);
		while (!(TWCR & (1 << TWINT)));
		return TWDR;
	*/
}

int16_t compass_val()
{
	uint16_t p;
    unsigned char d1,d2;
	uint16_t d4;

	i2c_start();
	i2c_write(0x42);
	i2c_write(0x41);
	i2c_stop();
	_delay_ms(10);
    i2c_start();
	i2c_write(0x43);
	d1=i2c_rec(0); // read more than 1 byte
	d2=i2c_rec(1);
	i2c_stop();
    d4=d1*256+d2;
    p = d4/10;
	//usart_send_int16(p);
	//usart_send_char('\n');
    return p;

}

void pwm1(int8_t a,int8_t b)
{
	TCCR0=0x6B;
	DDRB=255;
	OCR0=255-a;
	TCCR2=0x6B;
	DDRD=255;
	OCR2=255-b;
}
//forward motors

void pwm2(int8_t c,int8_t d)
{
	DDRD = 0xff;
	TCCR1A=0xA1;
	TCCR1B=0x0A;
	OCR1AH=0x00;
	OCR1AL=255-c;
	OCR1BL=255-d;
}


int main(void)
{
    usart_init();
    DDRA = 0xff;

    int16_t p,pi;//variable to be printed.
	int16_t pwm,error;
	int16_t threshold_value;
    i2c_init();
	usart_send_char('t');
    pi = compass_val();
    threshold_value = (pi+90)%360;
    if(threshold_value < pi)
    	threshold_value += 360;
     while(1)
    {
	 p = compass_val();//current value of angle
	 if(p < pi)
	 {
		 p += 360;
	 }
	 error = threshold_value - p;
	 if(error < 0)
	 {
		 PORTA = anticlk;
		 error = -error;
	 }
	 else
		 PORTA = clk;
	 pwm = error * kp;
	 pwm1(pwm,pwm);
	 pwm2(pwm,pwm);
	 /*
	 p =  clockwise(p,prev);
	 if(threshold_value > p)
	 {

		 //p = clockwise(p,prev,flag1);
		 PORTA = clk;
		 error = threshold_value - p;
	 }
	 else if(threshold_value <= p)
	 {
		 usart_send_char('r');
		 p = anticlockwise(p,prev);
		 PORTA = anticlk;
		 error = p - threshold_value;
	 }*/

	usart_send_char('p');
	 usart_send_char('\n');
	 usart_send_int16(p);
	 usart_send_char('\n');
	 usart_send_char('m');
	 usart_send_char('\n');
	 usart_send_int16(pwm);
	 usart_send_char('\n');
	 usart_send_char('\n');

	 _delay_ms(1000);
	}

    return 0;
}
