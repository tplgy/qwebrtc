#include "qwebrtcquickvideoitem_p.hpp"
#include <QDebug>
#include <assert.h>
#include <QPainter>
#include <webrtc/common_video/libyuv/include/webrtc_libyuv.h>
#include <QMutex>

QWebRTCQuickVideoItem::QWebRTCQuickVideoItem(QQuickItem *parent)
    : QQuickPaintedItem(parent), m_impl(new QWebRTCQuickVideoItem_p(this))
{
}

QWebRTCQuickVideoItem::~QWebRTCQuickVideoItem()
{
    std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
    m_impl->q_ptr = 0;
    if (m_impl->m_track && m_impl->m_track->videoTrack) {
        m_impl->m_track->videoTrack->RemoveSink(m_impl);
    }
    delete m_impl;
}

QObject* QWebRTCQuickVideoItem::videoTrack()
{
    std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
    return m_impl->m_track;
}

void QWebRTCQuickVideoItem::setVideoTrack(QObject* track)
{
    std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
    m_impl->m_sourceSize = QSize();
    auto trackImpl = qobject_cast<QWebRTCMediaTrack_impl*>(track);
    if (!trackImpl) {
        m_impl->m_frame.reset();
        //qWarning() << "incompatible object assigned as video track";
        return;
    }
    assert(trackImpl->trackType() == QWebRTCMediaTrack::Type::Video);
    m_impl->m_track = trackImpl;
    m_impl->m_track->videoTrack->AddOrUpdateSink(m_impl, rtc::VideoSinkWants());
    Q_EMIT videoTrackChanged();
}

void QWebRTCQuickVideoItem::paint(QPainter *painter)
{
    //painter->fillRect(boundingRect(), (m_impl->m_track == nullptr) ? Qt::blue : Qt::green);
    std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
    auto frame = m_impl->m_frame;
    if (m_impl->m_track != nullptr && frame) {
        size_t bufferSize = frame->width() * frame->height() * 4;
        uint8_t* buffer = new uint8_t[bufferSize];
        webrtc::ConvertFromI420(*frame, webrtc::kARGB, 0, buffer);
        QImage img = QImage(buffer, frame->width(), frame->height(), QImage::Format_ARGB32);
        //qDebug() << frame->rotation();

        int boundsWidth;
        int boundsHeight;
        int width;
        int height;

        if (frame->rotation() % 180 == 0) {
            boundsWidth = boundingRect().width();
            boundsHeight = boundingRect().height();
            width = qMax(boundsWidth, frame->width()*boundsHeight/frame->height());
            height = frame->height() * width/frame->width();
        } else {
            boundsWidth = boundingRect().height();
            boundsHeight = boundingRect().width();
            width = qMax(boundsWidth, frame->width()*boundsHeight/frame->height());
            height = frame->height() * width/frame->width();
        }

        painter->translate(boundingRect().width()/2, boundingRect().height()/2);
        painter->rotate(frame->rotation());
        painter->translate(-boundsWidth/2, -boundsHeight/2);
        painter->drawImage(QRect((boundsWidth-width)/2, (boundsHeight-height)/2, width, height), img);
    }
}

QSize QWebRTCQuickVideoItem::sourceSize()
{
    std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
    return m_impl->m_sourceSize;
}

void QWebRTCQuickVideoItem::asyncUpdate()
{
    update();
}

QWebRTCQuickVideoItem_p::QWebRTCQuickVideoItem_p(QWebRTCQuickVideoItem* q)
    : m_track(0), q_ptr(q)
{
}

void QWebRTCQuickVideoItem_p::OnFrame(const webrtc::VideoFrame& frame)
{
    std::lock_guard<std::recursive_mutex> locker(videoMutex);
    m_frame = std::make_shared<webrtc::VideoFrame>(frame);
    if (m_sourceSize.width() != frame.width() || m_sourceSize.height() != frame.height()) {
        m_sourceSize = QSize(frame.width(), frame.height());
        Q_EMIT q_ptr->sourceSizeChanged();
    }
    QMetaObject::invokeMethod(q_ptr, "asyncUpdate");
}
