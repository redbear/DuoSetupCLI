#ifndef JSON_CMD_H
#define JSON_CMD_H

void AssembleOtaCmdString(char *js, uint32_t file_len, uint32_t chunk_address, uint16_t chunk_size);
void AssembleRstCmdString(char *js);
void AssembleVerCmdString(char *js);
void AssembleDevidCmdString(char *js);
void AssembleChkCredentCmdString(char *js);
void AssembleScanApCmdString(char *js);
int SendJSONCmd(char *js, char *rsp, uint16_t rsp_len);


#endif

