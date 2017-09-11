#include <QMetaObject>
#include <QProcess>
#include <QFile>
#include <QDebug>

#include "hidgadget.h"
#include "repeater.h"
#include "qextserialport.h"
#include "avcencoder.h"

Repeater::Repeater(const QString &portName, QObject *parent) : Bigeye(parent)
{
    port = new QextSerialPort(portName, QextSerialPort::EventDriven);

    if (port->open(QIODevice::ReadWrite) == true) {
        connect(port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        qDebug() << "Starting bigeye repeater at" << port->portName();
    } else {
        qDebug() << "device failed to open:" << port->errorString();
    }
}

void Repeater::onDataArrived(const QByteArray &bytes)
{
    qDebug() << port->portName() << "onDataArrived enter";
    port->write(bytes);
    qDebug() << port->portName() << "onDataArrived exit";
}

void Repeater::onReadyRead()
{
    QByteArray bytes;
    int len = port->bytesAvailable();
    bytes.resize(len);
    port->read(bytes.data(), bytes.size());

    qDebug() << port->portName() << "onReadyRead enter";
    emit dataArrived(bytes);

    dispose(bytes);
    qDebug() << port->portName() << "onReadyRead exit";
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

#if 1
    qDebug() << "Key Event" << code << down;
#else
    HidGadget::instance()->report(code, down);
#endif
}

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
