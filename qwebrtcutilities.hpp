#pragma once

#include <QVariantMap>
#include <QSharedPointer>
#include "qwebrtcsessiondescription.hpp"
#include "qwebrtcicecandidate.hpp"

namespace QWebRTCUtilities {
QSharedPointer<QWebRTCSessionDescription> sessionFromJSON(const QJsonObject&);
QSharedPointer<QWebRTCIceCandidate> iceCandidateFromJSON(const QJsonObject&);
QJsonObject iceCandidateToJSON(const QSharedPointer<QWebRTCIceCandidate>&);
QJsonObject sessionToJSON(const QSharedPointer<QWebRTCSessionDescription>&);
}
