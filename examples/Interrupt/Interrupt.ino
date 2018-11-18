/**
 * Any press on one of the 16 keys will increase the displayed counter.
 */

#include <Wire.h>
#include <AS1115.h>

#define AS1115_ISR_PIN  2 ///< Interrupt pin connected to AS1115

AS1115 as = AS1115(0x00);
long m = 0;
volatile bool interrupted = false;

void interrupt() {
    interrupted = true;
}

void setup()
{
    pinMode(AS1115_ISR_PIN, INPUT);
	attachInterrupt(digitalPinToInterrupt(AS1115_ISR_PIN), interrupt, FALLING);

	Wire.begin();
	as.init(4, 6);
	as.clear();
    as.read(); // reset any pending interrupt on the chip side

    as.display(0);
}

void loop()
{
    if(!interrupted) {
        delay(100);
        return;
    }

    interrupted = false;
    
    // active input are LOW, it's easier to invert the result
    uint16_t current = ~as.read();
    if(current == LOW) return;

    m += 1;
    as.display(m);
}
