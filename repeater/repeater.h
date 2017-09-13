#ifndef REPEATER_H
#define REPEATER_H

#include <QDataStream>

#include "bigeye.h"

class QTimer;
class QextSerialPort;

class Repeater : public Bigeye
{
    Q_OBJECT
public:
    explicit Repeater(const QString &portName, bool isStudio = false, QObject *parent = Q_NULLPTR);

signals:
    void dataArrived(const char *data, int length);

public slots:
    void onDataArrived(const char *data, int length);

private slots:
    void onPollTimeout();
    void onReadyRead();
    void startDaemon(QDataStream &stream);
    void stopDaemon(QDataStream &stream);
    void keyEvent(QDataStream &stream);
    void extendedData(QDataStream &stream);
#if 0
    void snapshot(QDataStream &stream);
    void videoFrame(QDataStream &stream);
    void executeProgram(QDataStream &stream);
    void executeProgramDetached(QDataStream &stream);
    void fileTransferPut(QDataStream &stream);
    void fileTransferGet(QDataStream &stream);
#endif

private:
    void sendExtendedData(const QString &category, QByteArray &buffer);

private:
    bool isStudio;
    QextSerialPort *port;
    QTimer *pollTimer;
    QByteArray buffer;
    static const int BufferSize = 128 * 1024;
};

#endif // REPEATER_H
