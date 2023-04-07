#include "videosocialcontrol.h"
#include <iostream>
#include <utility>
#include <time.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

extern "C" {
    #include <libavutil/timestamp.h>
    #include <libavformat/avformat.h>
}

using json = nlohmann::json;

VideoSocialControl::VideoSocialControl(): m_rtmp(new RtmpClient)
{
    m_socket.Connect();
}

//注册
void VideoSocialControl::registerAccount(QString password, QString nickname)
{
    std::cout << "-------注册帐号------\n";
    json js;
    js["type"] = "register";

    json data;
//    data["id"] = id;
    data["password"] = password.toStdString();
    data["nickname"] = nickname.toStdString();
    js["data"] = data;

    m_socket.send(js);
}

QJsonObject VideoSocialControl::transition(json js)
{
    std::string info = js.dump();
    const char *ch=info.data();

    QByteArray byte_array;
    QJsonParseError json_error;
    QJsonObject rootObj;   //最后转换出来的json 对象
    QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_array.append(ch), &json_error);
    if(json_error.error == QJsonParseError::NoError) {
           if(parse_doucment.isObject())
           {
               rootObj = parse_doucment.object();
           }
    } else {
        qDebug()<<json_error.errorString();
    }
    return rootObj;
}

//登录
QJsonObject VideoSocialControl::login(QString id, QString key)
{
    std::cout << "-------登录帐号------\n";
    nlohmann::json js;
    js["type"] = "login";

    nlohmann::json info;
    info["id"] = id.toStdString();
    info["key"] = key.toStdString();
    js["data"] = info;
    m_socket.send(js);

    js = m_socket.receive();
    return transition(js["netizen"]);
}

QJsonObject VideoSocialControl::getSomeVideos()
{
    std::cout << "-------获取网民所有稿件------\n";
    nlohmann::json js;
    js["type"] = "getSomeVideos";
    nlohmann::json data;
    js["data"] = data;

    m_socket.send(js);

    js = m_socket.receive();
    return transition(js);
}

QJsonObject VideoSocialControl::loadVideo(QString manuscriptId, QString videoId)
{
    std::cout << "-------播放视频------\n";
    nlohmann::json js;
    js["type"] = "loadVideo";
    nlohmann::json data;
    data["id"] = manuscriptId.toStdString();
    js["data"] = data;

    m_socket.send(js);

    js = m_socket.receive();    //从系统服务器中获取到稿件相关的信息

    std::string videoUrl = js["videoAddress"];
    std::thread pullThread(&RtmpClient::pullStreaming, m_rtmp.get(), videoUrl, videoId.toStdString());   //单独创建一个线程，从rtmp流媒体服务器拉流
    pullThread.detach();

    std::string videoPath = "file:///tmp/testrtmp.flv"/* + videoId.toStdString() + ".flv"*/;   //这个视频地址为客户端拉流时缓存到客户端的一个视频
    js["videoAddress"] = videoPath;
    std::cout << js.dump(4) << std::endl;
    return transition(js);
}

void VideoSocialControl::closeConnect()
{
    std::cout << "拉流断开！！！！！\n";
    m_rtmp->closeConnect();   //客户端与流媒体服务器断开连接
}

QJsonObject VideoSocialControl::getVideoAndManuscriptId()
{
    std::cout << "-------生成视频Id和稿件Id------\n";
    nlohmann::json js;
    js["type"] = "genVideoIdAndManuscriptId";
    nlohmann::json data;
    js["data"] = data;

    m_socket.send(js);

    js = m_socket.receive();

    return transition(js);
}

std::pair<std::string, std::string> VideoSocialControl::mergeVideoFiles(std::vector<std::string> videoFiles)
{
/*
    //合并视频，保存一个本地的合并后的视频文件地址
    qDebug() << "正在进行合并";
    //处理导入文件的路径
    QList<QString> filelist;
    for(int i = 0; i < fileList.length(); i++)
    {
        filelist.push_back(fileList[i].right(fileList[i].length()-7));
        qDebug()<<"filelist[i] = "<<filelist[i];
    }

    //生成临时文件名
    QList<QString> outpathNames;
    for (int i = 0;i < filelist.length();i++)
    {
        QString fileName = "out" + QString::number(i,10) + ".mp4";
        outpathNames.push_back(fileName);
        qDebug()<<fileName;
    }

    //获取第一个视频的宽高
    QString temp = filelist[0];
    AVFormatContext *fmtCtx = nullptr;
    int width, height;
    std::string tr;
    QString fileFormat;
    tr= temp.toStdString();
    const char *in_filename = tr.data();
    int ret = avformat_open_input(&fmtCtx, in_filename, NULL, NULL);
    if(ret) {
        qDebug()<<"open input failed";
    }
    ret = avformat_find_stream_info(fmtCtx, NULL);
    int videoIndex = av_find_best_stream(fmtCtx,  AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    width = fmtCtx->streams[videoIndex]->codec->width;
    height = fmtCtx->streams[videoIndex]->codec->height;
    fileFormat = fmtCtx->streams[videoIndex]->codecpar->format;

    //存储临时文件
    QString width1 = QString::number(width, 10);
    QString height1 = QString::number(height, 10);

    //在转换之前将之前out1,ou2,...文件给删掉 第一次执行不会删
    for(int del = 0; del < filelist.length(); del++)
    {
        QString file = "rm ./out" + QString::number(del, 10) + ".mp4";
        const char *rmFile = file.toStdString().c_str();
        system(rmFile);
    }

    //进行格式转换，存储为临时文件
    for(int i = 0; i < filelist.length();i++)
    {
        std::string cmd = "ffmpeg -i " + (filelist[i]).toStdString() + "  -vf scale="+ (width1).toStdString()+ ":" + (height1).toStdString() + " ./" + (outpathNames[i]).toStdString();
        const char* cmd1 = cmd.c_str();
        system(cmd1);
    }

    QFile file("in.txt");

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return -1;
    }
    QTextStream stream(&file);

    for(int i = 0; i < outpathNames.length(); i++)
    {
        stream << "file \'" <<"./"<< outpathNames[i] << "\'"<< "\n";
        qDebug()<<"file \'" <<"./"<< outpathNames[i] << "\'";
    }


    cmd = "ffmpeg -f concat -safe 0 -i ./in.txt -c copy  ./新视频1.mp4 -y" ;
    char* ch;
    QByteArray ba = cmd.toLatin1(); // must
    ch=ba.data();
    QProcess *process = new QProcess;
    connect(process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(domove(int, QProcess::ExitStatus)));
    qDebug()<<"cmd is "<< cmd;
    if(process-> state() != process->NotRunning)
    {
        process->waitForFinished();
    }

    process->start(cmd);

    return 0;

*/
    //生成一个新的VideoFile的id

    //返回视频id
}

std::pair<std::string, std::string> VideoSocialControl::changeVideoFile(std::vector<std::string> videoFiles)
{
    mergeVideoFiles(videoFiles);

    //清理上次生成的视频
}


void VideoSocialControl::publishManuscript(QJsonObject publishInfo)
{
    nlohmann::json js = getJsonFromJsonObject(publishInfo);
    std::string videoId = js["videoId"];
    std::string s = js["videoPath"];
//    std::string videoName;
//    for (int i = s.size()-1; i >= 0; i--) {
//        if (s[i] == '/') {
//            videoName = s.substr(i+1, s.size()-i+1);
//            std::cout << videoName << std::endl;
//            break;
//        }
//    }


    convertVideoFormat(s);   //转换视频格式，将mp4,mkv,avi,mpg转换为flv
    std::cout << s << std::endl;
    if (m_rtmp->pushStreaming(videoId, s) < 0) {
        std::cerr << "rtmp推流失败，发布稿件结束!\n";
        return;
    }

    nlohmann::json m;
    nlohmann::json dat;
    m["type"] = "publishManuscript";

    dat["videoId"] = videoId;
    dat["manuscriptId"] = js["manuscriptId"];
    dat["description"] = js["description"];
    dat["title"] = js["title"];
    dat["label"] = js["label"];
    dat["subarea"] = js["subarea"];
    dat["isOriginal"] = js["isOriginal"];
    dat["date"] = js["date"];
    dat["cover"] = js["cover"];
    dat["videoAddress"] = "rtmp://192.168.43.150:1936/live/" + videoId;
    dat["netizenId"] = js["netizenId"];

    m["data"] = dat;

    m_socket.send(m);
}

void VideoSocialControl::convertVideoFormat(std::string &s)
{
    int i = s.size()-1;
    for ( ; i >= 0; i--) {
        if (s[i] == '.')
           break;
    }

    std::string sub = s.substr(i+1, s.size()-i);
    std::string newName = s.substr(0, i) + ".flv";
    std::cout << sub << "    " << newName << std::endl;
    std::string cmd;

    if (sub == "flv") return;

    if (sub == "mp4") {
        cmd = "ffmpeg -i "+ s +" -vcodec copy -acodec copy -flvflags add_keyframe_index " + newName;
        system(cmd.c_str());
    } else if (sub == "mpg") {
        cmd = "ffmpeg -i " + s + " " + newName;
        system(cmd.c_str());
    } else if (sub == "avi") {
        cmd = "ffmpeg -i " + s + " -f flv "+ newName;
        system(cmd.c_str());
    } else if (sub == "mkv") {
        cmd = "ffmpeg -i " + s + " -f flv "+ newName;
        system(cmd.c_str());
    }

    s = newName;
}

void VideoSocialControl::publishThread(QJsonObject publishInfo)
{
    std::cout << "-------发布稿件------\n";
    std::thread publish(&VideoSocialControl::publishManuscript, this, publishInfo);
    publish.detach();
}

void VideoSocialControl::focusOn(QString fanId, QString followerId, QString followerNickname)
{
    std::cout << "-------关注up主------\n";
    nlohmann::json js;
    js["type"] = "focusOn";

    nlohmann::json data;
    data["fanId"] = fanId.toStdString();
    data["followerId"] = followerId.toStdString();
    data["followerNickname"] = followerNickname.toStdString();
    js["data"] = data;

    m_socket.send(js);
}

void VideoSocialControl::takeOff(QString fanId, QString followerId)
{
    std::cout << "-------取消关注------\n";
    nlohmann::json js;
    js["type"] = "takeOff";

    nlohmann::json data;
    data["fanId"] = fanId.toStdString();
    data["followerId"] = followerId.toStdString();
    js["data"] = data;

    m_socket.send(js);
}


void VideoSocialControl::modifyHeadportrait(const QString &netizenId, const QString &newHeadportrait)
{
    std::cout << "-------修改头像------\n";
    nlohmann::json js;
    js["type"] = "modifyHeadportrait";

    nlohmann::json data;
    data["netizenId"] = netizenId.toStdString();
    data["newHeadportrait"] = newHeadportrait.toStdString();
    js["data"] = data;

    m_socket.send(js);
}

void VideoSocialControl::modifyNickname(const QString &netizenId, const QString &newNickname)
{
    std::cout << "-------修改网名------\n";
    nlohmann::json js;
    js["type"] = "modifyNickname";

    nlohmann::json data;
    data["netizenId"] = netizenId.toStdString();
    data["newNickname"] = newNickname.toStdString();
    js["data"] = data;

    m_socket.send(js);
}

bool VideoSocialControl::modifyPassword(const QString &netizenId, const QString &oldPassword, const QString &newPassword)
{
    std::cout << "-------修改密码------\n";
    nlohmann::json js;
    js["type"] = "modifyPassword";

    nlohmann::json data;
    data["netizenId"] = netizenId.toStdString();
    data["oldPassword"] = oldPassword.toStdString();
    data["newPassword"] = newPassword.toStdString();
    js["data"] = data;

    m_socket.send(js);
    js = m_socket.receive();
    std::cout << js["flag"].get<bool>() << std::endl;
    if (js["flag"].get<bool>() == 0) {
        return false;
    }
    //判断
    return true;
}

nlohmann::json VideoSocialControl::getJsonFromJsonObject(QJsonObject qjson)
{
    QString str = QString(QJsonDocument(qjson).toJson());
    std::string strjson = str.toStdString();
    nlohmann::json json = nlohmann::json::parse(strjson);
    return json;
}

void VideoSocialControl::modifyManuscriptInfo(const QString &netizenId, QJsonObject newManuscriptInfo)
{
    std::cout << "-------修改稿件信息------\n";
    nlohmann::json js;
    js["type"] = "modifyManuscriptInfo";

    nlohmann::json data;
    data["netizenId"] = netizenId.toStdString();
    data["newManuscriptInfo"] = getJsonFromJsonObject(newManuscriptInfo);
    js["data"] = data;

    m_socket.send(js);
}

void VideoSocialControl::deleteManuscript(const QString &netizenId, const QString &manuscriptId)
{
    std::cout << "-------删除稿件------\n";
    nlohmann::json js;
    js["type"] = "deleteManuscript";

    nlohmann::json data;
    data["netizenId"] = netizenId.toStdString();
    data["manuscriptId"] = manuscriptId.toStdString();
    js["data"] = data;

    m_socket.send(js);
}

void VideoSocialControl::commentManuscript(const QString &netizenId, const QString &manuscriptId, QString &text)
{
    std::cout << "-------发布评论------\n";
    nlohmann::json js;
    js["type"] = "commentManuscript";

    nlohmann::json data;
    data["netizenId"] = netizenId.toStdString();
    data["manuscriptId"] = manuscriptId.toStdString();
    data["text"] = text.toStdString();
    js["data"] = data;

    m_socket.send(js);
}

void VideoSocialControl::deleteComment(const QString &manuscriptId, const QString &commentId)
{
    std::cout << "-------删除评论------\n";
    nlohmann::json js;
    js["type"] = "deleteComment";

    nlohmann::json data;
    data["manuscriptId"] = manuscriptId.toStdString();
    data["commentId"] = commentId.toStdString();
    js["data"] = data;

    m_socket.send(js);
}

QJsonObject VideoSocialControl::loadManuscript(QString id)
{
    nlohmann::json js;
    js["type"] = "loadManuscript";
    nlohmann::json data;
    data["manuscriptId"] = id.toStdString();
    js["data"] = data;

    m_socket.send(js);

    js = m_socket.receive();

    return transition(js);
}



