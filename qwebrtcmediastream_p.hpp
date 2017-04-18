#pragma once

#include "qwebrtcmediastream.hpp"
#include <webrtc/api/mediastreaminterface.h>
#include <QList>
#include <QSharedPointer>

class QWebRTCMediaStream_impl : public QWebRTCMediaStream {
public:
    explicit QWebRTCMediaStream_impl(const rtc::scoped_refptr<webrtc::MediaStreamInterface>& stream);

    virtual void addTrack(const QSharedPointer<QWebRTCMediaTrack>&) override;
    virtual void removeTrack(const QSharedPointer<QWebRTCMediaTrack>&) override;
    virtual QList<QSharedPointer<QWebRTCMediaTrack>> tracks() override;

    virtual QString label() override;

    QList<QSharedPointer<QWebRTCMediaTrack>> m_tracks;
    rtc::scoped_refptr<webrtc::MediaStreamInterface> m_nativeStream;
};
