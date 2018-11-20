#include "AS1115.h"

#define DIGITS	0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B, 0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47, 

//uppercase letters		A,    b,    C,    d     E     F     G     H     I     J     K     L     M
//                      n     o     P     q     r     s     t     U     v     W     X     y     Z
#define LETTERS			0x77, 0x1F, 0x4E, 0x3D, 0x4F, 0x47, 0x5E, 0x37, 0x30, 0x3C, 0x2F, 0x0E, 0x54, \
                        0x15, 0x1D, 0x67, 0x73, 0x05, 0x5B, 0x0F, 0x3E, 0x1C, 0x2A, 0x49, 0x3B, 0x25

#define DOT		0x80
#define BLANK	0x00

//TODO : to be save in flash memory ??
const uint8_t digits[16] = { DIGITS };
const uint8_t letters[26] = { LETTERS };

uint8_t nthdigit(uint16_t value, uint8_t n)
{
	uint16_t pow = 1;
	for (uint8_t i = 0; i < n; i++) {
		pow *= 10;
	}

	return ((value / pow) % 10);
}

AS1115::AS1115(uint8_t addr) {
	_deviceAddr = addr;
}

AS1115::~AS1115() {}

void AS1115::init(uint8_t digits, uint8_t intensity)
{
	_digits = digits;
	
	Wire.beginTransmission(0x00);
	Wire.write(SHUTDOWN);
	Wire.write(NORMAL_OPERATION | RESET_FEATURE);
	Wire.endTransmission();

	if(_deviceAddr != 0x00) {
		Wire.beginTransmission(0x00);
		Wire.write(SELF_ADDRESSING);
		Wire.write(1);
		Wire.endTransmission();
	}

	writeRegister(DECODE_MODE, 0x00);
	writeRegister(SCAN_LIMIT, digits - 1);
	setIntensity(intensity);
}

void AS1115::setIntensity(uint8_t intensity)
{
	writeRegister(GLOBAL_INTENSITY, intensity);
}

void AS1115::shutdown(bool preserve)
{
	writeRegister(SHUTDOWN, SHUTDOWN_MODE | (preserve ? PRESERVE_FEATURE : RESET_FEATURE));
}

void AS1115::resume(bool preserve)
{
	writeRegister(SHUTDOWN, NORMAL_OPERATION | (preserve ? PRESERVE_FEATURE : RESET_FEATURE));
}

void AS1115::clear()
{
	int n = _digits;

	Wire.beginTransmission(_deviceAddr);
	Wire.write(DIGIT0);  //first digit to write is #1

	while (n--) {
		Wire.write(BLANK);
	}

	Wire.endTransmission();
}

void AS1115::display(uint16_t value) 
{
	uint8_t n = _digits;

	Wire.beginTransmission(_deviceAddr);
	Wire.write(DIGIT0); //first digit to write is #1

	while (n--) {
		Wire.write(digits[nthdigit(value, n)]);
	};

	Wire.endTransmission();
}

void AS1115::display(const char value[])
{
	uint8_t n = _digits;
	size_t len = strlen(value);
	char c;
	uint8_t modifier = BLANK;

	Wire.beginTransmission(_deviceAddr);
	Wire.write(DIGIT0); //first char to write is #1

	for(uint8_t i = 0; i < n; i++)
	{
		if(i >= len)
		{
			Wire.write(BLANK);
		}

		c = value[i];
		//trick to display '.' on the same digits if its the i + 1 char
		if(len - 1 > i && value[i + 1] == '.')
		{
			i++; //skip next char as handled on this iteration
			n++; //handle one extra char
			modifier = DOT;
		}
		else if(modifier != BLANK) //rewrite value only if != default
		{
			modifier = BLANK;
		}

		if(c >= 'A' && c <= 'Z')
		{
			Wire.write(letters[c - 'A'] + modifier);
		}
		else if(c >= 'a' && c <= 'z')
		{
			Wire.write(letters[c - 'a'] + modifier);
		}
		else if(c >= '0' && c <= '9')
		{
			Wire.write(digits[c - '0'] + modifier);
		}
		else if(c == ' ')
		{
			Wire.write(BLANK);
		}
	}

	Wire.endTransmission();
}

void AS1115::display(uint8_t digit, uint8_t value)
{
	writeRegister((AS1115_REGISTER)(DIGIT0 + digit - 1), value);
}

uint8_t AS1115::readPort(uint8_t port)
{
	uint8_t value = readRegister(KEY_A + port);
	//self-adrressing disable the two LSB of KEY_A 
	if(port == 0 && _deviceAddr != 0x00) return value && 0xFC;

	return value;
}

short AS1115::read()
{
	uint8_t a = readPort(0);
	uint8_t b = readPort(1);

	return a | b << 8;
}

#ifdef _AS1115_DIAGNOSTICS_

void AS1115::visualTest(bool stop)
{
	uint8_t testMode = readRegister(AS1115_REGISTER::DISPLAY_TEST_MODE);
	if(stop) bitClear(testMode, DISP_TEST);
	else bitSet(testMode, DISP_TEST);
	
	writeRegister(DISPLAY_TEST_MODE, DISP_TEST);
}

bool AS1115::ledTest(AS1115_DISPLAY_TEST_MODE mode, uint8_t result[])
{
	if(mode && (_BV(LED_SHORT) || _BV(LED_OPEN)) != mode) return true;

	uint8_t testMode = readRegister(DISPLAY_TEST_MODE);
	if(testMode & _BV(LED_TEST)) return true;

	writeRegister(DISPLAY_TEST_MODE, testMode | mode);
	do {
		delay(20);
	}while((testMode = readRegister(DISPLAY_TEST_MODE)) & _BV(LED_TEST));

	Wire.beginTransmission(_deviceAddr);
	Wire.write(DIAG_DIGIT0);
	Wire.endTransmission();
	//only care about the used digits
	Wire.requestFrom(_deviceAddr, (uint8_t)_digits);
	
	uint8_t i = 0;
	while(Wire.available()) {
		result[i] = Wire.read();
	}

	return !(testMode & _BV(LED_GLOBAL));
}

bool AS1115::rsetTest(AS1115_DISPLAY_TEST_MODE mode)
{
	if(mode < RSET_OPEN) return true;

	return !(readRegister(DISPLAY_TEST_MODE) & mode);
}

#endif

void AS1115::writeRegister(AS1115_REGISTER reg, uint8_t value)
{
	Wire.beginTransmission(_deviceAddr);
	Wire.write(reg);
	Wire.write(value);
	Wire.endTransmission();
}

uint8_t AS1115::readRegister(AS1115_REGISTER reg)
{
	Wire.beginTransmission(_deviceAddr);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(_deviceAddr, (uint8_t)1);
	return Wire.read();
}