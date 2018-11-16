#ifndef _AS1115_h
#define _AS1115_h

#define _AS1115_DIAGNOSTICS_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif
#include <Wire.h>

enum AS1115_DIGIT
{
	DIGIT0	= 0x01,
	DIGIT1	= 0x02,
	DIGIT2	= 0x03,
	DIGIT3	= 0x04,
	DIGIT4	= 0x05,
	DIGIT5	= 0x06,
	DIGIT6	= 0x07,
	DIGIT8	= 0x08
};

enum AS1115_REGISTER
{
	DECODE_MODE			= 0x09,
	GLOBAL_INTENSITY	= 0x0A,
	SCAN_LIMIT			= 0x0B,
	SHUTDOWN			= 0x0C,
	SELF_ADDRESSING		= 0x2D,
	FEATURE				= 0x0E,
	DISPLAY_TEST_MODE	= 0x0F,
	DIG01_INTENSITY		= 0x10,
	DIG23_INTENSITY		= 0x11,
	DIG45_INTENSITY		= 0x12,
	DIG67_INTENSITY		= 0x13,
	DIAG_DIGIT0			= 0x14,
	DIAG_DIGIT1			= 0x15,
	DIAG_DIGIT2			= 0x16,
	DIAG_DIGIT3			= 0x17,
	DIAG_DIGIT4			= 0x18,
	DIAG_DIGIT5			= 0x19,
	DIAG_DIGIT6			= 0x1A,
	DIAG_DIGIT7			= 0x1B,
	KEY_A				= 0x1C,
	KEY_B				= 0x1D
};

enum AS1115_SHUTDOWN_MODE
{
	SHUTDOWN_MODE		= 0x00,
	NORMAL_OPERATION	= 0x01,
	RESET_FEATURE		= 0x00,
	PRESERVE_FEATURE	= 0x80
};

enum AS1115_DECODE_SEL
{
	CODE_B_DECODING		= 0x00,
	HEX_DECODING		= 0x01
};

enum AS1115_FEATURE
{
	CLOCK_ACTIVE		= 0x00,
	RESET_ALL			= 0x01,
	DECODE_SEL			= 0x02,
	BLINK_ENABLE		= 0x04,
	BLINK_FREQ_SEL		= 0x05,
	SYNC				= 0x06,
	BLINK_START			= 0x07
};

#ifdef _AS1115_DIAGNOSTICS_

enum AS1115_DISPLAY_TEST_MODE
{
	DISP_TEST	= 0x00,
	LED_SHORT	= 0x02,
	LED_OPEN	= 0x04,
	LED_TEST	= 0x08,
	LED_GLOBAL	= 0x10,
	RSET_OPEN	= 0x20,
	RSET_SHORT	= 0x40
};

#endif

inline AS1115_REGISTER operator+(AS1115_REGISTER a, byte b) {
	return static_cast<AS1115_REGISTER>(static_cast<int>(a) + b);
};

inline AS1115_DIGIT operator+(AS1115_DIGIT a, byte b) {
	return static_cast<AS1115_DIGIT>(static_cast<int>(a) + b);
};

inline AS1115_SHUTDOWN_MODE operator|(AS1115_SHUTDOWN_MODE a, AS1115_SHUTDOWN_MODE b) {
	return static_cast<AS1115_SHUTDOWN_MODE>(static_cast<int>(a) | static_cast<int>(b));
};

class AS1115
{
private:
	byte _deviceAddr;
	byte _digits;

	void writeRegister(AS1115_REGISTER reg, byte value);
	byte readRegister(AS1115_REGISTER reg);
public:
	AS1115(byte addr);
	~AS1115();
#ifdef _DEBUG
	void debug();
#endif
	void init(byte digits, byte intensity);
	void setIntensity(byte intensity);

	void shutdown(bool preserve);
	void resume(bool preserve);

	void clear();
	void display(int value);
	void display(const char value[]);
	void display(byte digit, byte value);

	byte readPort(byte port);
	short read();

#ifdef _AS1115_DIAGNOSTICS_
	void visualTest(bool stop);
	bool ledTest(AS1115_DISPLAY_TEST_MODE mode, byte result[]);
	bool rsetTest(AS1115_DISPLAY_TEST_MODE mode);
#endif
};

#endif

