#pragma once

#include <QList>

struct QWebRTCIceServer {
    QByteArray uri;
    QList<QByteArray> urls;
    QByteArray username;
    QByteArray password;
    bool tlsCertNoCheck = false;
};

class QWebRTCConfiguration {
public:
    QList<QWebRTCIceServer> iceServers;
};
