# MCP3914 Driver

C++ Driver for reading from and configuring the MCP3914 ADC.

## Installation (CMAKE)

Either download the files from github and add them as a library to your project or add this repository as a git submodule.
Then, simply include the directory in your CMakeFile.

```CMake
include(MCP3914)
```

## Usage

```python
#define NUM_CHANNELS 8
#include "mcp3914.h"

MCP3914 adc_device = MCP3914(0);


// Test the device connection
uint8_t sec_res = adc_device.sec_read_test();

// Take a reading
float adcVals[NUM_CHANNELS];

adc_device.read(adcVals, NUM_CHANNELS);
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first
to discuss what you would like to change.


## License

MIT License

Copyright (c) 2024 Jacques Van Wyk

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.