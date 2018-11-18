/**
 * Use 4 7 segments to display passing seconds
 */

#include <Wire.h>
#include <AS1115.h>

AS1115 _as = AS1115(0x00);
long m = 0;

void setup()
{
	Wire.begin();
	_as.init(4, 6);
	_as.clear();
}

void loop()
{
	if (millis() - m < 1000) {
		return;
	}

	m = millis();
	_as.display(m / 1000);
}
