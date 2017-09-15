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

#include <QCoreApplication>
#include "repeater.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Repeater repeater0(QLatin1String("ttyGS0"), true);
    Repeater repeater1(QLatin1String("ttyGS1"));
    QObject::connect(&repeater0, SIGNAL(dataArrived(const char*,int)),
                     &repeater1, SLOT(onDataArrived(const char*,int)));
    QObject::connect(&repeater1, SIGNAL(dataArrived(const char*,int)),
                     &repeater0, SLOT(onDataArrived(const char*,int)));

    return a.exec();
}
