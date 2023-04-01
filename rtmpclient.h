#ifndef RTMPCLIENT_H
#define RTMPCLIENT_H

#include <librtmp/rtmp.h>
#include <librtmp/log.h>
#include <string>

class RtmpClient
{
public:
    RtmpClient();
    int pushStreaming(std::string videoId, std::string videoPath);
    void pullStreaming(std::string videoUrl, std::string videoId);
    void closeConnect();
private:
    RTMP* m_rtmp;
    bool ReadFP(char* pBuf, int nSize, FILE* pFile);
    bool ReadU8(uint8_t* u8, FILE* fp);
    bool ReadU16(uint16_t* u16, FILE* fp);
    bool ReadU24(uint32_t* u24, FILE* fp);
    bool ReadU32(uint32_t* u32, FILE* fp);
    bool ReadTime(uint32_t* utime, FILE* fp);
    bool PeekU8(uint8_t* u8, FILE* fp);
};

#endif // RTMPCLIENT_H
