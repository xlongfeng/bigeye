#ifndef DAEMON_H
#define DAEMON_H

#include <libusb-1.0/libusb.h>

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QDataStream>

#include "bigeye.h"

class USBHandleEventThread: public QThread
{
    Q_OBJECT

public:
    explicit USBHandleEventThread(QObject *parent = Q_NULLPTR);
    ~USBHandleEventThread();

#if QT_VERSION < QT_VERSION_CHECK(5, 2, 0)
    bool isInterruptionRequested() const
    {
        return interrupt;
    }
    void requestInterruption()
    {
        interrupt = true;
    }
#endif

    void stop();

    void enqueueReceiveBytes(const QByteArray &bytes);
    QByteArray dequeueReceiveBytes();

    void tramsmitBytes(const QByteArray &bytes);

signals:
    void dataArrived(const QByteArray &bytes);

protected:
    virtual void run();

private:
    void startReceive();
    void stopReceive();
    static void transmitTransferCallback(libusb_transfer *transfer);
    static void receiveTransferCallback(libusb_transfer *transfer);
    static int hotplugCallback(libusb_context *ctx, libusb_device *dev,
                                libusb_hotplug_event event, void *user_data);
    static void dumpUsbInformation(libusb_device *dev);

private:
#if QT_VERSION < QT_VERSION_CHECK(5, 2, 0)
    bool interrupt;
#endif

    struct {
        QByteArray buffer;
        libusb_transfer *transfer;
    } USBTransferBlock;

    QQueue<QByteArray> receiveQueue;

    QByteArray pingRreceiveBuffer;
    QByteArray pongRreceiveBuffer;
    libusb_transfer *pingReceiveTransfer;
    libusb_transfer *pongReceiveTransfer;

    libusb_context *ctx;
    libusb_device_handle *device;
    libusb_hotplug_callback_handle hotplug;
};

class Daemon : public Bigeye
{
    Q_OBJECT

public:
    explicit Daemon(QObject *parent = Q_NULLPTR);

protected:
    virtual void defaultDispose(const QString &command, QDataStream &stream);

private slots:
    void onDataArrived(const QByteArray &bytes);
    void startDaemon(QDataStream &stream);
    void stopDaemon(QDataStream &stream);
    void snapshot(QDataStream &stream);
    void videoFrame(QDataStream &stream);
    void executeProgram(QDataStream &stream);
    void executeProgramDetached(QDataStream &stream);
    void fileTransferPut(QDataStream &stream);
    void fileTransferGet(QDataStream &stream);

private:
    void sendExtendedData(const QString &category, QByteArray &buffer);

private:
    QMutex mutex;
    USBHandleEventThread *thread;
};

#endif // DAEMON_H
