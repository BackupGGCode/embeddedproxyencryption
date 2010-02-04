#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"

void display2CharsOn7SegBCD(unsigned char bcd);
unsigned char getUserInput();
void turnOffDisplay();
unsigned char processSystemIO();

unsigned char getRoleSignal(unsigned char role);

void sendCharToPort(unsigned char port, unsigned char Char);
unsigned char getCharFromPort(unsigned char port);

unsigned char waitForSignalCharFromPort(unsigned char signal, unsigned char port, unsigned char getNonSignalChar);
unsigned char waitForSignalFromPort(unsigned char signal, unsigned char port);
unsigned char waitForPublicKeyCharFromPort(unsigned char signal, unsigned char port);

unsigned int *receivePublicKey(unsigned char incomingSignal, unsigned char incomingPort, unsigned char outgoingSignal, unsigned char outgoingPort);
unsigned char receivePublicKeyElement(unsigned char incomingSignal, unsigned char incomingPort, unsigned char outgoingSignal, unsigned char outgoingPort);

void sendPublicKeyElement(unsigned char incomingSignal, unsigned char incomingPort, unsigned char outgoingSignal, unsigned char outgoingPort, unsigned int pkElement);
void sendPublicKey(unsigned char incomingSignal, unsigned char incomingPort, unsigned char outgoingSignal, unsigned char outgoingPort, unsigned int e, unsigned int n);
