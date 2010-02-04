#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <math.h>
#include "rsa.h"
#include "io.h"
#include "utils.h"

#define F_CPU 1000000UL

void performProxy();
void performSender();
void performReceiver();
