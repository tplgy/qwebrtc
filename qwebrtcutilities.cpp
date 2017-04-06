#include "qwebrtcutilities.hpp"
#include "qwebrtctypes_p.hpp"
#include "qwebrtcpeerconnection.hpp"
#include <QJsonObject>
#include <QDebug>

namespace QWebRTCUtilities {
std::shared_ptr<QWebRTCSessionDescription> sessionFromJSON(const QJsonObject& obj)
{
    if (obj.value(QLatin1String("sdp")).isString() && obj.value(QLatin1String("type")).isString()) {
        QString sdp = obj.value(QLatin1String("sdp")).toString();
        std::string sType = obj.value(QLatin1String("type")).toString().toStdString();
        QWebRTCSessionDescription::SDPType type;
        if (sType == webrtc::SessionDescriptionInterface::kOffer) {
            type = QWebRTCSessionDescription::SDPType::Offer;
        } else if (sType == webrtc::SessionDescriptionInterface::kAnswer) {
            type = QWebRTCSessionDescription::SDPType::Answer;
        } else {
            type = QWebRTCSessionDescription::SDPType::PrAnswer;
        }

        return QWebRTCPeerConnection::createSessionDescription(type, sdp.toUtf8());
    } else {
        return nullptr;
    }
}

std::shared_ptr<QWebRTCIceCandidate> iceCandidateFromJSON(const QJsonObject& obj)
{
    if (obj.value(QLatin1String("sdpMid")).isString() && obj.value(QLatin1String("candidate")).isString() && obj.value(QLatin1String("sdpMLineIndex")).isDouble()) {
        QString mid = obj.value(QLatin1String("sdpMid")).toString();
        QString sdp = obj.value(QLatin1String("candidate")).toString();
        int num = obj.value(QLatin1String("sdpMLineIndex")).toInt(-1);
        if (num == -1 || sdp.isEmpty() || mid.isEmpty()) {
            qWarning() << "Bad JSON for ICE candidate " << mid << " || " << sdp << " || " << num;
            return nullptr;
        }

        return QWebRTCPeerConnection::createIceCandidate(mid.toUtf8(), num, sdp.toUtf8());
    } else {
        qWarning() << "Bad JSON for ICE candidate " << obj.value(QLatin1String("sdpMid")).isString() << obj.value(QLatin1String("candidate")).isString() << obj.value(QLatin1String("sdpMLineIndex")).isString();
        return nullptr;
    }
}

QJsonObject iceCandidateToJSON(const std::shared_ptr<QWebRTCIceCandidate>& candidate)
{
    if (!candidate) {
        return QJsonObject();
    }
    return QJsonObject{
        {"sdpMid", QString::fromUtf8(candidate->sdpMediaId())},
        {"candidate", QString::fromUtf8(candidate->sdp())},
        {"sdpMLineIndex", candidate->sdpMLineIndex()}
    };
}

QJsonObject sessionToJSON(const std::shared_ptr<QWebRTCSessionDescription>& description)
{
    if (!description) {
        return QJsonObject();
    }
    return QJsonObject{
        {"sdp", QString::fromUtf8(description->sdp())},
        {"type", QString::fromStdString(QWebRTCSessionDescription_impl::SDPTypeToString(description->type()))}
    };
}
}
