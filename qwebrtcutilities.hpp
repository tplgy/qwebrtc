#pragma once

#include <QVariantMap>
#include "qwebrtcsessiondescription.hpp"
#include "qwebrtcicecandidate.hpp"

namespace QWebRTCUtilities {
std::shared_ptr<QWebRTCSessionDescription> sessionFromJSON(const QJsonObject&);
std::shared_ptr<QWebRTCIceCandidate> iceCandidateFromJSON(const QJsonObject&);
QJsonObject iceCandidateToJSON(const std::shared_ptr<QWebRTCIceCandidate>&);
QJsonObject sessionToJSON(const std::shared_ptr<QWebRTCSessionDescription>&);
}
