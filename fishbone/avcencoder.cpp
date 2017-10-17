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
