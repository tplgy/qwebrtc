#pragma once

#include "qwebrtcdatachannel.hpp"
#include "webrtc/api/datachannelinterface.h"

class QWebRTCDataChannel_impl : public QWebRTCDataChannel {
public:
    explicit QWebRTCDataChannel_impl(rtc::scoped_refptr<webrtc::DataChannelInterface> nativeDC);

    rtc::scoped_refptr<webrtc::DataChannelInterface> m_nativeDataChannel;
};
