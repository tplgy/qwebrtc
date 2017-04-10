#pragma once

#include <QObject>
#include <QString>
#include <memory>

class QWebRTCPeerConnection;
class QWebRTCPeerConnectionFactory_impl;
class QWebRTCMediaTrack;
class QWebRTCMediaStream;
class QWebRTCConfiguration;

class QWebRTCPeerConnectionFactory : public QObject {
public:
    QWebRTCPeerConnectionFactory();

    std::shared_ptr<QWebRTCMediaTrack> createAudioTrack(const QVariantMap& constraints, const QString& trackId = QString());

    std::shared_ptr<QWebRTCMediaTrack> createVideoTrack(const QVariantMap& constraints, const QString& trackId = QString());

    std::shared_ptr<QWebRTCMediaTrack> createScreenCaptureTrack(const QString& trackId = QString());

    std::shared_ptr<QWebRTCMediaStream> createMediaStream(const QString& label);

    std::shared_ptr<QWebRTCPeerConnection> createPeerConnection(const QWebRTCConfiguration&);

private:
    std::shared_ptr<QWebRTCPeerConnectionFactory_impl> m_impl;
};
