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
 
#ifndef _JSON_CMD_H_
#define _JSON_CMD_H_

void AssembleOtaCmdString(char *js, uint32_t file_len, uint32_t chunk_address, uint16_t chunk_size, uint8_t file_store);
void AssembleRstCmdString(char *js);
void AssembleVerCmdString(char *js);
void AssembleDevidCmdString(char *js);
void AssembleChkCredentCmdString(char *js); 
void AssembleScanApCmdString(char *js);
void AssembleConfigApCmdString(char *js, char *ssid, int32_t security, char *password);
void AssembleConnectApCmdString(char *js);
void AssemblePublicKeyCmdString(char *js);
void AssembleInvalidCmdString(char *js);
int SendJSONCmd(char *js, char *rsp, uint16_t rsp_len);


#endif

