#ifndef REPEATER_H
#define REPEATER_H

#include <QDataStream>

#include "bigeye.h"

class QextSerialPort;

class Repeater : public Bigeye
{
    Q_OBJECT
public:
    explicit Repeater(const QString &portName, QObject *parent = Q_NULLPTR);

signals:
    void dataArrived(const QByteArray &bytes);

public slots:
    void onDataArrived(const QByteArray &bytes);

private slots:
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
    QextSerialPort *port;
};

#endif // REPEATER_H
