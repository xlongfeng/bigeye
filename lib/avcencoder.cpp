#include <QDebug>

#include "avcencoder.h"
#include "encoder.h"


AVCEncoder::AVCEncoder(QObject *parent) : QThread(parent)
{
    qDebug() << "encoder_init" << encoder_init();
}

void AVCEncoder::stop()
{
    encoder_quit();
}

void AVCEncoder::run()
{
    encoder_run();
}
