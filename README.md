# DuoSetupCLI

Command line interface program for setting up Duo

    Usage: DuoSetupCLI <option> [parameters] 
	
	options:    
	    --upload       Upload firmware to Duo    
	    --version      Fetch the firmware versions    
	    --device-id    Fetch the MCU's unique 12-bytes device ID    
	    --credential   Check if Duo has stored credentials or not    
	    --scan-ap      Scan the nearby Wi-Fi Access Points for Duo to connect    
	
	parameters:   
	    --verbose      Print additional message during executing programm. 
	    --file <file>  Used with --upload option. Specify the binary file (.bin) to be uploaded.
	    --reset        Used with --upload option. Make Duo perform reset when uploading firmware complete.

# Build DuoSetupCLI

## Dependencies

make command and GCC tool chain

## Build
Git clone the repository or download it to your local system. Open command line terminal and change the working directory to `./DuoSetupCLI`.

Run `make` to build the `DuoSetupCLI`. The built program is under `./DuoSetupCLI/build`.

Run `make verbose=1` will build the program and print building details.

Run `make clean` to clean all object files and the built program.

# License

Copyright (c) 2016 Red Bear

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


  
