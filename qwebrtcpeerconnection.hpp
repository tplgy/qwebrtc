#pragma once

#include <QObject>
#include <QSharedPointer>
#include "qwebrtcsessiondescription.hpp"

class QWebRTCPeerConnection_impl;

class QWebRTCDataChannel;
struct QWebRTCDataChannelConfig;
class QWebRTCIceCandidate;
class QWebRTCSessionDescription;

class QWebRTCMediaStream;

class QWebRTCPeerConnection : public QObject {
    Q_OBJECT
    friend class QWebRTCPeerConnectionFactory;
public:
    enum class SignalingState {
        Stable,
        LocalOffer,
        LocalOfferPrAnswer,
        RemoteOffer,
        RemotePrAnswer,
        Closed
    };

    enum class IceConnectionState {
        New,
        Checking,
        Connected,
        Completed,
        Failed,
        Disconnected,
        Closed,
        Count
    };

    enum class IceGatheringState {
        New,
        Gathering,
        Complete
    };

    void close();

    void createOfferForConstraints(const QVariantMap& constraints,
            std::function<void(const QSharedPointer<QWebRTCSessionDescription>&)>);
    void createAnswerForConstraints(const QVariantMap& constraints,
            std::function<void(const QSharedPointer<QWebRTCSessionDescription>&)>);

    void addStream(const QSharedPointer<QWebRTCMediaStream>&);
    void removeStream(const QSharedPointer<QWebRTCMediaStream>&);

    void setConfiguration();

    void setLocalDescription(const QSharedPointer<QWebRTCSessionDescription>&, std::function<void(bool)>);
    void setRemoteDescription(const QSharedPointer<QWebRTCSessionDescription>&, std::function<void(bool)>);

    void addIceCandidate(const QSharedPointer<QWebRTCIceCandidate>&);
    void removeIceCandidate(const QSharedPointer<QWebRTCIceCandidate>&);

    QSharedPointer<QWebRTCDataChannel> dataChannelForLabel(const QString& label, const QWebRTCDataChannelConfig& config);

    static QSharedPointer<QWebRTCSessionDescription> createSessionDescription(QWebRTCSessionDescription::SDPType, const QByteArray&);
    static QSharedPointer<QWebRTCIceCandidate> createIceCandidate(QByteArray mId, int sdpMLineIndex, const QByteArray& sdpData);

    QSharedPointer<QWebRTCSessionDescription> currentLocalDescription();
    QSharedPointer<QWebRTCSessionDescription> currentRemoteDescription();

    SignalingState signalingState();
    IceConnectionState iceConnectionState();
    IceGatheringState iceGatheringState();

    ~QWebRTCPeerConnection();
Q_SIGNALS:
    void signalingChange();
    void iceConnectionStateChanged();
    void iceGatheringChanged();
    void streamAdded(const QSharedPointer<QWebRTCMediaStream>&);
    void streamRemoved(const QSharedPointer<QWebRTCMediaStream>&);
    void newIceCandidate(const QSharedPointer<QWebRTCIceCandidate>&);
    void iceCandidateRemoved(QSharedPointer<QWebRTCIceCandidate>);
    void renegotiationNeeded();
    void dataChannelReceived(const QSharedPointer<QWebRTCDataChannel>&);

private:
    QWebRTCPeerConnection();
    QWebRTCPeerConnection_impl* m_impl;
};
