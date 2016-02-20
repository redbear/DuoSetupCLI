#ifndef _UPLOAD_H_
#define _UPLOAD_H_

#define MAX_FILE_LENGTH               (512*1024)
#define OTA_REGION_ADDR               (0x0)         // The address is relative to the OTA region
#define OTA_REGION_SEC1_ADDR          (0x20000)
#define OTA_REGION_SEC2_ADDR          (0x40000)
#define OTA_REGION_SEC3_ADDR          (0x60000)
#define FAC_REGION_ADDR               (0x140000)    // The address is absolute to the Factory Reset region

int PrepareUpload(char *file_name, uint8_t *fileData, uint32_t *fileLength);
int OTAUploadFirmware(uint8_t *firmware, uint32_t fileLen, uint16_t chunkSize);


#endif

