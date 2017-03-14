#include "qwebrtcsessiondescription.hpp"
#include "qwebrtctypes_p.hpp"
#include <webrtc/api/jsep.h>
#include <QDebug>

QWebRTCSessionDescription::SDPType QWebRTCSessionDescription_impl::type() const
{
    return m_type;
}

QByteArray QWebRTCSessionDescription_impl::sdp() const
{
    return m_sdp;
}

bool QWebRTCSessionDescription_impl::isValid() const
{
    return !sdp().isEmpty();
}

QWebRTCSessionDescription_impl::QWebRTCSessionDescription_impl(QWebRTCSessionDescription::SDPType type, const QByteArray& sdp)
    : m_type(type), m_sdp(sdp), m_sessionDescription(0)
{
}

QWebRTCSessionDescription_impl::QWebRTCSessionDescription_impl(webrtc::SessionDescriptionInterface* sC)
    : m_sessionDescription(sC)
{
    parseNativeInterface(sC);
}

QWebRTCSessionDescription_impl::QWebRTCSessionDescription_impl(const webrtc::SessionDescriptionInterface* sC)
    : m_sessionDescription(0)
{
    qDebug() << "Setting const session description";
    parseNativeInterface(sC);
}

webrtc::SessionDescriptionInterface* QWebRTCSessionDescription_impl::getNativeDescription()
{
    if (m_sessionDescription) {
        auto sessionDescription = m_sessionDescription;
        m_sessionDescription = 0;
        return sessionDescription;
    } else {
        std::string typeString = SDPTypeToString(m_type);

        webrtc::SdpParseError sdpError;
        std::string sdp = m_sdp.toStdString();
        auto sessionDescription = webrtc::CreateSessionDescription(typeString,
                            sdp, &sdpError);
        if (!sessionDescription) {
            qWarning() << "Could not create session description " << QByteArray::fromStdString(sdpError.description) << " (line " << QByteArray::fromStdString(sdpError.line) << ")";
        }
        return sessionDescription;
    }
}

void QWebRTCSessionDescription_impl::parseNativeInterface(const webrtc::SessionDescriptionInterface* sC)
{
    if (sC->type() == webrtc::SessionDescriptionInterface::kAnswer) {
        m_type = QWebRTCSessionDescription::SDPType::Answer;
    } else if (sC->type() == webrtc::SessionDescriptionInterface::kPrAnswer) {
        m_type = QWebRTCSessionDescription::SDPType::PrAnswer;
    } else {
        m_type = QWebRTCSessionDescription::SDPType::Offer;
    }
    std::string sdp;
    sC->ToString(&sdp);
    m_sdp = QByteArray::fromStdString(sdp);
}

std::string QWebRTCSessionDescription::SDPTypeToString(QWebRTCSessionDescription::SDPType type)
{
    std::string typeString;
    switch (type) {
      case QWebRTCSessionDescription::SDPType::Offer:
        typeString = webrtc::SessionDescriptionInterface::kOffer; break;
      case QWebRTCSessionDescription::SDPType::PrAnswer:
        typeString = webrtc::SessionDescriptionInterface::kPrAnswer; break;
      case QWebRTCSessionDescription::SDPType::Answer:
        typeString = webrtc::SessionDescriptionInterface::kAnswer; break;
    }
    return typeString;
}
