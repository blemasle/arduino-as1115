/**
 * Use 4 7 segments to display passing seconds.
 */

#include <Wire.h>
#include <AS1115.h>

AS1115 as = AS1115(0x00);
long m = 0;

void setup()
{
	Wire.begin();
	as.init(4, 6);
	as.clear();
}

void loop()
{
	if (millis() - m < 1000) {
		return;
	}

	m = millis();
	as.display(m / 1000);
}
