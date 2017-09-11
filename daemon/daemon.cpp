#include <QDebug>

#include "daemon.h"

static const int UsbReceiveBufferSize = 16384;

USBHandleEventThread::USBHandleEventThread(QObject *parent) : QThread(parent)
#if QT_VERSION < QT_VERSION_CHECK(5, 2, 0)
    , interrupt(false)
#endif
{
    pingRreceiveBuffer.resize(UsbReceiveBufferSize);
    pongRreceiveBuffer.resize(UsbReceiveBufferSize);
    pingReceiveTransfer = Q_NULLPTR;
    pongReceiveTransfer = Q_NULLPTR;

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
    if (pongReceiveTransfer) {
        libusb_free_transfer(pongReceiveTransfer);
        pongReceiveTransfer = Q_NULLPTR;
    }
    if (pingReceiveTransfer) {
        libusb_free_transfer(pingReceiveTransfer);
        pingReceiveTransfer = Q_NULLPTR;
    }
    libusb_exit(ctx);
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

void USBHandleEventThread::run()
{
    qDebug() << "USBHandleEventThread" << "start";

    forever {
        if (isInterruptionRequested())
            break;
        qDebug() << "USBHandleEventThread" << "handle";
        libusb_handle_events_completed(Q_NULLPTR, Q_NULLPTR);
    }

    qDebug() << "USBHandleEventThread" << "end";
}

void USBHandleEventThread::startReceive()
{
    libusb_claim_interface(device, 0);

    pingReceiveTransfer = libusb_alloc_transfer(0);
    pongReceiveTransfer = libusb_alloc_transfer(0);

    libusb_fill_bulk_transfer(pingReceiveTransfer, device, 129,
            (unsigned char *)pingRreceiveBuffer.data(), UsbReceiveBufferSize,
            receiveTransferCallback, this, 0);

    libusb_fill_bulk_transfer(pongReceiveTransfer, device, 129,
            (unsigned char *)pongRreceiveBuffer.data(), UsbReceiveBufferSize,
            receiveTransferCallback, this, 0);

    libusb_submit_transfer(pingReceiveTransfer);
    libusb_submit_transfer(pongReceiveTransfer);
}

void USBHandleEventThread::stopReceive()
{
    libusb_cancel_transfer(pongReceiveTransfer);
    libusb_cancel_transfer(pingReceiveTransfer);
    pongReceiveTransfer = Q_NULLPTR;
    pingReceiveTransfer = Q_NULLPTR;
}

void USBHandleEventThread::receiveTransferCallback(libusb_transfer *transfer)
{
    USBHandleEventThread *self = (USBHandleEventThread *)transfer->user_data;
    const char *data = (const char *)transfer->buffer;
    qDebug() << "Receive transfer callback" << transfer->status;
    switch (transfer->status) {
    case LIBUSB_TRANSFER_COMPLETED:
        qDebug() << transfer->actual_length;
        self->dataArrived(QByteArray(data, transfer->actual_length));
        libusb_submit_transfer(transfer);
        break;
    case LIBUSB_TRANSFER_STALL:
    case LIBUSB_TRANSFER_ERROR:
    case LIBUSB_TRANSFER_TIMED_OUT:
    case LIBUSB_TRANSFER_OVERFLOW:
        break;
    case LIBUSB_TRANSFER_CANCELLED:
        break;
    case LIBUSB_TRANSFER_NO_DEVICE:
        break;
    default:
        break;
    }
}

int USBHandleEventThread::hotplugCallback(libusb_context *ctx, libusb_device *dev,
                            libusb_hotplug_event event, void *user_data)
{
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

Daemon::Daemon(QObject *parent) : QObject(parent)
{
    thread = new USBHandleEventThread(this);
    thread->start();
}
