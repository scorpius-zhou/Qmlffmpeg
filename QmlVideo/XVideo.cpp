#include "XVideo.h"
#include <QPainter>
#include <QDebug>
#include "XVideoThread.h"

void XVideo::SetSize(int width, int height)
{
    nWidth = width;
    nHeight = height;
}

XVideo::XVideo():
nHeight(205),
nWidth(360)
{
    qDebug()<<"create";
    m_pVedioPlayer = new VideoPlayer();
    connect(m_pVedioPlayer, &VideoPlayer::sig_GetOneFrame, this, &XVideo::slot_GetOneFrame);
    connect(m_pVedioPlayer, SIGNAL(sig_totalTimeChanged(QString,qint64)), this, SLOT(slot_totalTimeChanged(QString,qint64)));
    connect(m_pVedioPlayer, SIGNAL(sig_currentTimeChanged(QString,qint64)), this, SLOT(slot_currentTimeChanged(QString,qint64)));
}

XVideo::~XVideo()
{
    qDebug()<<"distory";
    m_pVedioPlayer->deleteLater();

}

void XVideo::startPlay()
{
    m_pVedioPlayer->startPlay();
}

void XVideo::pausePlay()
{
    m_pVedioPlayer->pausePlay();
}

void XVideo::slot_GetOneFrame(QImage image)
{
    m_Frame = image.copy();
    update();
}

void XVideo::slot_totalTimeChanged(QString str, qint64 sec)
{
    setDuration(sec);
    emit sig_totalTimeChanged(str);
}

void XVideo::slot_currentTimeChanged(QString str, qint64 sec)
{
    setPosition(sec);
    emit sig_currentTimeChanged(str);
}

void XVideo::paint(QPainter *pPainter)
{

    if (!m_Frame.isNull())
    {
        pPainter->drawImage(QRect(0, 0, nWidth, nHeight), m_Frame);
    }
}

QString XVideo::getStrVideoPath() const
{
    return strVideoPath;
}

void XVideo::setStrVideoPath(const QString &value)
{
    emit strVideoPathChanged(value);
    strVideoPath = value;
    if (m_pVedioPlayer)
    {
        m_pVedioPlayer->setFileName(value);
        qDebug() << "XVideo::setStrVideoPath:" << value;
    }
}

int XVideo::getWidth() const
{
    return nWidth;
}

void XVideo::setWidth(int value)
{
    nWidth = value;
    emit widthChanged(value);
}

int XVideo::getHeight() const
{
    return nHeight;
}

void XVideo::setHeight(int value)
{
    nHeight = value;
    emit heightChanged(value);
}

int XVideo::getDuration() const
{
    return nDuration;
}

void XVideo::setDuration(int value)
{
    nDuration = value;
    emit durationChanged(value);
}

int XVideo::getPosition() const
{
    return nPosition;
}

void XVideo::setPosition(int value)
{
    nPosition = value;
    emit positionChanged(value);
}
