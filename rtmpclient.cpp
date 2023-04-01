#include "rtmpclient.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <iostream>

#include <librtmp/rtmp.h>
// 大小端字节序转换
#define HTON16(x) ( (x >> 8 & 0x00FF) | (x << 8 & 0xFF00) )
#define HTON24(x) ( (x >> 16 & 0x0000FF) | (x & 0x00FF00) | (x << 16 & 0xFF0000) )
#define HTON32(x) ( (x >> 24 & 0x000000FF) | (x >> 8 & 0x0000FF00) | (x << 8 & 0x00FF0000) | (x << 24 & 0xFF000000) )
#define HTONTIME(x) ( (x >> 16 & 0x000000FF) | (x & 0x0000FF00) | (x << 16 & 0x00FF0000) | (x & 0xFF000000) )

// 从文件读取指定字节
bool RtmpClient::ReadFP(char* pBuf, int nSize, FILE* pFile)
{
    return (fread(pBuf, 1, nSize, pFile) == nSize);
}

// 从文件读取1个字节整数
bool RtmpClient::ReadU8(uint8_t* u8, FILE* fp)
{
    return ReadFP((char*)u8, 1, fp);
}

// 从文件读取2个字节整数
bool RtmpClient::ReadU16(uint16_t* u16, FILE* fp)
{
    if (!ReadFP((char*)u16, 2, fp))
        return false;

    *u16 = HTON16(*u16);
    return true;
}

// 从文件读取3个字节整数
bool RtmpClient::ReadU24(uint32_t* u24, FILE* fp)
{
    if (!ReadFP((char*)u24, 3, fp))
        return false;

    *u24 = HTON24(*u24);
    return true;
}

// 从文件读取4个字节整数
bool RtmpClient::ReadU32(uint32_t* u32, FILE* fp)
{
    if (!ReadFP((char*)u32, 4, fp))
        return false;

    *u32 = HTON32(*u32);
    return true;
}

// 从文件读取4个字节时间戳
bool RtmpClient::ReadTime(uint32_t* utime, FILE* fp)
{
    if (!ReadFP((char*)utime, 4, fp))
        return false;

    *utime = HTONTIME(*utime);
    return true;
}

// 从文件预读1个字节整数
bool RtmpClient::PeekU8(uint8_t* u8, FILE* fp)
{
    if (!ReadFP((char*)u8, 1, fp))
        return false;

    fseek(fp, -1, SEEK_CUR);
    return true;
}

RtmpClient::RtmpClient():m_rtmp(nullptr)
{
    m_rtmp = RTMP_Alloc();  //分配RTMP上下文
}

int RtmpClient::pushStreaming(std::string videoId, std::string videoPath)
{
    std::cout << "videoPath :" << videoPath << std::endl;
    FILE* pFile = fopen(videoPath.c_str(), "rb");
    if (!pFile) {
        std::cerr << "open file failed!\n";
        return -1;
    }

    RTMP_Init(m_rtmp);  //初始化rtmp上下文，设默认值

    //设置推流地址
    m_rtmp->Link.timeout = 10;
    std::string pushStreamUrl = "rtmp://192.168.43.150:1936/live/" + videoId;
    RTMP_SetupURL(m_rtmp, pushStreamUrl.data());

    //开启推流标志
    RTMP_EnableWrite(m_rtmp);

    //连接服务器
    bool b = RTMP_Connect(m_rtmp, NULL);
    if (!b) {
        std::cerr << "connect failed! \n";
        return -1;
    }

    //连接流地址
    b = RTMP_ConnectStream(m_rtmp, 0);
    if (!b) {
        std::cerr << "connect stream failed! \n";
        return -1;
    }

    //跳过FLV文件头的13个字节
    fseek(pFile, 9, SEEK_SET);
    fseek(pFile, 4, SEEK_CUR);

    //初始化RTMP报文
    RTMPPacket packet;
    RTMPPacket_Reset(&packet);
    packet.m_body = nullptr;
    packet.m_chunk = nullptr;

    packet.m_nInfoField2 = m_rtmp->m_stream_id;
    uint32_t starttime = RTMP_GetTime();

    while (true) {
        // 读取TAG头

        uint8_t type = 0;
        if (!ReadU8(&type, pFile)) break;

        uint32_t datalen = 0;
        if (!ReadU24(&datalen, pFile)) break;

        uint32_t timestamp = 0;
        if (!ReadTime(&timestamp, pFile)) break;

        uint32_t streamid = 0;
        if (!ReadU24(&streamid, pFile)) break;

        RTMPPacket_Alloc(&packet, datalen);

        if (fread(packet.m_body, 1, datalen, pFile) != datalen) break;

        // 组织报文并发送
        packet.m_headerType = RTMP_PACKET_SIZE_LARGE;
        packet.m_packetType = type;
        packet.m_hasAbsTimestamp = 0;
        packet.m_nChannel = 6;
        packet.m_nTimeStamp = timestamp;
        packet.m_nBodySize = datalen;

        if (!RTMP_SendPacket(m_rtmp, &packet, 0))
        {
            std::cout << "Send Error! \n";
            break;
        }

        printf("send type:[%d] timestamp:[%d] datasize:[%d] \n", type, timestamp, datalen);

        // 跳过PreTag
        uint32_t pretagsize = 0;
        if (!ReadU32(&pretagsize, pFile)) break;

        // 延时，避免发送太快
        uint32_t timeago = (RTMP_GetTime() - starttime);
        if (timestamp > 1000 && timeago < timestamp - 1000)
        {
            printf("sleep...\n");
            usleep(100000);
        }

        RTMPPacket_Free(&packet);
    }

    RTMP_Close(m_rtmp);  //关闭连接

    fclose(pFile);
    return 0;
}

void RtmpClient::pullStreaming(std::string videoUrl, std::string videoId)
{
    RTMP_Init(m_rtmp);

    // 设置拉流地址
    RTMP_SetupURL(m_rtmp, videoUrl.data());

    // 连接服务器
    m_rtmp->Link.timeout = 10;
    m_rtmp->Link.lFlags |= RTMP_LF_LIVE;
    bool b = RTMP_Connect(m_rtmp, NULL);
    if (!b)
    {
        std::cout << "connect failed! \n";
        return;
    }

    // 连接流地址
    b = RTMP_ConnectStream(m_rtmp, 0);
    if (!b)
    {
        std::cout <<"connect stream failed! \n";
        return;
    }

    std::string path = "/tmp/" + videoId + ".flv";
    FILE* pFile = fopen(path.c_str(), "wb");
    while (RTMP_IsConnected(m_rtmp)) {
        char sBuf[4096] = {0};
        int bytes = RTMP_Read(m_rtmp, sBuf, sizeof(sBuf));
        printf("RTMP_Read() ret:[%d] \n", bytes);

        if (bytes <= 0) break;

        fwrite(sBuf, 1, bytes, pFile);
    }
    fclose(pFile);

    RTMP_Close(m_rtmp); //关闭连接

}

void RtmpClient::closeConnect()
{
    RTMP_Close(m_rtmp); //关闭连接
}
