#pragma once

#include "qwebrtcsessiondescription.hpp"
#include "qwebrtcicecandidate.hpp"
#include <webrtc/api/jsep.h>

class QWebRTCSessionDescription_impl : public QWebRTCSessionDescription {
public:
    QWebRTCSessionDescription_impl(QWebRTCSessionDescription::SDPType type, const QByteArray& sdp);
    explicit QWebRTCSessionDescription_impl(webrtc::SessionDescriptionInterface*);
    explicit QWebRTCSessionDescription_impl(const webrtc::SessionDescriptionInterface*);

    //transfers ownership
    webrtc::SessionDescriptionInterface* getNativeDescription();

    void parseNativeInterface(const webrtc::SessionDescriptionInterface*);

    virtual SDPType type() const override;
    virtual QByteArray sdp() const override;
    virtual bool isValid() const override;

    QWebRTCSessionDescription::SDPType m_type;
    QByteArray m_sdp;
private:
    webrtc::SessionDescriptionInterface* m_sessionDescription;
};

namespace webrtc {
class JsepIceCandidate;
}

class QWebRTCIceCandidate_impl : public QWebRTCIceCandidate {
public:
    QWebRTCIceCandidate_impl(QByteArray sdpMid, int sdpIndex, const QByteArray& sdp);
    explicit QWebRTCIceCandidate_impl(const webrtc::IceCandidateInterface*);

    virtual QByteArray sdp() const override;

    virtual QByteArray sdpMediaId() const override;

    virtual int sdpMLineIndex() const override;

    virtual bool isValid() const override;

    virtual QString type() const override;

    webrtc::IceCandidateInterface* iceCandidate();
};
