#pragma once

#include "qwebrtcmediastream.hpp"
#include <webrtc/api/mediastreaminterface.h>
#include <QList>

class QWebRTCMediaStream_impl : public QWebRTCMediaStream {
public:
    explicit QWebRTCMediaStream_impl(const rtc::scoped_refptr<webrtc::MediaStreamInterface>& stream);

    virtual void addTrack(const std::shared_ptr<QWebRTCMediaTrack>&) override;
    virtual void removeTrack(const std::shared_ptr<QWebRTCMediaTrack>&) override;
    virtual QList<std::shared_ptr<QWebRTCMediaTrack>> tracks() override;

    QList<std::shared_ptr<QWebRTCMediaTrack>> m_tracks;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> m_nativeStream;
};
