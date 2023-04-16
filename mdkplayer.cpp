#include "mdkplayer.h"
#include <unistd.h>
#include <QObject>

MDKPlayer::MDKPlayer(QQuickItem *parent):
    QQuickFramebufferObject(parent),
    m_player(new Player)
{

    setMirrorVertically(true);
    m_player->set(State::Paused);

    m_position = 0;
    m_timer = new QTimer;
    m_timer->setInterval(500);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));  //循环触发定时器，触发onTimeOut槽函数，在其中修改m_position的值
}

MDKPlayer::~MDKPlayer()
{
    m_timer->stop();
    delete m_player;
}

QQuickFramebufferObject::Renderer *MDKPlayer::createRenderer() const
{
    return new VideoRendererInternal(const_cast<MDKPlayer*>(this));
}

void MDKPlayer::seek(qint64 ms, bool accurate)
{
    ++m_seekNum;  //由于slider每次按下会触发两次，所以我们只考虑第二次触发
    if (m_seekNum == 2) {
        auto flags = SeekFlag::FromStart;
        if (!accurate) flags |= SeekFlag::KeyFrame;
        bool b = m_player->seek(ms, flags);
        qDebug() << b;
        m_seekNum = 0;
     //   m_timer->start();
    }
}

void MDKPlayer::play()
{
    m_player->set(State::Playing);
    m_player->setRenderCallback([=](void *){
        QMetaObject::invokeMethod(this, "update");
    });
    m_timer->start();

    m_duration = m_player->mediaInfo().duration;
    qDebug() << m_duration;
}

void MDKPlayer::pause()
{
    m_player->set(State::Paused);
    m_timer->stop();
}

bool MDKPlayer::isPlay()
{
    return m_player->state() == State::Playing;
}

bool MDKPlayer::isPause()
{
    return m_player->state() == State::Paused;
}

void MDKPlayer::setPlaybackRate(float rate)
{
    m_player->setPlaybackRate(rate);
}

void MDKPlayer::setVideoSurfaceSize(int width, int height)
{
    m_player->setVideoSurfaceSize(width, height);
}

void MDKPlayer::renderVideo()
{
    m_player->renderVideo();

}


qint64 MDKPlayer::position() const
{
    return m_position;
}
qint64 MDKPlayer::duration() const
{
    return m_position;
}

void MDKPlayer::onTimeOut()
{
    m_position = m_player->position();
    emit positionChanged();
}

