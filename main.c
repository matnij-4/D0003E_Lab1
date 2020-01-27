/*
 * D0003E_Lab1.c
 *
 * Created: 2020-01-21 10:22:46
 * Author : Mattias Nilsson matnij-4
 * Author : Jacob Moregård
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>


void LCDInit(void) {
	
	//Set Lowpower Waveform, no frame interrupt, no blanking. LCD Enable
	LCDCRA = (1 << LCDAB) | (1 << LCDEN);
	
	//drive time 300 microseconds, contrast control voltage 3.35 V.
	LCDCCR = (1 << LCDCC0) | (1 << LCDCC1) | (1 << LCDCC2) | (1 << LCDCC3);
	
	//external asynchronous clock source, 1/3 bias, 1/4 duty cycle, 25 segments.
	LCDCRB = (1 << LCDCS) | (1<< LCDMUX0) | (1<< LCDMUX1) | (1 <<LCDPM0) | (1 <<LCDPM1) | (1 <<LCDPM2);
	
	//prescaler setting N=16, clock divider setting D=8
	LCDFRR = (1 << LCDCD0) | (1 << LCDCD1) | (1 << LCDCD2);

}



//Will write the Char on the screen as position pos.
void writeChar(char ch, int pos)
{
	
	//The Address to the first position on the LCD.
	uint8_t *lcdaddr = (uint8_t *) 0xec;
	
	//Mask to get only the nibble of a value.
	uint8_t mask;
	
	//Nibbel the number that is sent to the LCD.
	uint8_t nibbleNumber = 0x0;
	
	
	//SCC Table with the numbers from 0 to 9.
	uint16_t sccTable[10] = {0x1551, 0x0110, 0x1e11, 0x1B11, 0x0B50, 0x1B41, 0x1F41, 0x0111, 0x1F51, 0x0B51};
	
	
	// Check if position is outside or not.
	if (pos < 0 || pos > 5) {
		return;
	}
	
	
	uint16_t number = 0x0;
	
	// Check if it is a 0 to 9.
	if (ch >= '0' || ch <= '9')
	{
		//Get the number out of the array.
		number = sccTable[ch - '0'];
	}
	
	
	
	
	//Point to the right position. Divide by 2 you can say.
	lcdaddr += pos >> 1;
	
	
	//Check if it is odd or even possition.
	if (pos % 2 == 0)
	{
		mask = 0xf0;
	}
	else
	{
		mask = 0x0f;	
	}
	
	
	//Will place out the nibbels on the right LCD address for the number.
	for (int i = 0; i < 4; i++ )
	{
		//Masking the smallest byte.
		nibbleNumber = number & 0xf;
		number = number >> 4;
		
		
		//Check position.
		if(pos % 2 != 0)
		{
			nibbleNumber = nibbleNumber << 4;
		}
		
		//Send the nibble.
		*lcdaddr = (*lcdaddr & mask) | nibbleNumber;
		
		lcdaddr += 5;
		
	}
	
}



void writelong(long i)
{
	//Set the position to 5.
	int pos = 5;
	
	
	//For loop to go through all the numbers.
	for(int n = 0; n < 6 ; n++ )
	{
		//Break the loop if it is out of numbers.
		if(1 > i)
		{
			break;
		}
		
		//Call the function with the position. But convert it first with '0'
		writeChar((i % 10)+'0', pos);
		
		// Change Pos and cut the long.
		pos -= 1;
		i = i/10;
	}

	
}


//Check if a number is a Prime
bool is_prime(long i)
{
	//Loop all the number under and try to divide with them.
	for(int n = 2; n < i; n++)
	{
		if(i % n == 0)
		{
			return false;
		}
		
	}
	return true;
	
}


//Finds prime numbers and prints them.
void primes(){
	
	//Being at 2 as it is the first prime.
	long n = 2;
	
	
	//Loop all the numbers.
	while(true)
	{
		//If it is prime print the number.
		if(is_prime(n))
		{
			writelong(n);
		}
		//The next number
		n++;
	}
// Can be done faster if only doing odd numbers and only trying to divide with previously found prime numbers.	
}


//Make one of the segments of the LCD blink.
void blink(void)
{
	//prescaling factor of 256.
	TCCR1B = (0 << CS10) | (0 << CS11) | (1 << CS12);
	
	
	//Time counter in 16 bit.
	//8 MHz system clock with a prescaling factor of 256.
	// 31250 just happen to be 8000000/256.
	uint16_t timeCounter = 31250;
	
	
	//Flag to see if it is on or off.
	bool onFlag = false;
	
	
	// Loop to make the LCD blink.
	while(true)
	{
		
		//Will start the blinking if the timer and onFlag is right.
		if(TCNT1 >= timeCounter && !onFlag)
		{
			
			//Set the flag to known the LCD have been accessed.
			onFlag = true;
			
			//If the timer reach the top it will need to reset.
			if(timeCounter + 31250 > 0xffff)
			{
				//Reset the timer.
				timeCounter = 31250;
			}
			//Add to the counter
			else
			{
				//adds to the counter.
				timeCounter = timeCounter + 31250;
			}
			
			//Read the LCD port to see if it is already on.
			if(LCDDR0 != 0)
			{
				//Turn in off
				LCDDR0 = 0x00;
			}
			else
			{
				//Turn it on.
				LCDDR0 = 0x02;
			}
			
			
		}
		
		//Cycle Check and then begin the blinking.
		if(timeCounter > TCNT1)
			{
				onFlag = false;
			}
		
	}
}


//Activates lights on the LCD by using the joystick.
void button(void)
{
	
}



int main(void)
{
	// Disabled the clock prescaler functionality.
	CLKPR = 0x80;
	CLKPR = 0x00;
	
	
	//Initializes the LCD settings and the power supply.
    LCDInit();
	
	
	
	
    while (true) 
    {
		//writeChar('8',5);
		//writelong(133769420);
		//primes();
		blink();
    }
	
	return 0;
}