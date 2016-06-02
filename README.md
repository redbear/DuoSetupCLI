# DuoSetupCLI
---

DuoSetupCLI Version: **v0.2.0**  
Cooperate with Duo Firmware Version: **v0.2.5 or above**

## Usage
---

DuoSetupCLI is a command line interface program for setting up Duo. **Power on your Duo and make it enter the listening mode**. The Duo will act as an AP, e.g. "Duo-xxxx" and you can see it in the AP scanned list on your computer. Connect your computer to the AP which your Duo is brodcasting.

    Usage: DuoSetupCLI <option> [parameters] 
	
    <options>:
        --upload          Upload firmware to Duo. At least one of the --file and -fac
                          parameters must be presented. It's OK if both are presented.
        --version         Fetch the firmware versions
        --device-id       Fetch the MCU's unique 12-bytes device ID
        --credential      Check if Duo has stored credentials or not
        --scan-ap         Scan the nearby Wi-Fi Access Points
        --config-ap       Config an AP for Duo to connect (**TBD**)
        --connect-ap      Leave listening mode and try connecting the stored AP
                          It will enter listening mode again if no valid AP stored
        --public-key      Fetch the device public key that stored in Duo

    [parameters]:
        --verbose,-v      Print additional message during executing this programm
        --file,-f <file>  Must be used with --upload option. Specify the binary file
                          (.bin) to be uploaded. The first file specified by --file,
                          will be stored from the beginning of the OTA region. If
                          several --file are presented, the rest files will be stored
                          in the OTA region successively.
        -fac <file>       Used with --upload option. Specify the binary file
                          (.bin) to be uploaded. The file will be stored from the
                          beginning of the FAC region.
        --safe,-s         Used with --upload option. Invalid user part so that
                          Duo enter safe mode after firmware update
        --leave,-l        Used with --upload option. Leave listening mode when
                          uploading firmware completed

#### OTA(Over The Air) Update Firmware

The binary image you are going to upload is stored at the begining of the OTA region of the external flash. Every time after you run the DuosetupCLI to upload firmware images, you must NOT run the command to perform upload action again if you don't have your Duo reset once to deploy the uploded firmware, or the forward uploaded files will be over written. Accompany with the `--leave` parameter when uploading files will automatically make your Duo leave listening mode and perform soft reset after files being uploaded. E.g.:

    DuoSetupCLI --upload -f duo-system-part1-v0.2.3.bin -l

DuoSetupCLI supports updating multiple files simultaneously (limited to 3 files for now) by specifying each file with `-f` parameter, as long as the total size of these files is not large than the OTA region size. E.g.:

    DuoSetupCLI --upload -f duo-system-part1-v0.2.3.bin -f duo-system-part2-v0.2.3.bin -f duo-user-part-v0.2.3.bin -l

If you are going to upload a factory reset (FAC) image to Duo, you should specify the file with `-fac` parameter so that it will be stored in FAC region, independent from the OTA region. Since the FAC image is stored in external flash directly, you don't have to leave the listening mode to deploy the FAC firmware. E.g.:

    DuoSetupCLI --upload -fac duo-fac-web-server-v0.2.3.bin    

The `-f` and `-fac` parameters can be presented in the same command, since the files specified by each are stored in defferent memory space. E.g.:

    DuoSetupCLI --upload -f duo-user-part-v0.2.3.bin -fac duo-fac-web-server-v0.2.3.bin

Sometimes the new released firmware version is not compatible with the old application running on your Duo. In this case, you had better use the "--safe" parameter in the command so that after deploying the firmware your Duo will run into Safe Mode. E.g.:

    DuoSetupCLI --upload -f duo-system-part1-v0.2.3.bin -f duo-system-part2-v0.2.3.bin -s -l

- Command(e.g.):

        DuoSetupCLI --upload -f duo-user-part-v0.2.3.bin -l

- Report(e.g.):

		Upload image duo-user-part.bin to 0x0 of the OTA region.
		Open file "duo-user-part.bin"
		Check file validity:
		
		        Target Platform         : RedBear Duo
		        Module function         : User part
		        Module index            : 1
		        Module dependency       : System part
		        Module dependency index : 2
		        Module start address    : 0x80c0000
		        Module end address      : 0x80c183c
		
		File length : 6208
		Upload  [=========================] 100% 6208 bytes
		Upload done.
		Selected file is uploaded successfully.
	
	    Leave listening mode...

#### Fetch Firmware Version

- Command:  

        DuoSetupCLI --version

- Report(e.g.):

        Fetch firmware versions.

            Release      : 0.2.3
            Bootloader   : 3
            System part1 : 6
            System part2 : 6
            User part    : 6

#### Fetch Device ID

- Command:   

        DuoSetupCLI --device-id

- Report(e.g.):   

	    Fetch device ID.
	
	        Device ID : 24004xxxxC47353033323637
	        Claimed   : NO


#### Check Credentials

- Command: 

        DuoSetupCLI --credentials

- Report(e.g.):

	    Check credential.
	
	        Has credentials : YES

#### Scan AP

- Command: 

        DuoSetupCLI --scan-ap

- Report(e.g.):

	    Scan Access Points.
	
	        SSID             RSSI    Security      Channel  MDR
	        ----             ----    --------      -------  ---
	    01. Dong mac         -53dBm  WPA2_AES_PSK     11    216KB/s
	    02. AP-02_2.4G       -59dBm  WPA2_AES_PSK     5     300KB/s
	    03. RedBear          -41dBm  WPA2_MIXED_PSK   6     130KB/s

#### Connect AP

- Command:

        DuoSetupCLI --connect-ap

- Report:

	    Connect to Access Point.
	
	        Result : 0

#### Fetch Device Public Key

- Command:

        DuoSetupCLI --public-key

- Report(e.g.):

	    Fetch device public key.
	
	        Device public key : 308123546754567......
    
## Build DuoSetupCLI
---

#### Dependencies

Build tools and GCC Tools chain

#### Build
Git clone the repository or download it to your local system. Open command line terminal and change the working directory to `./DuoSetupCLI`.

Run `make` to build the `DuoSetupCLI`. The built program is under `./DuoSetupCLI/build`.

Run `make verbose=1` will build the program and print building details.

Run `make clean` to clean all object files and the built program.

## License
---

Copyright (c) 2016 Red Bear

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


  
