#include <QMetaObject>
#include <QTimer>
#include <QProcess>
#include <QFile>
#include <QDebug>

#include "hidgadget.h"
#include "repeater.h"
#include "qextserialport.h"
#include "avcencoder.h"

#undef POLL_MODE

Repeater::Repeater(const QString &portName, bool isStudio, QObject *parent) :
    Bigeye(parent),
    isStudio(isStudio)
{
#ifdef POLL_MODE
    port = new QextSerialPort(portName, QextSerialPort::Polling);
    pollTimer = new QTimer(this);
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(onPollTimeout()));
#else
    port = new QextSerialPort(portName, QextSerialPort::EventDriven);
    connect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
#endif

    buffer.reserve(BufferSize);

    if (port->open(QIODevice::ReadWrite) == true) {
        qDebug() << "Starting bigeye repeater at" << port->portName();
    } else {
        qDebug() << "device failed to open:" << port->errorString();
    }

#ifdef POLL_MODE
    pollTimer = new QTimer(this);
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(onPollTimeout()));
    pollTimer->start(10);
#endif
}

void Repeater::onPollTimeout()
{
    onReadyRead();
}

void Repeater::onDataArrived(const char *data, int length)
{
    port->write(data, length);
}

void Repeater::onReadyRead()
{
#ifdef POLL_MODE
    forever {
        int lenght = port->read(buffer.data(), BufferSize);
        if (lenght > 0) {
            emit dataArrived(buffer.constData(), lenght);
            if (!isStudio) {
                qDebug() << port->portName() << lenght;
            }
            if (isStudio)
                dispose(QByteArray::fromRawData(buffer.constData(), lenght));
        } else {
            break;
        }
        break;
    }
#else
    int lenght = port->read(buffer.data(), BufferSize);
    if (lenght > 0) {
        emit dataArrived(buffer.constData(), lenght);
        if (isStudio)
            dispose(QByteArray::fromRawData(buffer.constData(), lenght));
    }
#endif
}

void Repeater::startDaemon(QDataStream &stream)
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

void Repeater::stopDaemon(QDataStream &stream)
{
    Q_UNUSED(stream)
}

void Repeater::keyEvent(QDataStream &stream)
{
    int code;
    bool down;
    stream >> code >> down;

    // qDebug() << "Key Event start" << code << down;
    HidGadget::instance()->report(code, down);
    // qDebug() << "Key Event end" << code << down;
}

void Repeater::extendedData(QDataStream &stream)
{
    QString category;
    bool compressed;
    int dataSize;
    stream >> category >> compressed >> dataSize;

    if (stream.status() == QDataStream::Ok) {
        setExtendDataSize(dataSize);
    }
}

#if 0
void Repeater::snapshot(QDataStream &stream)
{
    Q_UNUSED(stream)

    QByteArray &buffer = getFramebuffer();

    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respSnapshot");
    istream << getFramebufferWidth()
            << getFramebufferHeight()
            << getFramebufferBitDepth();
    bool compressed = false;
    if (compressed) {
        buffer = qCompress(buffer);
    }
    istream << compressed << buffer.size();

    port->write(escape(block));
    sendExtendedData("snapshot", buffer);
}

void Repeater::videoFrame(QDataStream &stream)
{
    Q_UNUSED(stream)

    QByteArray &buffer = getFramebuffer();

    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respVideoFrame");
    istream << getFramebufferWidth()
            << getFramebufferHeight()
            << getFramebufferBitDepth();
    bool compressed = false;
    if (compressed) {
        buffer = qCompress(buffer);
    }
    istream << compressed << buffer.size();

    port->write(escape(block));
    sendExtendedData("videoFrame", buffer);
}

void Repeater::executeProgram(QDataStream &stream)
{
    QString program;
    QStringList arguments;
    stream >> program;
    stream >> arguments;
    if (stream.status() == QDataStream::Ok) {
        QProcess::execute(program, arguments);
    }
}

void Repeater::executeProgramDetached(QDataStream &stream)
{
    QString program;
    QStringList arguments;
    stream >> program;
    stream >> arguments;
    if (stream.status() == QDataStream::Ok) {
        QProcess::startDetached(program, arguments);
    }
}

void Repeater::fileTransferPut(QDataStream &stream)
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

void Repeater::fileTransferGet(QDataStream &stream)
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

        port->write(escape(block));
    }
}

void Repeater::sendExtendedData(const QString &category, QByteArray &buffer)
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
    port->write(escape(block));
    port->write(buffer);
}
#endif
