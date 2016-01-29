#ifndef UPLOAD_H
#define UPLOAD_H

#define MAX_FILE_LENGTH               (512*1024)

int PrepareUpload(char *file_name, uint8_t *fileData, uint32_t *fileLength);
int OTAUploadFirmware(uint8_t *firmware, uint32_t fileLen, uint16_t chunkSize);


#endif

