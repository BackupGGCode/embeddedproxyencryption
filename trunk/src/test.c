#include "test.h"

int main(void)
{
	//Setting ports for i/o in sequence from usb port back


	if(ROLE == RECEIVER)
	{
		DDRA = 0xFF; // PortA is LED 1 output
		DDRF = 0xFF; // PortF is LED 2 
		DDRB = 0x1F; // PortB is joystick
		PORTB = 0xE0;
		DDRE = 0xCF; // PortE is joystick
		PORTE = 0x30;
		DDRC = 0xFF; // PortC is output to proxy
		DDRD = 0x00; // PortD is input from proxy
		performReceiver();
	}
	else if(ROLE == PROXY)
	{
		DDRA = 0xFF; // PortA is LED 1 output
		DDRB = 0x00; // PortB is input from reciever
		DDRE = 0xFF; // PortE is output to reciever
		DDRD = 0x00; // Port D input from sender
		DDRC = 0xFF; // Port C output to sender
		_delay_ms(1000.0);
		performProxy();
	}
	else
	{
		DDRA = 0xFF; // PortA is LED 1 output
		DDRF = 0xFF; // PortF is LED 2 
		DDRB = 0x1F; // PortB is joystick / secondary output
		PORTB = 0xE0;
		DDRE = 0xCF; // PortE is joystick / secondray input
		PORTE = 0x30;
		DDRC = 0xFF; // PortC is output to proxy
		DDRD = 0x00; // PortD is input from proxy
		display2CharsOn7SegBCD(SENDER_SIGNAL);
		_delay_ms(1000.0);
		performSender();
	}

	return 0;
}

void performProxy()
{
	unsigned char receiverSendPort = PORTE_ID;
	unsigned char receiverReceivePort = PORTB_ID;
	unsigned char senderSendPort = PORTC_ID;
	unsigned char senderReceivePort = PORTD_ID;
	sendCharToPort(receiverSendPort, PROXY_SIGNAL);
	//receiver public keys
	unsigned int e = 0;
	unsigned int n = 0;
	//unsigned int *e_s = (unsigned int*)malloc(sizeof(unsigned int)*(NUM_PROXIES+1));
	//unsigned int *n_s = (unsigned int*)malloc(sizeof(unsigned int)*(NUM_PROXIES+1));
	for(int i = 0; i <= NUM_PROXIES; i++)
	{
		unsigned int *publicKey = (unsigned int*)malloc(sizeof(unsigned int)*2);
		publicKey = receivePublicKey(RECEIVER_SIGNAL, receiverReceivePort, PROXY_SIGNAL, receiverSendPort);
		if(i > 0)
		{
			unsigned int e_temp = (unsigned int)*(publicKey);
			unsigned int n_temp = (unsigned int)*(publicKey + 1);
			display2CharsOn7SegBCD(PROXY_SIGNAL);
			_delay_ms(1000.0);
			sendPublicKey(PROXY_SIGNAL, senderSendPort, SENDER_SIGNAL, senderReceivePort, e_temp, n_temp);
		}
		else
		{
			e = (unsigned int)*(publicKey);
			n = (unsigned int)*(publicKey + 1);
		}
	}
	display2CharsOn7SegBCD(PROXY_SIGNAL);

}

void performReceiver()
{
	unsigned char proxyReceivePort = PORTD_ID;
	unsigned char proxySendPort = PORTC_ID;
	sendCharToPort(proxySendPort, RECEIVER_SIGNAL);
	display2CharsOn7SegBCD(RECEIVER_SIGNAL);
	_delay_ms(1000.0);
	unsigned int *d_s = (unsigned int*)malloc(sizeof(unsigned int)*(NUM_PROXIES+1));
	unsigned int *e_s = (unsigned int*)malloc(sizeof(unsigned int)*(NUM_PROXIES+1));
	unsigned int *n_s = (unsigned int*)malloc(sizeof(unsigned int)*(NUM_PROXIES+1));
	//Create public keys
	for(int i = 0; i <= NUM_PROXIES; i++)
	{
		unsigned int seed = (unsigned int)processSystemIO();
		unsigned char *keys = (unsigned char*)malloc(sizeof(unsigned char)*6);
		keys = rsaEncrypt(seed);
		*(e_s + i) = (*(keys + 1) << 8) + *(keys + 0);
		*(d_s + i) = (*(keys + 3) << 8) + *(keys + 2);
		*(n_s + i) = (*(keys + 5) << 8) + *(keys + 4);
		//publish public keys
		unsigned int e = *(keys + 0);
		unsigned int n = *(keys + 4);
		sendPublicKey(RECEIVER_SIGNAL, proxySendPort, PROXY_SIGNAL, proxyReceivePort, e, n);
	}
	//wait for message
	display2CharsOn7SegBCD(RECEIVER_SIGNAL);

}

void performSender()
{
	unsigned char proxyReceivePort = PORTD_ID;
	unsigned char proxySendPort = PORTC_ID;
	//receiver public keys
	unsigned int e_s = 0x00;
	unsigned int n_s = 0x00;
	display2CharsOn7SegBCD(SENDER_SIGNAL);
	_delay_ms(1000.0);
	unsigned int *publicKey = (unsigned int*)malloc(sizeof(unsigned int)*2);
	publicKey = receivePublicKey(PROXY_SIGNAL, proxyReceivePort, SENDER_SIGNAL, proxySendPort);
	e_s = (unsigned int)*(publicKey);
	display2CharsOn7SegBCD((unsigned char)e_s);
	_delay_ms(1000.0);
	n_s = (unsigned int)*(publicKey + 1);
	display2CharsOn7SegBCD((unsigned char)n_s);
	_delay_ms(1000.0);
	display2CharsOn7SegBCD(SENDER_SIGNAL);
	_delay_ms(1000.0);
}
