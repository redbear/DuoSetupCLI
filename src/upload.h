#ifndef _UPLOAD_H_
#define _UPLOAD_H_

#define MAX_FILE_LENGTH         (512*1024)
#define OTA_REGION_UNIT_SIZE    (0x10000)
#define MAX_OTA_REGION_ADDR     (0x80000)
#define OTA_REGION_UNIT_CNT     (MAX_OTA_REGION_ADDR/OTA_REGION_UNIT_SIZE)
#define FAC_REGION_ADDR         (0x140000)
#define MAX_FAC_REGION_ADDR     (0x180000)

int PrepareUpload(char *file_name, uint8_t *fileData, uint32_t *fileLength);
int OTAUploadFirmware(uint8_t *firmware, uint32_t fileLen, uint16_t chunkSize);


#endif

