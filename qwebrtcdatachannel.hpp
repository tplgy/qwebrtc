#pragma once

#include <memory>
#include <QByteArray>
#include <QObject>

struct QWebRTCDataChannelConfig {
    bool isOrdered = true;
    /**
     * The length of the time window (in milliseconds) during which transmissions
     * and retransmissions may occur in unreliable mode.
     */
    uint16_t maxPacketLifeTime = 0;
    /**
     * The maximum number of retransmissions that are attempted in unreliable mode.
     */
    uint16_t maxRetransmits;
    /**
     * Returns whether this data channel was negotiated by the application or not.
     */
    bool isNegotiated = false;
    /** The identifier for this data channel. */
    int channelId = 0;
    /**
     * The name of the sub-protocol used with this data channel, if any. Otherwise
     * this returns an empty string.
     */
    QString protocol;
};

class QWebRTCDataChannel : public QObject {
    Q_OBJECT
public:
    enum ChannelState {
        Connecting,
        Open,
        Closing,
        Closed
    };

    // Sends data as text
    virtual bool sendData(const QString&) = 0;
    // Treats the data as binary data
    virtual bool sendData(const QByteArray&) = 0;
    // closes the data channel
    virtual void close() = 0;

    virtual QString label() = 0;
    virtual bool isReliable() = 0;
    virtual QString protocol() = 0;
    virtual bool isNegotiated() = 0;
    virtual ChannelState channelState() = 0;
    virtual uint64_t bufferedAmount() = 0;

Q_SIGNALS:
    void dataReceived(QByteArray);
    void channelStateChanged();
};
