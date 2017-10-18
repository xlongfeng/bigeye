/*
 * Bigeye - Accessorial Tool for Daily Test
 *
 * Copyright (c) 2017, longfeng.xiao <xlongfeng@126.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <QMetaObject>
#include <QProcess>
#include <QFile>
#include <QDebug>

#include "daemon.h"

QList<USBHandleEventThread::USBTransferBlock *> USBHandleEventThread::USBTransferBlock::transferBlockFreeList;

USBHandleEventThread::USBHandleEventThread(QObject *parent) : QThread(parent)
#if QT_VERSION < QT_VERSION_CHECK(5, 2, 0)
    , interrupt(false)
#endif
{
    pingReceiveBlock = USBTransferBlock::create(this);
    pongReceiveBlock = USBTransferBlock::create(this);

    int rc = libusb_init(&ctx);
    if (LIBUSB_SUCCESS != rc) {
        qDebug() << "libusb_init" << libusb_error_name(rc);
    }

    rc = libusb_hotplug_register_callback(
                ctx, (libusb_hotplug_event)(LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED |
                LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT),
                (libusb_hotplug_flag)LIBUSB_HOTPLUG_ENUMERATE,
                0x2009, 0x0805, LIBUSB_HOTPLUG_MATCH_ANY,
                hotplugCallback, this, &hotplug);
    if (LIBUSB_SUCCESS != rc) {
        qDebug() << "libusb_init" << libusb_error_name(rc);
    }
}

USBHandleEventThread::~USBHandleEventThread()
{
    libusb_exit(ctx);

    USBTransferBlock::destroyAll();
}

void USBHandleEventThread::stop()
{
    libusb_hotplug_deregister_callback(ctx, hotplug);
}

void USBHandleEventThread::enqueueReceiveBytes(const QByteArray &bytes)
{
    receiveQueue.enqueue(bytes);
}

QByteArray USBHandleEventThread::dequeueReceiveBytes()
{
    if (receiveQueue.isEmpty())
        return QByteArray();

    return receiveQueue.dequeue();
}

void USBHandleEventThread::tramsmitBytes(const QByteArray &bytes)
{
    // qDebug() << "tramsmitBytes" << bytes.size();
    USBTransferBlock *transmitBlock = USBTransferBlock::create(this, bytes);
    transmitBlock->fillBulk(device, 1, transmitTransferCallback);
    transmitBlock->submit();
}

void USBHandleEventThread::run()
{
    qDebug() << "USBHandleEventThread" << "start";

    forever {
        if (isInterruptionRequested())
            break;
        // qDebug() << "USBHandleEventThread" << "handle";
        libusb_handle_events_completed(Q_NULLPTR, Q_NULLPTR);
    }

    qDebug() << "USBHandleEventThread" << "end";
}

void USBHandleEventThread::startReceive()
{
    if (libusb_kernel_driver_active(device, 0) == 1)
        libusb_detach_kernel_driver(device, 0);
    libusb_claim_interface(device, 0);

    pingReceiveBlock->fillBulk(device, 129, receiveTransferCallback);
    pingReceiveBlock->submit();

    pongReceiveBlock->fillBulk(device, 129, receiveTransferCallback);
    pongReceiveBlock->submit();
}

void USBHandleEventThread::stopReceive()
{
    pongReceiveBlock->cancel();
    pingReceiveBlock->cancel();
}

void USBHandleEventThread::transmitTransferCallback(libusb_transfer *transfer)
{
    USBTransferBlock *usbTransferBlock = (USBTransferBlock *)transfer->user_data;

    switch (transfer->status) {
    case LIBUSB_TRANSFER_COMPLETED:
        usbTransferBlock->reclaim();
        break;
    case LIBUSB_TRANSFER_STALL:
    case LIBUSB_TRANSFER_ERROR:
    case LIBUSB_TRANSFER_TIMED_OUT:
    case LIBUSB_TRANSFER_OVERFLOW:
        // break;
    case LIBUSB_TRANSFER_CANCELLED:
        // break;
    case LIBUSB_TRANSFER_NO_DEVICE:
        // break;
    default:
        qDebug() << "Transmit transfer callback" << transfer->status;
        break;
    }
}

void USBHandleEventThread::receiveTransferCallback(libusb_transfer *transfer)
{
    USBTransferBlock *usbTransferBlock = (USBTransferBlock *)transfer->user_data;
    USBHandleEventThread *self = (USBHandleEventThread *)usbTransferBlock->parent;
    const char *buffer = (const char *)transfer->buffer;

    switch (transfer->status) {
    case LIBUSB_TRANSFER_COMPLETED:
        self->dataArrived(QByteArray(buffer, transfer->actual_length));
        usbTransferBlock->submit();
        break;
    case LIBUSB_TRANSFER_STALL:
    case LIBUSB_TRANSFER_ERROR:
    case LIBUSB_TRANSFER_TIMED_OUT:
    case LIBUSB_TRANSFER_OVERFLOW:
        // break;
    case LIBUSB_TRANSFER_CANCELLED:
        // break;
    case LIBUSB_TRANSFER_NO_DEVICE:
        // break;
    default:
        qDebug() << "Receive transfer callback" << transfer->status;
        break;
    }
}

int USBHandleEventThread::hotplugCallback(libusb_context *ctx, libusb_device *dev,
                            libusb_hotplug_event event, void *user_data)
{
    Q_UNUSED(ctx)
    USBHandleEventThread *self = (USBHandleEventThread *)user_data;
    int rc;

    if (LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED == event) {
        rc = libusb_open(dev, &self->device);
        if (LIBUSB_SUCCESS != rc) {
            qDebug() << "libusb_open" << libusb_error_name(rc);
        } else {
            dumpUsbInformation(dev);
            self->startReceive();
        }
    } else if (LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT == event) {
        if (self->device) {
            self->stopReceive();
            libusb_close(self->device);
            self->device = Q_NULLPTR;
        }
        qDebug() << "libusb_close";
    } else {
        qDebug() << "Unhandled event" << event;
    }

    return 0;
}

void USBHandleEventThread::dumpUsbInformation(libusb_device *dev)
{
    qDebug() << "Bigeye usb device attached";
    qDebug() << "Bus Number" << libusb_get_bus_number(dev);
    qDebug() << "Port Number" << libusb_get_port_number(dev);
    qDebug() << "Device Address" << libusb_get_device_address(dev);
    qDebug() << "Device Speed" << libusb_get_device_speed(dev);

    int rc;

    libusb_device_descriptor deviceDescriptor;
    rc = libusb_get_device_descriptor(dev, &deviceDescriptor);
    qDebug() << "Num of configurations" << deviceDescriptor.bNumConfigurations;

    libusb_config_descriptor *configDescriptor;
    rc = libusb_get_active_config_descriptor(dev, &configDescriptor);
    if (LIBUSB_SUCCESS != rc)
        return;

    qDebug() << "Active configuration value" << configDescriptor->bConfigurationValue;
    qDebug() << "Num of interfaces" << configDescriptor->bNumInterfaces;
    for (int i = 0; i < configDescriptor->bNumInterfaces; i++) {
        qDebug() << "Interface at" << i;
        const libusb_interface &interface = configDescriptor->interface[i];
        qDebug() << "  Num alsetting" << interface.num_altsetting;
        for (int j = 0; j < interface.num_altsetting; j++) {
            const libusb_interface_descriptor &interfaceDescriptor = interface.altsetting[j];
            qDebug() << "    Interface number" << interfaceDescriptor.bInterfaceNumber;
            qDebug() << "    Interface number endpoints" << interfaceDescriptor.bNumEndpoints;
            for (int k = 0; k < interfaceDescriptor.bNumEndpoints; k++) {
                const libusb_endpoint_descriptor &endpoint = interfaceDescriptor.endpoint[k];
                qDebug() << "      Endpoint address" << endpoint.bEndpointAddress;
                qDebug() << "      Endpoint attributes" << endpoint.bmAttributes;
                qDebug() << "      Endpoint max packet size" << endpoint.wMaxPacketSize;
            }
        }
    }

    libusb_free_config_descriptor(configDescriptor);
}

Daemon::Daemon(QObject *parent) : Bigeye(parent)
{
    thread = new USBHandleEventThread(this);
    connect(thread, SIGNAL(dataArrived(QByteArray)), this, SLOT(onDataArrived(QByteArray)));
    thread->start();
}

void Daemon::defaultDispose(const QString &command, QDataStream &stream)
{
    Q_UNUSED(stream)
    qDebug() << "Unhanded command:" << command;
}

void Daemon::onDataArrived(const QByteArray &bytes)
{
    dispose(bytes);
}

void Daemon::queryDevice(QDataStream &stream)
{
    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respQueryDevice");
    istream << QString("daemon")
            << getDeviceType()
            << getFramebufferWidth()
            << getFramebufferHeight()
            << getFramebufferBitDepth();

    thread->tramsmitBytes(escape(block));
}

void Daemon::startDaemon(QDataStream &stream)
{
    QByteArray daemon;
    stream >> daemon;
    if (stream.status() == QDataStream::Ok) {
        QFile file("/tmp/bigeyeDaemon");
        file.open(QIODevice::WriteOnly);
        file.setPermissions(file.permissions() | QFile::ExeOwner);
        file.write(daemon);
        file.close();
    }
}

void Daemon::stopDaemon(QDataStream &stream)
{
    Q_UNUSED(stream)
}

void Daemon::snapshot(QDataStream &stream)
{
    Q_UNUSED(stream)

    QByteArray &buffer = getFramebuffer();
    sendExtendedData("snapshot", buffer);
}

void Daemon::videoFrame(QDataStream &stream)
{
    Q_UNUSED(stream)

    QByteArray &buffer = getFramebuffer();
    sendExtendedData("videoFrame", buffer);
}

void Daemon::executeProgram(QDataStream &stream)
{
    QString program;
    QStringList arguments;
    stream >> program;
    stream >> arguments;
    if (stream.status() == QDataStream::Ok) {
        QProcess::execute(program, arguments);
    }
}

void Daemon::executeProgramDetached(QDataStream &stream)
{
    QString program;
    QStringList arguments;
    stream >> program;
    stream >> arguments;
    if (stream.status() == QDataStream::Ok) {
        QProcess::startDetached(program, arguments);
    }
}

void Daemon::fileTransferPut(QDataStream &stream)
{
    QString src;
    QString dst;
    QByteArray data;
    stream >> src >> dst >> data;
    if (stream.status() == QDataStream::Ok) {
        QFile file(dst);
        file.open(QIODevice::WriteOnly);
        file.write(data);
        file.close();
    }
}

void Daemon::fileTransferGet(QDataStream &stream)
{
    QString src;
    QString dst;
    QByteArray data;
    stream >> src >> dst;
    if (stream.status() == QDataStream::Ok) {
        QFile file(src);
        file.open(QIODevice::ReadOnly);
        data = file.readAll();
        file.close();

        QByteArray block;
        QDataStream istream(&block, QIODevice::WriteOnly);
        istream.setVersion(QDataStream::Qt_4_8);
        istream << QString("Bigeye");
        istream << QString("respFileTransferGet");
        istream << src << dst << data;

        thread->tramsmitBytes(escape(block));
    }
}

void Daemon::sendExtendedData(const QString &category, QByteArray &buffer)
{
    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye") << QString("extendedData");
    istream << category;
    bool compressed = false;
    if (compressed) {
        buffer = qCompress(buffer);
    }
    istream << compressed << buffer.size();
    thread->tramsmitBytes(escape(block));
    thread->tramsmitBytes(buffer);
}
