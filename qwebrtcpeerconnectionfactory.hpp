#pragma once

#include <QObject>
#include <QString>
#include <QSharedPointer>

class QWebRTCPeerConnection;
class QWebRTCPeerConnectionFactory_impl;
class QWebRTCMediaTrack;
class QWebRTCMediaStream;
class QWebRTCConfiguration;

class QWebRTCPeerConnectionFactory : public QObject {
public:
    QWebRTCPeerConnectionFactory();

    QSharedPointer<QWebRTCMediaTrack> createAudioTrack(const QVariantMap& constraints, const QString& trackId = QString());

    QSharedPointer<QWebRTCMediaTrack> createVideoTrack(const QVariantMap& constraints, const QString& trackId = QString());

    QSharedPointer<QWebRTCMediaTrack> createScreenCaptureTrack(const QString& trackId = QString());

    QSharedPointer<QWebRTCMediaStream> createMediaStream(const QString& label);

    QSharedPointer<QWebRTCPeerConnection> createPeerConnection(const QWebRTCConfiguration&);

private:
    // This pointer is shared among all peer connections to ensure that all resources allocated by the
    // factory are not deallocated (e.g. the webrtc threads)
    QSharedPointer<QWebRTCPeerConnectionFactory_impl> m_impl;
};
