#pragma once

#include <memory>
#include <QString>

class QWebRTCSessionDescription_impl;

class QWebRTCSessionDescription {
public:
    enum class SDPType {
        Offer,
        PrAnswer,
        Answer
    };

    virtual SDPType type() const = 0;
    virtual QByteArray sdp() const = 0;
    virtual bool isValid() const = 0;

    static std::string SDPTypeToString(QWebRTCSessionDescription::SDPType);
};
