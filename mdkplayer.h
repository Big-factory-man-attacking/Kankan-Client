#ifndef MDKPLAYER_H
#define MDKPLAYER_H
#include <QQuickFramebufferObject>
#include <QOpenGLFramebufferObject>
#include <QDebug>
#include "mdk/Player.h"
#include <QTimer>

using namespace MDK_NS;
#include <QQuickFramebufferObject>

class MDKPlayer : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
public:
    explicit MDKPlayer(QQuickItem *parent = nullptr);
    virtual ~MDKPlayer();
    Renderer *createRenderer() const;

    Q_INVOKABLE QString source() { return m_source; }
    Q_INVOKABLE void seek(qint64 ms, bool accurate = false);
    Q_INVOKABLE void setSource(const QString & s) {
        m_player->setMedia(s.toUtf8().data());
        m_source = s;

        emit sourceChanged();
    }


    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE qint64 getDuration() {return m_duration;}
    Q_INVOKABLE bool isPlay();
    Q_INVOKABLE bool isPause();
    Q_INVOKABLE void setPlaybackRate(float rate);
    Q_INVOKABLE void setVideoSurfaceSize(int width, int height);

    void renderVideo();

    qint64 position() const;
    qint64 duration() const;
signals:
    void sourceChanged();
    void durationChanged();
    void positionChanged();

public slots:
    void onTimeOut();

private:
    QString m_source;
    qint64 m_duration;
    qint64 m_position;
    Player *m_player;
    QTimer* m_timer;
    int m_seekNum = 0;

};

class VideoRendererInternal : public QQuickFramebufferObject::Renderer
{
public:
    VideoRendererInternal(MDKPlayer *r) {
        this->r = r;
    }

    void render() override {
        r->renderVideo();
    }

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size) override {
        r->setVideoSurfaceSize(size.width(), size.height());
        return new QOpenGLFramebufferObject(size);
    }

    MDKPlayer *r;
};
#endif // MDKPLAYER_H
