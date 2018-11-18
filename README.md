# AS1115
[![Build Status](https://travis-ci.org/blemasle/arduino-as1115.svg?branch=master)](https://travis-ci.org/blemasle/arduino-as1115)
[![License](https://img.shields.io/badge/license-MIT%20License-blue.svg)](http://doge.mit-license.org)

Display digits and letters to up to 8 7 segments displays using an AMS' [AS1115](https://ams.com/AS1115).  
Reading the 16 keys state is also supported.

> No default instance is created when the library is included

## Features
 * Display messages on up to 8 7 segments displays using char arrays.
 * Variable display intensity.
 * Read the staet of the 16 keys at once.
 * Support for low power Shutdown mode

## Usage
Unlike most Arduino library, no default instance is created when the library is included. It's up to you to create one with the appropriate chip I2C address.

```cpp
#include <Arduino.h>
#include <AS1115.h>

AS1115 as = AS1115(0x00);
```

See included examples for furher usage.
