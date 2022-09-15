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

VideoSocialControl::VideoSocialControl()
{
    m_socket.Connect();
}

//注册
void VideoSocialControl::registerAccount(QString password, QString nickname)
{
    //利用boost/uuid库生成uuid
    boost::uuids::random_generator gen;
    boost::uuids::uuid  uid = gen();

    std::string id = to_string(uid);

    json js;
    js["type"] = "register";

    json data;
    data["id"] = id;
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
    nlohmann::json js;
    js["type"] = "getSomeVideos";
    nlohmann::json data;
    js["data"] = data;

    m_socket.send(js);

    js = m_socket.receive();
    return transition(js);
}

QJsonObject VideoSocialControl::loadVideo(QString id)
{
    nlohmann::json js;
    js["type"] = "loadVideo";
    nlohmann::json data;
    data["id"] = id.toStdString();
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



QJsonObject VideoSocialControl::publishManuscript(QString description, QString title, QString label,
                                     QString subarea, QString isOriginal, QString cover, QString date,
                                     QString netizenId, QString videoAddress)
{
    std::string s = videoAddress.toStdString();
    std::string videoName;
    for (int i = s.size()-1; i >= 0; i--) {
        if (s[i] == '/') {
            videoName = s.substr(i+1, s.size()-i+1);
            std::cout << videoName << std::endl;
            break;
        }
    }
    FILE* fp;
    if( ( fp = fopen(s.c_str(),"rb") ) == nullptr ){
        printf("File open.\n");
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long nlen = ftell(fp);
    fseek(fp, 0, 0);
    fclose(fp);
    printf("%d\n", nlen);

    nlohmann::json js;
    js["type"] = "sendVideo";
    nlohmann::json data;
    data["videoName"] = videoName;
    data["videoLen"] = std::to_string(nlen);
    js["data"] = data;
    m_socket.send(js);
    sleep(2);

    m_socket.sendVideo(videoAddress.toStdString());

    nlohmann::json address = m_socket.receive();
//    std::cout << address.dump(4)<< std::endl;

    nlohmann::json m;
    nlohmann::json dat;
    m["type"] = "publishManuscript";

    dat["description"] = description.toStdString();
    dat["title"] = title.toStdString();
    dat["label"] = label.toStdString();
    dat["subarea"] = subarea.toStdString();
    dat["isOriginal"] = isOriginal.toStdString();
    dat["cover"] = cover.toStdString();
    dat["date"] = date.toStdString();
    dat["videoAddress"] = address["videoAddress"];
    dat["netizenId"] = netizenId.toStdString();

    m["data"] = dat;

    m_socket.send(m);
    js = m_socket.receive();
    return transition(js);
}

void VideoSocialControl::focusOn(QString fanId, QString followerId, QString followerNickname)
{
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
    nlohmann::json js;
    js["type"] = "deleteComment";

    nlohmann::json data;
    data["manuscriptId"] = manuscriptId.toStdString();
    data["commentId"] = commentId.toStdString();
    js["data"] = data;

    m_socket.send(js);
}


