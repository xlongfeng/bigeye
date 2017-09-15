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

#ifndef ENCODER_H
#define ENCODER_H

#include <sys/cdefs.h>

__BEGIN_DECLS

#define err_msg(fmt, arg...)    printf("[ERR]\t" fmt, ## arg)
#define info_msg(fmt, arg...)   printf("[INFO]\t" fmt, ## arg)
#define warn_msg(fmt, arg...)   printf("[WARN]\t" fmt, ## arg)

typedef signed char s8;         /* 8 bit signed */
typedef unsigned char u8;       /* 8 bit unsigned */
typedef short s16;              /* 16 bit signed */
typedef unsigned short u16;     /* 16 bit unsigned */
typedef int s32;                /* 32 bit signed */
typedef unsigned int u32;       /* 32 bit unsigned */


#define SZ_4K               (4 * 1024)

#define STREAM_BUF_SIZE		0x200000
#define STREAM_FILL_SIZE	0x40000
#define STREAM_READ_SIZE	(512 * 8)
#define STREAM_END_SIZE		0
#define PS_SAVE_SIZE		0x080000
#define VP8_MB_SAVE_SIZE	0x080000
#define MPEG4_SCRATCH_SIZE	0x080000
#define MJPG_FILL_SIZE		(8 * 1024)

#define STREAM_ENC_PIC_RESET 	1

#define PATH_V4L2	0
#define PATH_FILE	1
#define PATH_NET	2
#define PATH_IPU	3

/* Test operations */
#define ENCODE		1
#define DECODE		2
#define LOOPBACK	3
#define TRANSCODE	4

#define DEFAULT_PORT		5555
#define DEFAULT_PKT_SIZE	0x28000

#define SIZE_USER_BUF            0x1000
#define USER_DATA_INFO_OFFSET    8*17

enum {
    MODE420 = 0,
    MODE422 = 1,
    MODE224 = 2,
    MODE444 = 3,
    MODE400 = 4
};

#define MAX_PATH	256
struct cmd_line {
    int src_fd;
    int dst_fd;
    int fb_size;
    int bitdepth;
    int width;
    int height;
    int enc_width;
    int enc_height;
    int format;
    int rot_en;
    int ipu_rot_en;
    int rot_angle;
    int mirror;
    int chromaInterleave;
    int bitrate;
    int gop;
    int save_enc_hdr;
    int count;
    int mp4_h264Class;
    int fps;
    int mapType;
    int quantParam;
};

#ifdef __arm__

#include <vpu_lib.h>
#include <vpu_io.h>

struct frame_buf {
    int addrY;
    int addrCb;
    int addrCr;
    int strideY;
    int strideC;
    int mvColBuf;
    vpu_mem_desc desc;
};

struct encode {
    EncHandle handle;		/* Encoder handle */
    PhysicalAddress phy_bsbuf_addr; /* Physical bitstream buffer */
    u32 virt_bsbuf_addr;		/* Virtual bitstream buffer */
    int enc_picwidth;	/* Encoded Picture width */
    int enc_picheight;	/* Encoded Picture height */
    int src_picwidth;        /* Source Picture width */
    int src_picheight;       /* Source Picture height */
    int totalfb;	/* Total number of framebuffers allocated */
    int src_fbid;	/* Index of frame buffer that contains YUV image */
    FrameBuffer *fb; /* frame buffer base given to encoder */
    struct frame_buf **pfbpool; /* allocated fb pointers are stored here */
    ExtBufCfg scratchBuf;
    int mp4_dataPartitionEnable;
    int ringBufferEnable;
    int mjpg_fmt;
    int mvc_paraset_refresh_en;
    int mvc_extension;
    int linear2TiledEnable;
    int minFrameBufferCount;

    EncReportInfo mbInfo;
    EncReportInfo mvInfo;
    EncReportInfo sliceInfo;

    struct cmd_line *cmdl; /* command line */
    u8 * huffTable;
    u8 * qMatTable;
};

#endif

int encoder_init(void);
void encoder_uninit(void);
int encoder_run(void);
void encoder_quit(void);

__END_DECLS

#endif /* ENCODER_H */
