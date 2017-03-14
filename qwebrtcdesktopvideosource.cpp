#include "qwebrtcdesktopvideosource_p.hpp"
#include <webrtc/modules/desktop_capture/desktop_capture_options.h>
#include <webrtc/common_video/libyuv/include/webrtc_libyuv.h>
#include <QDebug>

int I420DataSize(int height, int stride_y, int stride_u, int stride_v)
{
    return stride_y * height + (stride_u + stride_v) * ((height + 1) / 2);
}


QWebRTCDesktopVideoSource::QWebRTCDesktopVideoSource()
    : m_capturer(webrtc::DesktopCapturer::CreateScreenCapturer(webrtc::DesktopCaptureOptions::CreateDefault())),
      height(0), width(0), m_state(webrtc::MediaSourceInterface::SourceState::kInitializing)
{
    qDebug() << "Creating screen capture source";
    timer = new QTimer();
    timer->setSingleShot(true);
    timer->setInterval(50);
    QObject::connect(timer, &QTimer::timeout, this, &QWebRTCDesktopVideoSource::Capture);
    webrtc::DesktopCapturer::SourceList desktopScreens;
    if (!m_capturer) {
        qWarning() << "Could not create capturer";
    } else {
        m_capturer->GetSourceList(&desktopScreens);
        for (auto s : desktopScreens) {
            qDebug() << "screen: " << s.id << " -> " << QString::fromStdString(s.title);
        }
        m_capturer->SelectSource(desktopScreens[0].id);
        m_capturer->Start(this);
    }
}

QWebRTCDesktopVideoSource::~QWebRTCDesktopVideoSource()
{
    qDebug() << "Deleting screen capture source";
    timer->deleteLater();
}

void QWebRTCDesktopVideoSource::OnCaptureResult(webrtc::DesktopCapturer::Result result,
        std::unique_ptr<webrtc::DesktopFrame> frame)
{
    if (result == webrtc::DesktopCapturer::Result::ERROR_PERMANENT) {
        qWarning() << "Video capurer failed";
        return;
    } else {
        //qDebug() << "Video capture success";
        if (result == webrtc::DesktopCapturer::Result::SUCCESS) {
            if (!width) {
                qDebug() << "Creating video buffers";
                m_state = webrtc::MediaSourceInterface::SourceState::kLive;
                width = frame->size().width();
                height = frame->size().height();
                videoBuffer = webrtc::I420Buffer::Create(width, height);
                //notifyObservers();
            }

            webrtc::ConvertToI420(webrtc::VideoType::kARGB, frame->data(),
                    0, 0, frame->size().width(), frame->size().height(),
                    0, webrtc::VideoRotation::kVideoRotation_0, videoBuffer);
            auto videoFrame = webrtc::VideoFrame(videoBuffer, webrtc::VideoRotation::kVideoRotation_0, rtc::TimeMicros());

            for (auto it=m_videoSinks.begin(); it!=m_videoSinks.end(); ++it) {
                it->first->OnFrame(videoFrame);
            }
        }
        timer->start();
    }
}

void QWebRTCDesktopVideoSource::Capture()
{
    //qDebug() << "Capture";
    if (m_capturer) {
        m_capturer->CaptureFrame();
    }
}

void QWebRTCDesktopVideoSource::Start()
{
    qDebug() << "Start QWebRTCDesktopVideoSource";
    QMetaObject::invokeMethod(this, "Capture", Qt::QueuedConnection);
}

void QWebRTCDesktopVideoSource::Stop()
{
    qDebug() << "Stop QWebRTCDesktopVideoSource";
    timer->stop();
    height = 0;
    width = 0;
    videoBuffer.release();
    m_state = webrtc::MediaSourceInterface::SourceState::kEnded;
    //notifyObservers();
}

bool QWebRTCDesktopVideoSource::is_screencast() const
{
    return true;
}

rtc::Optional<bool> QWebRTCDesktopVideoSource::needs_denoising() const
{
    return rtc::Optional<bool>(false);
}

bool QWebRTCDesktopVideoSource::GetStats(webrtc::VideoTrackSourceInterface::Stats* stats)
{
    if (!stats || !width) {
        return false;
    }
    stats->input_height = height;
    stats->input_width = width;
    return true;
}

webrtc::MediaSourceInterface::SourceState QWebRTCDesktopVideoSource::state() const
{
    return m_state;
}

bool QWebRTCDesktopVideoSource::remote() const
{
    return false;
}

void QWebRTCDesktopVideoSource::AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
        const rtc::VideoSinkWants& wants)
{
    //qDebug() << "QWebRTCDesktopVideoSource AddOrUpdateSink";
    for (auto it=m_videoSinks.begin(); it!=m_videoSinks.end(); ++it) {
        if (it->first == sink) {
            it->second = wants;
            return;
        }
    }
    m_videoSinks.append(QPair<rtc::VideoSinkInterface<webrtc::VideoFrame>*, rtc::VideoSinkWants>(sink, wants));
}

void QWebRTCDesktopVideoSource::RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink)
{
    //qDebug() << "QWebRTCDesktopVideoSource RemoveSink";
    for (auto it=m_videoSinks.begin(); it!=m_videoSinks.end(); ++it) {
        if (it->first == sink) {
            m_videoSinks.erase(it);
            return;
        }
    }
}

void QWebRTCDesktopVideoSource::RegisterObserver(webrtc::ObserverInterface* observer)
{
    //qDebug() << "QWebRTCDesktopVideoSource RegisterObserver";
    m_notifiers.append(observer);
}

void QWebRTCDesktopVideoSource::UnregisterObserver(webrtc::ObserverInterface* observer)
{
    //qDebug() << "QWebRTCDesktopVideoSource UnregisterObserver";
    m_notifiers.removeOne(observer);
}

void QWebRTCDesktopVideoSource::notifyObservers()
{
    for (auto observer : m_notifiers) {
        observer->OnChanged();
    }
}
