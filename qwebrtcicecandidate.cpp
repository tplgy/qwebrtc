#include "qwebrtcicecandidate.hpp"
#include "qwebrtctypes_p.hpp"
#include <QDebug>
#include <assert.h>
#include <webrtc/api/jsepicecandidate.h>

QByteArray QWebRTCIceCandidate_impl::sdp() const
{
    if (isValid()) {
        std::string sdpData;
        if (m_iceCandidate->ToString(&sdpData)) {
            return QByteArray::fromStdString(sdpData);
        }
    }
    return QByteArray();
}

QByteArray QWebRTCIceCandidate_impl::sdpMediaId() const
{
    if (!isValid()) {
        return QByteArray();
    }
    return QByteArray::fromStdString(m_iceCandidate->sdp_mid());
}

int QWebRTCIceCandidate_impl::sdpMLineIndex() const
{
    if (!isValid()) {
        return 0;
    }
    return m_iceCandidate->sdp_mline_index();
}

bool QWebRTCIceCandidate_impl::isValid() const
{
    return m_iceCandidate != nullptr;
}

QString QWebRTCIceCandidate_impl::type() const
{
    if (!isValid()) {
        return QString();
    }
    return QString::fromStdString(m_iceCandidate->candidate().type());
}

webrtc::IceCandidateInterface* QWebRTCIceCandidate_impl::iceCandidate()
{
    return m_iceCandidate.get();
}

QWebRTCIceCandidate_impl::QWebRTCIceCandidate_impl(QByteArray sdpMid, int sdpIndex, const QByteArray& sdp)
{
    webrtc::SdpParseError error;
    m_iceCandidate = std::unique_ptr<webrtc::IceCandidateInterface>(webrtc::CreateIceCandidate(sdpMid.toStdString(), sdpIndex,
                sdp.toStdString(), &error));
    if (!m_iceCandidate) {
        qWarning() << "Ice candidate parsing error: " << QByteArray::fromStdString(error.line) << " " << QByteArray::fromStdString(error.description);
        assert(true);
    }
}

QWebRTCIceCandidate_impl::QWebRTCIceCandidate_impl(const webrtc::IceCandidateInterface* candidate)
{
    if (candidate) {
        m_iceCandidate = std::unique_ptr<webrtc::IceCandidateInterface>(new webrtc::JsepIceCandidate(candidate->sdp_mid(), candidate->sdp_mline_index(), candidate->candidate()));
    }
}
