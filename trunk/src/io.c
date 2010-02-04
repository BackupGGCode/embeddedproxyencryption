#include "io.h"
#include "test.h"


unsigned char getUserInput()
{
	unsigned char input = 0;
	unsigned char pinEInput = (PINE & 0b00110000);
	unsigned char pinBInput = (PINB & 0b11100000);
	if(pinEInput == 0x20)//down joystick pressed
	{
		input = 2;
	}
	else if(pinEInput == 0x10)//right joystick pressed
	{
		input = 3;
	}
	else if(pinBInput == 0x60)//up joystick pressed
	{
		input = 1;
	}
	else if(pinBInput == 0xA0)//left joystick pressed
	{
		input = 4;
	}
	else if(pinBInput == 0xC0)//select joystick pressed
	{
		input = 5;
	}
	return input;
}

unsigned char processSystemIO()
{
	unsigned char output = 0;
	unsigned char prevInput = 0;
	unsigned char input = 0;
	while(1)
	{
		input = getUserInput();
		if(input != prevInput)
		{
			//if left joystick pressed shift hexadecimal digits left
			if(input == 4)
			{
				output = output << 4;
				display2CharsOn7SegBCD(output);
				_delay_ms (100.0);
			}
			//if down joystick button pressed down decrement output
			else if(input == 3)
			{
				if(output == 0)
				{
					output = 255;
				}
				else
				{
					output--;
				}
				display2CharsOn7SegBCD(output);
				_delay_ms (100.0);
			}
			//if right joystick pressed shift hexadecimal digits right
			else if(input == 2)
			{
				output = output >> 4;
				display2CharsOn7SegBCD(output);
				_delay_ms (100.0);
			}
			//if up joystick button pressed up increment output
			else if(input == 1)
			{
				output++;
				if(output > 255)
				{
					output = 0;
				}
				display2CharsOn7SegBCD(output);
				_delay_ms (100.0);
			}
			//if select button pressed reset output = 0
			if(input == 5)
			{
				display2CharsOn7SegBCD(output);
				_delay_ms (100.0);
				break;
			}
		}
		prevInput = input;
	}
	turnOffDisplay();
	_delay_ms (100.0);
	return output;
}

void display2CharsOn7SegBCD(unsigned char bcd)
{
	unsigned char bits0 = (bcd & 0b00001111);
	unsigned char bits1 = (unsigned char)(bcd>>4);
	unsigned char hexBCDArray[] = {0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110,
								   0b10111110, 0b11100000, 0b11111110, 0b11100110, 0b11101110, 0b00111110,
								   0b10011100, 0b01111010, 0b10011110, 0b10001110};
	unsigned char display0 = hexBCDArray[bits0];
	unsigned char display1 = hexBCDArray[bits1];
	if(ROLE != SENDER)
	{
		display0 = ~display0;
		display1 = ~display1;
	}
	PORTA = display1;
	PORTF = display0;
}

void turnOffDisplay()
{
	PORTA = 0x00;
	PORTF = 0x00;
}

unsigned char getRoleSignal(unsigned char role)
{
	unsigned char signal = 0x00;
	if(role == RECEIVER)
	{
		signal = RECEIVER_SIGNAL;
	} 
	else if(role == PROXY)
	{
		signal = PROXY_SIGNAL;
	}
	else if(role == SENDER)
	{
		signal = SENDER_SIGNAL;
	}
	return signal;
}

void sendCharToPort(unsigned char port, unsigned char message)
{
	switch (port) 
	{
		case PORTA_ID:
			PORTA = message;
			break;
		case PORTB_ID:
			PORTB = message;
			break;
		case PORTC_ID:
			PORTC = message;
			break;
		case PORTD_ID:
			PORTD = message;
			break;
		case PORTE_ID:
			PORTE = message;
			break;
		case PORTF_ID:
			PORTF = message;
			break;
	}
}

unsigned char getCharFromPort(unsigned char port)
{
	unsigned char receivedChar = 0x00;
	switch(port)
	{
		case PORTA_ID:
			receivedChar = PINA;
			break;
		case PORTB_ID:
			receivedChar = PINB;
			break;
		case PORTC_ID:
			receivedChar = PINC;
			break;
		case PORTD_ID:
			receivedChar = PIND;
			break;
		case PORTE_ID:
			receivedChar = PINE;
			break;
		case PORTF_ID:
			receivedChar = PINF;
			break;
	}
	return receivedChar;
}

unsigned char waitForSignalCharFromPort(unsigned char signal, unsigned char port, unsigned char getNonSignalChar)
{
	unsigned char currentChar = getCharFromPort(port);
	if(getNonSignalChar == 1)
	{
		while(currentChar == signal)
		{
			_delay_ms(10.0);
			currentChar = getCharFromPort(port);
			display2CharsOn7SegBCD(currentChar);
		}
	}
	else
	{
		while(currentChar != signal)
		{
			_delay_ms(10.0);
			currentChar = getCharFromPort(port);
			display2CharsOn7SegBCD(currentChar);
		}
	}
	display2CharsOn7SegBCD(currentChar);
	return currentChar;
}

unsigned char waitForSignalFromPort(unsigned char signal, unsigned char port)
{
	unsigned char currentChar = getCharFromPort(port);
	while(currentChar != signal)
	{
		_delay_ms(10.0);
		currentChar = getCharFromPort(port);
		display2CharsOn7SegBCD(currentChar);
	}
	display2CharsOn7SegBCD(currentChar);
	return currentChar;
}

unsigned char waitForPublicKeyCharFromPort(unsigned char signal, unsigned char port)
{
	unsigned char currentChar = getCharFromPort(port);
	while(currentChar == signal)
	{
		_delay_ms(10.0);
		currentChar = getCharFromPort(port);
		display2CharsOn7SegBCD(currentChar);
	}
	display2CharsOn7SegBCD(currentChar);
	return currentChar;
}

unsigned char receivePublicKeyElement(unsigned char incomingSignal, unsigned char incomingPort, unsigned char outgoingSignal, unsigned char outgoingPort)
{
	unsigned char pkElement = 0x00;

	//wait for signal to expect public key element
	waitForSignalFromPort(incomingSignal, incomingPort);
	_delay_ms(1000.0);
	sendCharToPort(outgoingPort, 0x00);


	//Get first char of public key element
	unsigned char currentChar = waitForPublicKeyCharFromPort(incomingSignal, incomingPort);
	pkElement = pkElement + ((currentChar & 0x0F) << 4);
	_delay_ms(1000.0);
	sendCharToPort(outgoingPort, outgoingSignal);
	
	//wait for signal to expect public key element	
	currentChar = waitForSignalFromPort(incomingSignal, incomingPort);
	_delay_ms(1000.0);
	sendCharToPort(outgoingPort, 0x00);

	//Get second char of public key element
	currentChar = waitForPublicKeyCharFromPort(incomingSignal, incomingPort);
	pkElement = pkElement + (currentChar & 0x0F);
	display2CharsOn7SegBCD(pkElement);
	_delay_ms(1000.0);
	sendCharToPort(outgoingPort, outgoingSignal);

	return pkElement;
}

unsigned int *receivePublicKey(unsigned char incomingSignal, unsigned char incomingPort, unsigned char outgoingSignal, unsigned char outgoingPort)
{

	unsigned int *publicKey = (unsigned int*)malloc(sizeof(unsigned int)*2);
	unsigned char e = 0x00;
	unsigned char n = 0x00;

	//recv'd e
	e = receivePublicKeyElement(incomingSignal, incomingPort, outgoingSignal, outgoingPort);
	*(publicKey) = (unsigned int)e;

	//recv'd n
	n = receivePublicKeyElement(incomingSignal, incomingPort, outgoingSignal, outgoingPort); 
	*(publicKey + 1) = (unsigned int)n;

	return publicKey;
}

void sendPublicKeyElement(unsigned char incomingSignal, unsigned char incomingPort, unsigned char outgoingSignal, unsigned char outgoingPort, unsigned int pkElement)
{
	//send signal to prep reciever to have public key element sent
	display2CharsOn7SegBCD(outgoingSignal);
	sendCharToPort(outgoingPort, outgoingSignal);
	display2CharsOn7SegBCD(outgoingSignal);
	waitForSignalFromPort(0x00, incomingPort);
	_delay_ms(1000.0);	

	//send first char of public key element pair
	unsigned char currentChar = ((unsigned char)(pkElement & 0xF0) >> 4);
	sendCharToPort(outgoingPort, currentChar);
	waitForSignalFromPort(incomingSignal, incomingPort);
	display2CharsOn7SegBCD(pkElement);
	_delay_ms(1000.0);

	//send signal to prep for second char
	sendCharToPort(outgoingPort, outgoingSignal);
	waitForSignalFromPort(0x00, incomingPort);
	display2CharsOn7SegBCD(outgoingSignal);
	_delay_ms(1000.0);	

	//send second char of public key element
	currentChar = (unsigned char)(pkElement & 0x0F);
	sendCharToPort(outgoingPort, currentChar);
	waitForSignalFromPort(incomingSignal, incomingPort);
	display2CharsOn7SegBCD(pkElement);
	_delay_ms(1000.0);
}

void sendPublicKey(unsigned char incomingSignal, unsigned char incomingPort, unsigned char outgoingSignal, unsigned char outgoingPort, unsigned int e, unsigned int n)
{
	//Send e of public key pair
	sendPublicKeyElement(incomingSignal, incomingPort, outgoingSignal, outgoingPort, e);

	//Send e of public key pair
	sendPublicKeyElement(incomingSignal, incomingPort, outgoingSignal, outgoingPort, n);

	//Send signal that public key pair sent
	sendCharToPort(outgoingPort, outgoingSignal);
	display2CharsOn7SegBCD(outgoingSignal);
	_delay_ms(1000.0);
}

