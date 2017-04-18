#include "qwebrtcsessiondescription.hpp"
#include <webrtc/api/jsep.h>
#include <QSharedPointer>

class QWebRTCSessionDescription_impl {
public:
    QWebRTCSessionDescription_impl(QWebRTCSessionDescription::SDPType type, const QByteArray& sdp)
        : m_type(type), m_sdp(sdp)
    {
        m_sessionDescription = QSharedPointer<webrtc::SessionDescriptionInterface>(
                    webrtc::CreateSessionDescription(type == QWebRTCSessionDescription::SDPType::Answer ? "answer" : "offer",
                            sdp.toStdString(), nullptr));
    }

    QWebRTCSessionDescription::SDPType m_type;
    QByteArray m_sdp;
    QSharedDataPointer<webrtc::SessionDescriptionInterface> m_sessionDescription;
};
