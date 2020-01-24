/*
 * D0003E_Lab1.c
 *
 * Created: 2020-01-21 10:22:46
 * Author : Mattias Nilsson matnij-4
 * Author : Jacob Moreg�rd
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
	
	
	// Check if it is a 0 to 9.
	if (ch < 0 || ch > 9) {
		ch = 0;
	}
	
	
	//Get the number out of the array.
	uint16_t number = sccTable[ch];
	
	
	//Point to the right position.
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
		
		*lcdaddr = (*lcdaddr & mask) | nibbleNumber;
		
		lcdaddr += 5;
		
	}
	
}



void writelong(long i)
{
	//Converts it to a char and set the position to 5.
	char chars = i;
	int pos = 5;
	
	//For loop to go through all the numbers.
	for(int n = 0; n < 6 ; n++ )
	{
		//Break the loop if it is out of numbers.
		if(1 > i)
		{
			break;
		}
		
		//Call the function with the position.
		writeChar(i % 10, pos);
		
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
	
	//Beinge at 2 as it is the first prime.
	int n = 2;
	
	
	//Loop all the numbers.
	while(true)
	{
		//If it is prime print.
		if(is_prime(n))
		{
			writelong(n);
		}
		n++;
	}
	
}



int main(void)
{
	// Disabled the clock prescaler functionality.
	CLKPR = 0x80;
	CLKPR = 0x00;
	
	
	//Inizialis the LCD settings and the power supply.
    LCDInit();
	
	
	
	primes();
	
    while (true) 
    {
		//writeChar(8,5);
		//writelong(120);
    }
	return 0;
}
