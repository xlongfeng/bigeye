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

#include <sys/vfs.h>

#include <QMetaObject>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QDebug>

#include "daemon.h"
#include "option.h"


Daemon::Daemon(QObject *parent) :
    Bigeye(parent),
    num_cpus(2)
{
    cpu_jif = (jiffy_counts_t *)calloc(num_cpus, sizeof(jiffy_counts_t));
    cpu_prev_jif = (jiffy_counts_t *)calloc(num_cpus, sizeof(jiffy_counts_t));

    linker = new BigeyeLinker(this);
    connect(linker, SIGNAL(dataArrived(QByteArray)), this, SLOT(onDataArrived(QByteArray)));
    linker->start();
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
    Q_UNUSED(stream)
    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respQueryDevice");
    istream << QString("daemon")
            << QString("1.0")
            << getDeviceType()
            << getFramebufferWidth()
            << getFramebufferHeight()
            << getFramebufferBitDepth();

    linker->tramsmitBytes(escape(block));
}

void Daemon::startDaemon(QDataStream &stream)
{
    Q_UNUSED(stream)
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
    bool detached;
    QString program;
    QStringList arguments;
    QString preCommand;
    QString postCommand;
    stream >> detached >> program >> arguments >> preCommand >> postCommand;
    if (stream.status() == QDataStream::Ok) {
        if (preCommand.length() > 0)
            QProcess::execute(preCommand);

        if (detached)
            QProcess::startDetached(program, arguments);
        else
            QProcess::execute(program, arguments);

        if (postCommand.length() > 0)
            QProcess::execute(postCommand);
    }
}

void Daemon::executeRemoteProgram(QDataStream &stream)
{
    bool detached;
    QString program;
    QStringList arguments;
    QString preCommand;
    QString postCommand;
    QByteArray image;
    stream >> detached >> program >> arguments >> preCommand >> postCommand >> image;
    if (stream.status() == QDataStream::Ok) {
        if (preCommand.length() > 0)
            system(preCommand.toLatin1().data());

        QFileInfo programImageInfo(program);
        program = QString("/tmp/").append(programImageInfo.fileName());
        QFile file(program);
        file.open(QIODevice::WriteOnly);
        file.setPermissions(file.permissions() | QFile::ExeOwner);
        file.write(image);
        file.close();

        if (detached)
            QProcess::startDetached(program, arguments);
        else
            QProcess::execute(program, arguments);

        if (postCommand.length() > 0)
            system(postCommand.toLatin1().data());
    }
}

void Daemon::getCpuStat(QDataStream &stream)
{
    Q_UNUSED(stream)

    get_jiffy_counts();

    /*
     * xxx% = (cur_jif.xxx - prev_jif.xxx) / (cur_jif.total - prev_jif.total) * 100%
     */
    unsigned total_diff;
    jiffy_counts_t *p_jif, *p_prev_jif;
    int i;
    int n_cpu_lines;

    /* using (unsigned) casts to make operations cheaper */
# define  CALC_TOTAL_DIFF do { \
    total_diff = (unsigned)(p_jif->total - p_prev_jif->total); \
    if (total_diff == 0) total_diff = 1; \
} while (0)

#define CALC_STAT(xxx) unsigned xxx = 100 * (unsigned)(p_jif->xxx - p_prev_jif->xxx) / total_diff

    /* Loop thru CPU(s) */
    n_cpu_lines = num_cpus;

    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respGetCpuStat");
    istream << (quint32)num_cpus;

    for (i = 0; i < n_cpu_lines; i++) {
        p_jif = &cpu_jif[i];
        p_prev_jif = &cpu_prev_jif[i];

        CALC_TOTAL_DIFF;

        { /* Need a block: CALC_STAT are declarations */
            CALC_STAT(usr);
            CALC_STAT(sys);
            CALC_STAT(nic);
            CALC_STAT(idle);
            CALC_STAT(iowait);
            CALC_STAT(irq);
            CALC_STAT(softirq);
            /*CALC_STAT(steal);*/
            // qDebug() << "cpu" << i << usr << sys << nic << idle << iowait << irq << softirq;
            istream << (quint32)usr << (quint32)sys << (quint32)nic << (quint32)idle
                    << (quint32)iowait << (quint32)irq << (quint32)softirq;
        }
    }

    linker->tramsmitBytes(escape(block));
}

void Daemon::getMemInfo(QDataStream &stream)
{
    Q_UNUSED(stream)

    char buf[80];
    unsigned long mtotal, mfree, buffers, cached;
    FILE *fp = fopen("/proc/meminfo", "r");
    fscanf(fp, "MemTotal: %lu %s\n", &mtotal, buf);
    fscanf(fp, "MemFree: %lu %s\n", &mfree, buf);
    fscanf(fp, "Buffers: %lu %s\n", &buffers, buf);
    fscanf(fp, "Cached: %lu %s\n", &cached, buf);
    fclose(fp);

    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respGetMemInfo");
    istream << (quint32)mtotal << (quint32)mfree << (quint32)buffers << (quint32)cached;

    linker->tramsmitBytes(escape(block));
}

void Daemon::getDiskVolume(QDataStream &stream)
{
    Q_UNUSED(stream)

    struct statfs s;
    if (statfs("/", &s) != 0) {
        qDebug() << "Mount point / statfs failed";
        return;
    }
    // qDebug() << s.f_blocks << s.f_bfree << s.f_bavail << s.f_bsize;

    unsigned long mtotal, mfree;
    mtotal = s.f_blocks * s.f_bsize / 1024;
    mfree = s.f_bfree * s.f_bsize / 1024;

    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respGetDiskVolume");
    istream << (quint32)mtotal << (quint32)mfree;

    linker->tramsmitBytes(escape(block));
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

        linker->tramsmitBytes(escape(block));
    }
}

void Daemon::setOption(QDataStream &stream)
{
    QString name, value;
    stream >> name >> value;
    option->setenv(name.toLatin1().data(), value.toLatin1().data());
    option->saveenv();
}

void Daemon::getOption(QDataStream &stream)
{
    QString name;
    stream >> name;
    QString value = option->getenv(name.toLatin1().data());

    QByteArray block;
    QDataStream istream(&block, QIODevice::WriteOnly);
    istream.setVersion(QDataStream::Qt_4_8);
    istream << QString("Bigeye");
    istream << QString("respGetOption");
    istream << name << value;

    linker->tramsmitBytes(escape(block));
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
    linker->tramsmitBytes(escape(block));
    linker->tramsmitBytes(buffer);
}

int Daemon::read_cpu_jiffy(FILE *fp, jiffy_counts_t *p_jif)
{
    static const char fmt[] = "cp%*s %llu %llu %llu %llu %llu %llu %llu %llu";
    char line_buf[80];
    int ret;

    if (!fgets(line_buf, 80, fp) || line_buf[0] != 'c' /* not "cpu" */)
        return 0;
    ret = sscanf(line_buf, fmt,
            &p_jif->usr, &p_jif->nic, &p_jif->sys, &p_jif->idle,
            &p_jif->iowait, &p_jif->irq, &p_jif->softirq,
            &p_jif->steal);
    if (ret >= 4) {
        p_jif->total = p_jif->usr + p_jif->nic + p_jif->sys + p_jif->idle
            + p_jif->iowait + p_jif->irq + p_jif->softirq + p_jif->steal;
        /* procps 2.x does not count iowait as busy time */
        p_jif->busy = p_jif->total - p_jif->idle - p_jif->iowait;
    }

    return ret;
}

void Daemon::get_jiffy_counts()
{
    FILE* fp = fopen("/proc/stat", "r");

    /* We need to parse cumulative counts even if SMP CPU display is on,
     * they are used to calculate per process CPU% */
    prev_jif = cur_jif;
    if (read_cpu_jiffy(fp, &cur_jif) < 4) {
        qDebug() << "can't read /proc/stat";
        return;
    }

    jiffy_counts_t *tmp;
    int i;

    /* First switch the sample pointers: no need to copy */
    tmp = cpu_prev_jif;
    cpu_prev_jif = cpu_jif;
    cpu_jif = tmp;

    /* Get the new samples */
    for (i = 0; i < num_cpus; i++)
        read_cpu_jiffy(fp, &cpu_jif[i]);

    fclose(fp);
}
