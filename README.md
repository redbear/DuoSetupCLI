# Usage
---

DuoSetupCLI is a command line interface program for setting up Duo. Power on your Duo and make it enter the listening mode. The Duo will act as an AP, e.g. "Duo-xxxx" and you can see it in the AP scanned list on your computer. Connect your computer to the AP which your Duo is brodcasting.

    Usage: DuoSetupCLI <option> [parameters] 
	
    <options>:
        --upload          Upload firmware to Duo
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
                          (.bin) to be uploaded. The file will be stored from the
                          beginning of the OTA region by default, if no "--region"
                          is present
        --region,-r <n>   Used with --upload option. Specify the region of
                          the external flash to store the file. Without this
                          parameter, the file is stored from the beginning of
                          the OTA region. Otherwise, if n=[1...7], the file
                          is stored from the offset of the OTA region, where
                          the offset is n*64KB. if n=8, the file is stored
                          at the Factory Reset region
        --safe,-s         Used with --upload option. Invalid user part so that
                          Duo enter safe mode after firmware update
        --leave,-l        Used with --upload option. Leave listening mode when
                          uploading firmware completed

### OTA(Over The Air) Update Firmware

The binary image you are going to upload is stored at the begining of the OTA region of the external flash by default, i.e. "--region 0", if no "--region" presented. Thus, every time you upload a binary image, you have to run the command with "--leave" parameter to make Duo leave listening mode and perform a soft reset to deploy the firmware.    

If you are going to upload a factory reset (FAC) image to Duo, you have to run the command with setting the "--region" to **8** to specify that this image is going to be stored in FAC region, not the OTA region. Since the FAC image is stored in external flash directly, you don't need to leave the listening mode to deploy the FAC firmware.    

Then what if I want to upload the system part1, system part 2 and user part within the same connection, and then leave the listening mode to deploy all of them one time? Using the "--region" parameter in the command is important! If you keep uploading new image without leaving listening mode and speicfying the "--region", the image you just uploaded before will be overrode. The OTA region, of which size is 512KB, is separated into 8 sub-regions, of which size is 64KB. You need to know how many sub-regions are occupied by the images you uploaded before, then you can upload a new image from a free sub-region. E.g., if you have uploaded an image, of which size is 65KB(64KB+1KB), from the begining of the OTA region, then the sub-region 0 and 1 are not availabe any more before leaving listening mode. If you're going to upload a new image by the next, you must set the "--region" to 2 to not override the images stored before.

Sometimes the new released firmware version is not compatible with the old application running on your Duo. In this case, you had better use the "--safe" parameter in the command so that after leaving the listening mode the Duo will deploy the firmware first and then run into Safe Mode.

Command:

* OTA update system part1 of the firmware and then leave listening mode to deploy the new firmware:    

        DuoSetupCLI --upload -f duo-system-part1-v0.2.3.bin -l

* OTA update system part2 of the firmware and then leave listening mode to deploy the new firmware:

        DuoSetupCLI --upload -f duo-system-part2-v0.2.3.bin -l

* OTA update user part application and then leave the listening mode to run the new application:

        DuoSetupCLI --upload -f duo-user-part2-v0.2.3.bin -l

* OTA update the Factory Reset Firmware. We must set the "--region" parameter to **8** to specify the location where the FAC firmware to be stored. The Factory Reset Firmware is under deploying during uploading progress, so it doesn't need to leave listening mode to deploy it again:

        DuoSetupCLI --upload -f duo-fac-tinker-v0.2.3.bin -r 8

* OTA upldate system part1(assume it 38KB), system part 2(assume it 281KB) and user part(assume it 7KB) and then leave the listening mode to deploy all of them one time:

        DuoSetupCLI --upload -f duo-system-part1-v0.2.3.bin 
        DuoSetupCLI --upload -f duo-system-part2-v0.2.3.bin -r 1
        DuoSetupCLI --upload -f duo-user-part-v0.2.3.bin -r 6 -l

    or

        DuoSetupCLI --upload -f duo-system-part2-v0.2.3.bin 
        DuoSetupCLI --upload -f duo-system-part1-v0.2.3.bin -r 5
        DuoSetupCLI --upload -f duo-user-part-v0.2.3.bin -r 6 -l

    or

        DuoSetupCLI --upload -f duo-user-part-v0.2.3.bin 
        DuoSetupCLI --upload -f duo-system-part1-v0.2.3.bin -r 1
        DuoSetupCLI --upload -f duo-system-part2-v0.2.3.bin -r 2 -l

    etc. The order of executing these commands does not matter, as long as they do not override each other and finally leave the listening mode. It is little complex to operate the multi-firmware uploading. It should be fixed later -- How about `DuoSetupCLI --upload -f image1.bin -f image2.bin -f image3.bin -l` ?

Report(e.g.):

    Upload firmware to Duo.
    Open file "Blink.bin"
    Check file validity:

        Platform ID             : 88
        Module function         : MOD_FUNC_USER_PART
        Module index            : 1
        Module dependency       : MOD_FUNC_SYSTEM_PART
        Module dependency index : 2
        Module start address    : 0x80c0000
        Module and address      : 0x80c0c44

    File length : 3144
    File will be stored from offset 0x00 0f the OTA region.
    Upload  [==========================] 100%  3144 bytes
    Upload done.
    Selected file is uploaded successfully.

    Leave listening mode...

### Fetch Firmware Version

Command:  

    DuoSetupCLI --version

Report(e.g.):

    Fetch firmware versions.

        Release      : 0.2.3
        Bootloader   : 3
        System part1 : 6
        System part2 : 6
        User part    : 6

### Fetch Device ID

Command:   

    DuoSetupCLI --device-id

Report(e.g.):   

    Fetch device ID.

        Device ID : 24004xxxxC47353033323637
        Claimed   : NO


### Check Credentials

Command: 

    DuoSetupCLI --credentials

Report(e.g.):

    Check credential.

        Has credentials : YES

### Scan AP

Command: 

    DuoSetupCLI --scan-ap

Report(e.g.):

    Scan Access Points.

        SSID             RSSI    Security      Channel  MDR
        ----             ----    --------      -------  ---
    01. Dong mac         -53dBm  WPA2_AES_PSK     11    216KB/s
    02. AP-02_2.4G       -59dBm  WPA2_AES_PSK     5     300KB/s
    03. RedBear          -41dBm  WPA2_MIXED_PSK   6     130KB/s

### Config AP (*TBD*)

### Connect AP

Command:

    DuoSetupCLI --connect-ap

Report:

    Connect to Access Point.

        Result : 0

### Fetch Device Public Key

Command:

    DuoSetupCLI --public-key

Report(e.g.):

    Fetch device public key.

        Device public key : 308123546754567......
    
# Build DuoSetupCLI
---

## Dependencies

Build tools and GCC Tools chain

## Build
Git clone the repository or download it to your local system. Open command line terminal and change the working directory to `./DuoSetupCLI`.

Run `make` to build the `DuoSetupCLI`. The built program is under `./DuoSetupCLI/build`.

Run `make verbose=1` will build the program and print building details.

Run `make clean` to clean all object files and the built program.

# License
---

Copyright (c) 2016 Red Bear

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


  
