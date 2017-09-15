#ifndef DAEMON_H
#define DAEMON_H

#include <libusb-1.0/libusb.h>

#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QList>
#include <QDataStream>
#include <QDebug>

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

    class USBTransferBlock {
    public:
        USBTransferBlock(USBHandleEventThread *parent, const QByteArray &bytes) :
            parent(parent),
            transfer(Q_NULLPTR)
        {
            init(bytes);
            alloc();
        }

        ~USBTransferBlock()
        {
            free();
        }

        static USBTransferBlock *create(USBHandleEventThread *parent, const QByteArray &bytes)
        {
            USBTransferBlock *block;
            if (transferBlockFreeList.isEmpty()) {
                block = new USBTransferBlock(parent, bytes);
                qDebug() << "new transfer block";
            } else {
                block = transferBlockFreeList.takeFirst();
                block->init(bytes);
            }
            return block;
        }

        static USBTransferBlock *create(USBHandleEventThread *parent, int length = DefaultBufferSize)
        {
            QByteArray bytes;
            bytes.resize(length);
            return create(parent, bytes);
        }

        static void destroyAll()
        {
            qDeleteAll(transferBlockFreeList);
        }

        void reclaim()
        {
            transferBlockFreeList.append(this);
        }

        void alloc()
        {
            transfer = libusb_alloc_transfer(0);
            if (transfer == Q_NULLPTR) {
            }
        }

        void free()
        {
            if (transfer != Q_NULLPTR) {
                libusb_free_transfer(transfer);
                transfer = Q_NULLPTR;
            }
        }

        void init(const QByteArray &bytes)
        {
            buffer = bytes;
            data = (unsigned char *)buffer.data();
            size = buffer.size();
        }

        void fillBulk(libusb_device_handle *dev_handle, unsigned char endpoint,
                      libusb_transfer_cb_fn callback, unsigned int timeout=0)
        {
            libusb_fill_bulk_transfer(transfer, dev_handle, endpoint, data, size, callback, this, timeout);
        }

        void submit()
        {
            libusb_submit_transfer(transfer);
        }

        void cancel()
        {
            libusb_cancel_transfer(transfer);
        }

        static QList<USBTransferBlock *> transferBlockFreeList;

        USBHandleEventThread *parent;
        QByteArray buffer;
        unsigned char *data;
        int size;
        libusb_transfer *transfer;
        static const int DefaultBufferSize = 16384;
    };

    QQueue<QByteArray> receiveQueue;

    USBTransferBlock *pingReceiveBlock;
    USBTransferBlock *pongReceiveBlock;

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
