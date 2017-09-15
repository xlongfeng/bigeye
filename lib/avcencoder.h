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

#ifndef AVCENCODER_H
#define AVCENCODER_H

#include <QThread>

#ifndef Q_NULLPTR
#define Q_NULLPTR         NULL
#endif

class AVCEncoder : public QThread
{
    Q_OBJECT
public:
    explicit AVCEncoder(QObject *parent = Q_NULLPTR);

    void stop();

protected:
    virtual void run();
};

#endif // AVCENCODER_H
