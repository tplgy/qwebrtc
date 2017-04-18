#pragma once

#include <QSharedPointer>
#include <QString>

class QWebRTCMediaTrack;
class QWebRTCMediaStream {
public:
    virtual void addTrack(const QSharedPointer<QWebRTCMediaTrack>&) = 0;
    virtual void removeTrack(const QSharedPointer<QWebRTCMediaTrack>&) = 0;
    virtual QList<QSharedPointer<QWebRTCMediaTrack>> tracks() = 0;
    virtual QString label() = 0;
};
