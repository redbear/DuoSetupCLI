/*
 * Copyright (c) 2016 RedBear
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
 
#ifndef _UPLOAD_H_
#define _UPLOAD_H_

#define MAX_FILE_LENGTH         (512*1024)
#define OTA_REGION_UNIT_SIZE    (0x1000)
#define MAX_OTA_REGION_ADDR     (0x80000)
#define OTA_REGION_UNIT_CNT     (MAX_OTA_REGION_ADDR/OTA_REGION_UNIT_SIZE)
#define FAC_REGION_ADDR         (0x140000)
#define MAX_FAC_REGION_ADDR     (0x180000)

int PrepareUpload(char *file_name, uint8_t *fileData, uint32_t *fileLength);
int OTAUploadFirmware(uint8_t *firmware, uint32_t fileLen, uint16_t chunkSize);


#endif

