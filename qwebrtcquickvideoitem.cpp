#include "qwebrtcquickvideoitem_p.hpp"
#include <QDebug>
#include <assert.h>
#include <QSGSimpleTextureNode>
#include <QSGTexture>
#include <QOpenGLTexture>
#include <QOpenGLContext>
#include <webrtc/common_video/libyuv/include/webrtc_libyuv.h>
#include <QMutex>
#include <QElapsedTimer>

QWebRTCQuickVideoItem::QWebRTCQuickVideoItem(QQuickItem *parent)
    : QQuickItem(parent), m_impl(new QWebRTCQuickVideoItem_p(this))
{
    setFlags(QQuickItem::ItemHasContents);
    setAntialiasing(true);
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
        qDebug() << "Video track changed to " << (long long)trackImpl;
        Q_EMIT videoTrackChanged();
        return;
    }
    assert(trackImpl->trackType() == QWebRTCMediaTrack::Type::Video);
    m_impl->m_track = trackImpl;
    m_impl->m_track->videoTrack->AddOrUpdateSink(m_impl, rtc::VideoSinkWants());
    Q_EMIT videoTrackChanged();
}

class VideoFrameTexture : public QSGTexture
{
public:
    VideoFrameTexture();
    ~VideoFrameTexture();

    void newVideoFrame(std::shared_ptr<webrtc::VideoFrame> frame);

    virtual void bind() override;

    virtual int textureId() const override;
    virtual QSize textureSize() const override;
    virtual bool hasAlphaChannel() const override;
    virtual bool hasMipmaps() const override;

private:
    std::vector<uint8_t> m_buffer;
    QSize m_size;
    GLuint m_texture;
};

VideoFrameTexture::VideoFrameTexture()
    : m_texture(0)
{
    glGenTextures(1, &m_texture);
}

VideoFrameTexture::~VideoFrameTexture()
{
    glDeleteTextures(1, &m_texture);
}

void VideoFrameTexture::newVideoFrame(std::shared_ptr<webrtc::VideoFrame> frame)
{
    m_size = QSize(frame->width(), frame->height());
    size_t bufferSize = m_size.width() * m_size.height() * 4;
    if (m_buffer.size() != bufferSize) {
        m_buffer.resize(bufferSize);
    }
    webrtc::ConvertFromI420(*frame, webrtc::kBGRA, 0, m_buffer.data());
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.width(), m_size.height(), 0, GL_BGRA,
                 GL_UNSIGNED_INT_8_8_8_8, static_cast<GLvoid*>(m_buffer.data()));
}

void VideoFrameTexture::bind()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

int VideoFrameTexture::textureId() const
{
    return m_texture;
}

QSize VideoFrameTexture::textureSize() const
{
    return m_size;
}

bool VideoFrameTexture::hasAlphaChannel() const
{
    return true;
}

bool VideoFrameTexture::hasMipmaps() const
{
    return false;
}

QSGNode *QWebRTCQuickVideoItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *update)
{
    if (!m_impl->m_frame) {
        return nullptr;
    }
    QSGTransformNode *rotationNode = static_cast<QSGTransformNode*>(node);
    QSGSimpleTextureNode *textureNode = nullptr;
    if (rotationNode) {
        textureNode = static_cast<QSGSimpleTextureNode*>(rotationNode->childAtIndex(0));
    }
    if (!textureNode) {
        qDebug() << "Constructing new texture node";
        textureNode = new QSGSimpleTextureNode();
        rotationNode = new QSGTransformNode();
        rotationNode->appendChildNode(textureNode);

        auto texture = new VideoFrameTexture();
        textureNode->setTexture(texture);
    }
    auto frame = m_impl->m_frame;

    auto videoFrameTexture = static_cast<VideoFrameTexture*>(textureNode->texture());
    videoFrameTexture->newVideoFrame(m_impl->m_frame);
    rotationNode->markDirty(QSGNode::DirtyMatrix);
    textureNode->markDirty(QSGNode::DirtyMaterial);

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

    // Apply rotation
    QMatrix4x4 mat;
    mat.translate(boundingRect().width()/2, boundingRect().height()/2);
    mat.rotate(frame->rotation(), 0, 0, 1);
    mat.translate(-boundsWidth/2, -boundsHeight/2);
    rotationNode->setMatrix(mat);

    textureNode->setRect(QRect((boundsWidth-width)/2, (boundsHeight-height)/2, width, height));
    return rotationNode;
}

/*void QWebRTCQuickVideoItem::paint(QPainter *painter)
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
}*/

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
