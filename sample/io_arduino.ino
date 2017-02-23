/*
 * Despite the name, this is actually "main()" file for Arduino - contains the setup() and loop() functions.
 */

#include "picoshell.h"

#define BAUD 9600

int shell(void);

void io_open(void) {};
void io_close(void) {};

int pico_getchar(void)
{
    while( ! Serial.available() ) {
        delay(1);
    }
    int c = Serial.read();
    if ( c == '\r' ) {
        return '\n';
    } else {
        return c;
    }
}

int pico_putchar(int c)
{
    if ( c == '\n' ) {
        Serial.write('\r');
    }
    Serial.write(c);
    return 0;
}

int pico_puts(const char* s)
{
    const char* c = s;
    while ( *c != '\0' ) {
        pico_putchar(*c);
        c++;
    }
    return 1;
}

void setup()  {
    Serial.begin(BAUD);
    delay(10);
    pico_puts("\n\n*** picoshell for Arduino ***\n");
}

void loop() {
    shell();
}
