#include "qwebrtcicecandidate.hpp"
#include "qwebrtctypes_p.hpp"
#include <QDebug>
#include <assert.h>

QByteArray QWebRTCIceCandidate_impl::sdp() const
{
    return m_sdp;
}

QByteArray QWebRTCIceCandidate_impl::sdpMediaId() const
{
    return m_sdpMid;
}

int QWebRTCIceCandidate_impl::sdpMLineIndex() const
{
    return m_sdpMlineIndex;
}

bool QWebRTCIceCandidate_impl::isValid() const
{
    return m_iceCandidate != nullptr;
}

QWebRTCIceCandidate_impl::QWebRTCIceCandidate_impl(QByteArray sdpMid, int sdpIndex, const QByteArray& sdp)
    : m_sdpMid(sdpMid), m_sdp(sdp), m_sdpMlineIndex(sdpIndex)
{
    webrtc::SdpParseError error;
    m_iceCandidate = std::shared_ptr<webrtc::IceCandidateInterface>(webrtc::CreateIceCandidate(sdpMid.toStdString(), sdpIndex,
                        sdp.toStdString(), &error));
    if (!m_iceCandidate) {
        qWarning() << "Ice candidate parsing error: " << QByteArray::fromStdString(error.line) << " " << QByteArray::fromStdString(error.description);
        assert(true);
    }
}

QWebRTCIceCandidate_impl::QWebRTCIceCandidate_impl(const webrtc::IceCandidateInterface* candidate)
{
    if (candidate) {
        std::string sdpData;
        if (candidate->ToString(&sdpData)) {
            m_sdp = QByteArray::fromStdString(sdpData);
            m_sdpMid = QByteArray::fromStdString(candidate->sdp_mid());
            m_sdpMlineIndex = candidate->sdp_mline_index();
        }
    }
}
