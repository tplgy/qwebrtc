#pragma once

#include "qwebrtcdatachannel.hpp"
#include "webrtc/api/datachannelinterface.h"

class QWebRTCDataChannel_impl : public QWebRTCDataChannel {
public:
    virtual bool sendData(const QString&) override;
    virtual bool sendData(const QByteArray&) override;
    virtual void close() override;

    virtual QString label() override;
    virtual bool isReliable() override;
    virtual QString protocol() override;
    virtual bool isNegotiated() override;
    virtual ChannelState channelState() override;
    virtual uint64_t bufferedAmount() override;


    explicit QWebRTCDataChannel_impl(rtc::scoped_refptr<webrtc::DataChannelInterface> nativeDC);

    rtc::scoped_refptr<webrtc::DataChannelInterface> m_nativeDataChannel;
};
