/*
 * Despite the name, this is actually true "main()" file for Arduino - contains the setup() and loop() functions.
 */

#include "picoshell.h"

#define BAUD 9600

int main(void);

void io_open(void) {};
void io_close(void) {};

int pico_getchar(void)
{
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
}

void loop() {
    if (Serial.available()) {
        pico_puts("-----------------------------\n");
        pico_puts("    picoshell for Arduino\n");
        pico_puts("-----------------------------\n");
        main();
    }
}
