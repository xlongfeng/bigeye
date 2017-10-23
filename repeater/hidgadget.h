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

#ifndef HIDGADGET_H
#define HIDGADGET_H

#include <QObject>

#ifndef Q_NULLPTR
#define Q_NULLPTR         NULL
#endif

class HidGadget : public QObject
{
    Q_OBJECT

public:
    static HidGadget *instance();

    int open();
    void close();
    void report(int code, bool down);

private:
    explicit HidGadget(QObject *parent = Q_NULLPTR);
    Q_DISABLE_COPY(HidGadget)

private:
    static HidGadget *self;

    int fd;
};

#endif // HIDGADGET_H
