#pragma once

#include <webrtc/modules/desktop_capture/desktop_capturer.h>
#include <QTimer>
#include <webrtc/api/video/i420_buffer.h>
#include <webrtc/api/video/video_frame.h>
#include <webrtc/api/mediastreaminterface.h>
#include <QObject>

class QWebRTCDesktopVideoSource: public QObject, public webrtc::VideoTrackSourceInterface,
        public webrtc::DesktopCapturer::Callback {
    Q_OBJECT
public:
    QWebRTCDesktopVideoSource();
    ~QWebRTCDesktopVideoSource();

    virtual void OnCaptureResult(webrtc::DesktopCapturer::Result result,
            std::unique_ptr<webrtc::DesktopFrame> frame) override;

    void Start();
    void Stop();

    virtual bool is_screencast() const override;

    virtual rtc::Optional<bool> needs_denoising() const override;

    virtual bool GetStats(Stats* stats) override;

    virtual SourceState state() const override;

    virtual bool remote() const override;

    virtual void AddOrUpdateSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink,
                                 const rtc::VideoSinkWants& wants) override;
    // RemoveSink must guarantee that at the time the method returns,
    // there is no current and no future calls to VideoSinkInterface::OnFrame.
    virtual void RemoveSink(rtc::VideoSinkInterface<webrtc::VideoFrame>* sink) override;

    virtual void RegisterObserver(webrtc::ObserverInterface* observer) override;
    virtual void UnregisterObserver(webrtc::ObserverInterface* observer) override;

public Q_SLOTS:
    void Capture();

private:
    void notifyObservers();
    std::unique_ptr<webrtc::DesktopCapturer> m_capturer;
    QTimer* timer;
    rtc::scoped_refptr<webrtc::I420Buffer> videoBuffer;
    int videBufferSize;
    int height, width;
    QList<QPair<rtc::VideoSinkInterface<webrtc::VideoFrame>*, rtc::VideoSinkWants> > m_videoSinks;
    QList<webrtc::ObserverInterface*> m_notifiers;
    webrtc::MediaSourceInterface::SourceState m_state;
};
