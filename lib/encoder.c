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

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "encoder.h"

/* #define DEBUG_YUV_FILE */

#ifndef __arm__
typedef enum {
    STD_MPEG4 = 0,
    STD_H263 = 1,
    STD_AVC = 2,
    STD_VC1 = 3,
    STD_MPEG2 = 4,
    STD_DIV3 =5,
    STD_RV = 6,
    STD_MJPG = 7,
    STD_AVS = 8,
    STD_VP8 = 9
} CodStd;
#endif

static int quitflag;
static struct cmd_line *cmdl;
static u16 *framebuffer;
static u8 *y_table;
static u16 *uv_table;

static void encoder_preinit(void)
{
    static struct cmd_line cmd_line;
    struct fb_var_screeninfo fb_varinfo;
    int length, i;
    u8 r, g, b, u, v;

    cmdl = &cmd_line;
    memset(&fb_varinfo, 0, sizeof(struct fb_var_screeninfo));

    const char *device = "/dev/fb0";
    cmdl->src_fd = open(device, O_RDONLY);

    if (cmdl->src_fd == -1) {
        err_msg("Couldn't open %s\n", device);
    } else {
        if (ioctl(cmdl->src_fd, FBIOGET_VSCREENINFO, &fb_varinfo) != 0) {
            err_msg("ioctl FBIOGET_VSCREENINFO\n");
            cmdl->bitdepth = 16;
            cmdl->width = 800;
            cmdl->height = 600;
        } else {
            cmdl->bitdepth = (int) fb_varinfo.bits_per_pixel;
            cmdl->width = (int) fb_varinfo.xres;
            cmdl->height = (int) fb_varinfo.yres;
        }
    }

#ifndef __arm__
    cmdl->bitdepth = 16;
    cmdl->width = 800;
    cmdl->height = 600;
#endif

    cmdl->fb_size = cmdl->width * cmdl->height * (((unsigned int)cmdl->bitdepth + 7) >> 3);

    info_msg("Encoder image size %dx%d (%d)\n", cmdl->width, cmdl->height, cmdl->fb_size);

    framebuffer = malloc(cmdl->fb_size);
    length = 1 << cmdl->bitdepth;
    y_table = malloc(length);
    uv_table = malloc(length << 1);

    for (i = 0; i < length; i++) {
        b = (i >> 0 & 0x1f) << 3;
        g = (i >> 5 & 0x3f) << 2;
        r = (i >> 11 & 0x1f) << 3;
        y_table[i] = (u8)(0.299*(float)r + 0.587*(float)g + 0.114*(float)b);
        u = (u8)(-0.1687*(float)r - 0.3313*(float)g + 0.5*(float)b + 128.0);
        v = (u8)(0.5*(float)r - 0.4187*(float)g - 0.0813*(float)b + 128.0);
        uv_table[i] = v << 8 | u;
#ifndef __arm__
        printf("r %d, g %d, b %d, y %d, u %d, v %d\n",
               r, g, b, y_table[i], uv_table[i] & 0xff, uv_table[i] >> 8 & 0xff);
#endif
    }

#ifdef DEBUG_YUV_FILE
    cmdl->dst_fd = open("/tmp/encoder.yuv",
                        O_CREAT | O_RDWR | O_TRUNC,
                        S_IRWXU | S_IRWXG | S_IRWXO);
#else
    cmdl->dst_fd = open("/tmp/encoder.mp4",
                        O_CREAT | O_RDWR | O_TRUNC,
                        S_IRWXU | S_IRWXG | S_IRWXO);
#endif

    cmdl->format = STD_AVC;
    cmdl->chromaInterleave = 1;
    cmdl->fps = 15;
#ifdef DEBUG_YUV_FILE
    cmdl->count = 5;
#else
    cmdl->count = 300;
#endif
}

int vpu_write(struct cmd_line *cmd, char *buf, int n)
{
#ifndef DEBUG_YUV_FILE
    int ret = write(cmd->dst_fd, buf, n);
    if (ret != n)
        printf("vpu write %p %d %d\n", buf, n, ret);
#endif
    return 0;
}

#ifdef __arm__

#define NUM_FRAME_BUFS	32
#define FB_INDEX_MASK	(NUM_FRAME_BUFS - 1)

static int fb_index;
static struct frame_buf *fbarray[NUM_FRAME_BUFS];
static struct frame_buf fbpool[NUM_FRAME_BUFS];

void framebuf_init(void)
{
    int i;

    for (i = 0; i < NUM_FRAME_BUFS; i++) {
        fbarray[i] = &fbpool[i];
    }
}

struct frame_buf *get_framebuf(void)
{
    struct frame_buf *fb;

    fb = fbarray[fb_index];
    fbarray[fb_index] = 0;

    ++fb_index;
    fb_index &= FB_INDEX_MASK;

    return fb;
}

void put_framebuf(struct frame_buf *fb)
{
    --fb_index;
    fb_index &= FB_INDEX_MASK;

    fbarray[fb_index] = fb;
}

struct frame_buf *framebuf_alloc(int stdMode, int format, int strideY, int height, int mvCol)
{
    struct frame_buf *fb;
    int err;
    int divX, divY;

    fb = get_framebuf();
    if (fb == NULL)
        return NULL;

    divX = (format == MODE420 || format == MODE422) ? 2 : 1;
    divY = (format == MODE420 || format == MODE224) ? 2 : 1;

    memset(&(fb->desc), 0, sizeof(vpu_mem_desc));
    fb->desc.size = (strideY * height  + strideY / divX * height / divY * 2);
    if (mvCol)
        fb->desc.size += strideY / divX * height / divY;

    err = IOGetPhyMem(&fb->desc);
    if (err) {
        printf("Frame buffer allocation failure\n");
        memset(&(fb->desc), 0, sizeof(vpu_mem_desc));
        return NULL;
    }

    fb->addrY = fb->desc.phy_addr;
    fb->addrCb = fb->addrY + strideY * height;
    fb->addrCr = fb->addrCb + strideY / divX * height / divY;
    fb->strideY = strideY;
    fb->strideC =  strideY / divX;
    if (mvCol)
        fb->mvColBuf = fb->addrCr + strideY / divX * height / divY;

    fb->desc.virt_uaddr = IOGetVirtMem(&(fb->desc));
    if (fb->desc.virt_uaddr <= 0) {
        IOFreePhyMem(&fb->desc);
        memset(&(fb->desc), 0, sizeof(vpu_mem_desc));
        return NULL;
    }

    return fb;
}

struct frame_buf *tiled_framebuf_alloc(int stdMode, int format, int strideY, int height, int mvCol, int mapType)
{
    struct frame_buf *fb;
    int err, align;
    int divX, divY;
    Uint32 lum_top_base, lum_bot_base, chr_top_base, chr_bot_base;
    Uint32 lum_top_20bits, lum_bot_20bits, chr_top_20bits, chr_bot_20bits;
    int luma_top_size, luma_bot_size, chroma_top_size, chroma_bot_size;

    fb = get_framebuf();
    if (fb == NULL)
        return NULL;

    divX = (format == MODE420 || format == MODE422) ? 2 : 1;
    divY = (format == MODE420 || format == MODE224) ? 2 : 1;

    memset(&(fb->desc), 0, sizeof(vpu_mem_desc));

    /*
     * The buffers is luma top, chroma top, luma bottom and chroma bottom for
     * tiled map type, and only 20bits for the address description, so we need
     * to do 4K page align for each buffer.
     */
    align = SZ_4K;
    if (mapType == TILED_FRAME_MB_RASTER_MAP) {
        /* luma_top_size means the Y size of one frame, chroma_top_size
         * means the interleaved UV size of one frame in frame tiled map type*/
        luma_top_size = (strideY * height + align - 1) & ~(align - 1);
        chroma_top_size = (strideY / divX * height / divY * 2 + align - 1) & ~(align - 1);
        luma_bot_size = chroma_bot_size = 0;
    } else {
        /* This is FIELD_FRAME_MB_RASTER_MAP case, there are two fields */
        luma_top_size = (strideY * height / 2 + align - 1) & ~(align - 1);
        luma_bot_size = luma_top_size;
        chroma_top_size = (strideY / divX * height / divY + align - 1) & ~(align - 1);
        chroma_bot_size = chroma_top_size;
    }
    fb->desc.size = luma_top_size + chroma_top_size + luma_bot_size + chroma_bot_size;
    /* There is possible fb->desc.phy_addr in IOGetPhyMem not 4K page align,
     * so add more SZ_4K byte here for alignment */
    fb->desc.size += align - 1;

    if (mvCol)
        fb->desc.size += strideY / divX * height / divY;

    err = IOGetPhyMem(&fb->desc);
    if (err) {
        printf("Frame buffer allocation failure\n");
        memset(&(fb->desc), 0, sizeof(vpu_mem_desc));
        return NULL;
    }

    fb->desc.virt_uaddr = IOGetVirtMem(&(fb->desc));
    if (fb->desc.virt_uaddr <= 0) {
        IOFreePhyMem(&fb->desc);
        memset(&(fb->desc), 0, sizeof(vpu_mem_desc));
        return NULL;
    }

    lum_top_base = (fb->desc.phy_addr + align - 1) & ~(align -1);
    chr_top_base = lum_top_base + luma_top_size;
    if (mapType == TILED_FRAME_MB_RASTER_MAP) {
        lum_bot_base = 0;
        chr_bot_base = 0;
    } else {
        lum_bot_base = chr_top_base + chroma_top_size;
        chr_bot_base = lum_bot_base + luma_bot_size;
    }

    lum_top_20bits = lum_top_base >> 12;
    lum_bot_20bits = lum_bot_base >> 12;
    chr_top_20bits = chr_top_base >> 12;
    chr_bot_20bits = chr_bot_base >> 12;

    /*
     * In tiled map format the construction of the buffer pointers is as follows:
     * 20bit = addrY [31:12]: lum_top_20bits
     * 20bit = addrY [11: 0], addrCb[31:24]: chr_top_20bits
     * 20bit = addrCb[23: 4]: lum_bot_20bits
     * 20bit = addrCb[ 3: 0], addrCr[31:16]: chr_bot_20bits
     */
    fb->addrY = (lum_top_20bits << 12) + (chr_top_20bits >> 8);
    fb->addrCb = (chr_top_20bits << 24) + (lum_bot_20bits << 4) + (chr_bot_20bits >> 16);
    fb->addrCr = chr_bot_20bits << 16;
    fb->strideY = strideY;
    fb->strideC = strideY / divX;
    if (mvCol) {
        if (mapType == TILED_FRAME_MB_RASTER_MAP) {
            fb->mvColBuf = chr_top_base + chroma_top_size;
        } else {
            fb->mvColBuf = chr_bot_base + chroma_bot_size;
        }
    }

    return fb;
}

void framebuf_free(struct frame_buf *fb)
{
    if (fb == NULL)
        return;

    if (fb->desc.virt_uaddr) {
        IOFreeVirtMem(&fb->desc);
    }

    if (fb->desc.phy_addr) {
        IOFreePhyMem(&fb->desc);
    }

    memset(&(fb->desc), 0, sizeof(vpu_mem_desc));
    put_framebuf(fb);
}

static vpu_mem_desc	mem_desc;
static vpu_mem_desc scratch_mem_desc;
static struct encode *enc;

static int enc_readbs_reset_buffer(struct encode *enc,
                                   PhysicalAddress paBsBufAddr, int bsBufsize)
{
    u32 vbuf;

    vbuf = enc->virt_bsbuf_addr + paBsBufAddr - enc->phy_bsbuf_addr;
    return vpu_write(enc->cmdl, (void *)vbuf, bsBufsize);
}

static int enc_readbs_ring_buffer(EncHandle handle, struct cmd_line *cmd,
                                  u32 bs_va_startaddr, u32 bs_va_endaddr, u32 bs_pa_startaddr,
                                  int defaultsize)
{
    RetCode ret;
    int space = 0, room;
    PhysicalAddress pa_read_ptr, pa_write_ptr;
    u32 target_addr, size;

    ret = vpu_EncGetBitstreamBuffer(handle, &pa_read_ptr, &pa_write_ptr,
                                    (Uint32 *)&size);
    if (ret != RETCODE_SUCCESS) {
        err_msg("EncGetBitstreamBuffer failed\n");
        return -1;
    }

    /* No space in ring buffer */
    if (size <= 0)
        return 0;

    if (defaultsize > 0) {
        if (size < defaultsize)
            return 0;

        space = defaultsize;
    } else {
        space = size;
    }

    if (space > 0) {
        target_addr = bs_va_startaddr + (pa_read_ptr - bs_pa_startaddr);
        if ( (target_addr + space) > bs_va_endaddr) {
            room = bs_va_endaddr - target_addr;
            vpu_write(cmd, (void *)target_addr, room);
            vpu_write(cmd, (void *)bs_va_startaddr,(space - room));
        } else {
            vpu_write(cmd, (void *)target_addr, space);
        }

        ret = vpu_EncUpdateBitstreamBuffer(handle, space);
        if (ret != RETCODE_SUCCESS) {
            err_msg("EncUpdateBitstreamBuffer failed\n");
            return -1;
        }
    }

    return space;
}

static int encoder_fill_headers(struct encode *enc)
{
    EncHeaderParam enchdr_param = {0};
    EncHandle handle = enc->handle;
    RetCode ret;
    int mbPicNum;

    /* Must put encode header before encoding */
    if (enc->cmdl->format == STD_MPEG4) {
        enchdr_param.headerType = VOS_HEADER;

        if (cpu_is_mx6x())
            goto put_mp4header;
        /*
                 * Please set userProfileLevelEnable to 0 if you need to generate
                 * user profile and level automaticaly by resolution, here is one
                 * sample of how to work when userProfileLevelEnable is 1.
                 */
        enchdr_param.userProfileLevelEnable = 1;
        mbPicNum = ((enc->enc_picwidth + 15) / 16) *((enc->enc_picheight + 15) / 16);
        if (enc->enc_picwidth <= 176 && enc->enc_picheight <= 144 &&
                mbPicNum * enc->cmdl->fps <= 1485)
            enchdr_param.userProfileLevelIndication = 8; /* L1 */
        /* Please set userProfileLevelIndication to 8 if L0 is needed */
        else if (enc->enc_picwidth <= 352 && enc->enc_picheight <= 288 &&
                 mbPicNum * enc->cmdl->fps <= 5940)
            enchdr_param.userProfileLevelIndication = 2; /* L2 */
        else if (enc->enc_picwidth <= 352 && enc->enc_picheight <= 288 &&
                 mbPicNum * enc->cmdl->fps <= 11880)
            enchdr_param.userProfileLevelIndication = 3; /* L3 */
        else if (enc->enc_picwidth <= 640 && enc->enc_picheight <= 480 &&
                 mbPicNum * enc->cmdl->fps <= 36000)
            enchdr_param.userProfileLevelIndication = 4; /* L4a */
        else if (enc->enc_picwidth <= 720 && enc->enc_picheight <= 576 &&
                 mbPicNum * enc->cmdl->fps <= 40500)
            enchdr_param.userProfileLevelIndication = 5; /* L5 */
        else
            enchdr_param.userProfileLevelIndication = 6; /* L6 */

put_mp4header:
        vpu_EncGiveCommand(handle, ENC_PUT_MP4_HEADER, &enchdr_param);
        if (enc->ringBufferEnable == 0 ) {
            ret = enc_readbs_reset_buffer(enc, enchdr_param.buf, enchdr_param.size);
            if (ret < 0)
                return -1;
        }

        enchdr_param.headerType = VIS_HEADER;
        vpu_EncGiveCommand(handle, ENC_PUT_MP4_HEADER, &enchdr_param);
        if (enc->ringBufferEnable == 0 ) {
            ret = enc_readbs_reset_buffer(enc, enchdr_param.buf, enchdr_param.size);
            if (ret < 0)
                return -1;
        }

        enchdr_param.headerType = VOL_HEADER;
        vpu_EncGiveCommand(handle, ENC_PUT_MP4_HEADER, &enchdr_param);
        if (enc->ringBufferEnable == 0 ) {
            ret = enc_readbs_reset_buffer(enc, enchdr_param.buf, enchdr_param.size);
            if (ret < 0)
                return -1;
        }
    } else if (enc->cmdl->format == STD_AVC) {
        if (!enc->mvc_extension || !enc->mvc_paraset_refresh_en) {
            enchdr_param.headerType = SPS_RBSP;
            vpu_EncGiveCommand(handle, ENC_PUT_AVC_HEADER, &enchdr_param);
            if (enc->ringBufferEnable == 0 ) {
                ret = enc_readbs_reset_buffer(enc, enchdr_param.buf, enchdr_param.size);
                if (ret < 0)
                    return -1;
            }
        }

        if (enc->mvc_extension) {
            enchdr_param.headerType = SPS_RBSP_MVC;
            vpu_EncGiveCommand(handle, ENC_PUT_AVC_HEADER, &enchdr_param);
            if (enc->ringBufferEnable == 0 ) {
                ret = enc_readbs_reset_buffer(enc, enchdr_param.buf, enchdr_param.size);
                if (ret < 0)
                    return -1;
            }
        }

        enchdr_param.headerType = PPS_RBSP;
        vpu_EncGiveCommand(handle, ENC_PUT_AVC_HEADER, &enchdr_param);
        if (enc->ringBufferEnable == 0 ) {
            ret = enc_readbs_reset_buffer(enc, enchdr_param.buf, enchdr_param.size);
            if (ret < 0)
                return -1;
        }

        if (enc->mvc_extension) { /* MVC */
            enchdr_param.headerType = PPS_RBSP_MVC;
            vpu_EncGiveCommand(handle, ENC_PUT_AVC_HEADER, &enchdr_param);
            if (enc->ringBufferEnable == 0 ) {
                ret = enc_readbs_reset_buffer(enc, enchdr_param.buf, enchdr_param.size);
                if (ret < 0)
                    return -1;
            }
        }
    }

    return 0;
}

void encoder_free_framebuffer(struct encode *enc)
{
    int i;

    for (i = 0; i < enc->totalfb; i++) {
        framebuf_free(enc->pfbpool[i]);
    }

    free(enc->fb);
    free(enc->pfbpool);
}

int encoder_allocate_framebuffer(struct encode *enc)
{
    EncHandle handle = enc->handle;
    int i, enc_stride, src_stride, src_fbid;
    int totalfb, minfbcount, srcfbcount, extrafbcount;
    RetCode ret;
    FrameBuffer *fb;
    PhysicalAddress subSampBaseA = 0, subSampBaseB = 0;
    struct frame_buf **pfbpool;
    EncExtBufInfo extbufinfo = {0};
    int enc_fbwidth, enc_fbheight, src_fbwidth, src_fbheight;

    minfbcount = enc->minFrameBufferCount;
    /* dprintf(4, "minfb %d\n", minfbcount); */
    srcfbcount = 1;

    enc_fbwidth = (enc->enc_picwidth + 15) & ~15;
    enc_fbheight = (enc->enc_picheight + 15) & ~15;
    src_fbwidth = (enc->src_picwidth + 15) & ~15;
    src_fbheight = (enc->src_picheight + 15) & ~15;

    if (cpu_is_mx6x()) {
        if (enc->cmdl->format == STD_AVC && enc->mvc_extension) /* MVC */
            extrafbcount = 2 + 2; /* Subsamp [2] + Subsamp MVC [2] */
        else if (enc->cmdl->format == STD_MJPG)
            extrafbcount = 0;
        else
            extrafbcount = 2; /* Subsamp buffer [2] */
    } else
        extrafbcount = 0;

    enc->totalfb = totalfb = minfbcount + extrafbcount + srcfbcount;

    /* last framebuffer is used as src frame in the test */
    enc->src_fbid = src_fbid = totalfb - 1;

    fb = enc->fb = calloc(totalfb, sizeof(FrameBuffer));
    if (fb == NULL) {
        err_msg("Failed to allocate enc->fb\n");
        return -1;
    }

    pfbpool = enc->pfbpool = calloc(totalfb,
                                    sizeof(struct frame_buf *));
    if (pfbpool == NULL) {
        err_msg("Failed to allocate enc->pfbpool\n");
        free(fb);
        return -1;
    }

    if (enc->cmdl->mapType == LINEAR_FRAME_MAP) {
        /* All buffers are linear */
        for (i = 0; i < minfbcount + extrafbcount; i++) {
            pfbpool[i] = framebuf_alloc(enc->cmdl->format, enc->mjpg_fmt,
                                        enc_fbwidth, enc_fbheight, 0);
            if (pfbpool[i] == NULL) {
                goto err1;
            }
        }
    } else {
        /* Encoded buffers are tiled */
        for (i = 0; i < minfbcount; i++) {
            pfbpool[i] = tiled_framebuf_alloc(enc->cmdl->format, enc->mjpg_fmt,
                                              enc_fbwidth, enc_fbheight, 0, enc->cmdl->mapType);
            if (pfbpool[i] == NULL)
                goto err1;
        }
        /* sub frames are linear */
        for (i = minfbcount; i < minfbcount + extrafbcount; i++) {
            pfbpool[i] = framebuf_alloc(enc->cmdl->format, enc->mjpg_fmt,
                                        enc_fbwidth, enc_fbheight, 0);
            if (pfbpool[i] == NULL)
                goto err1;
        }
    }

    for (i = 0; i < minfbcount + extrafbcount; i++) {
        fb[i].myIndex = i;
        fb[i].bufY = pfbpool[i]->addrY;
        fb[i].bufCb = pfbpool[i]->addrCb;
        fb[i].bufCr = pfbpool[i]->addrCr;
        fb[i].strideY = pfbpool[i]->strideY;
        fb[i].strideC = pfbpool[i]->strideC;
    }

    if (cpu_is_mx6x() && (enc->cmdl->format != STD_MJPG)) {
        subSampBaseA = fb[minfbcount].bufY;
        subSampBaseB = fb[minfbcount + 1].bufY;
        if (enc->cmdl->format == STD_AVC && enc->mvc_extension) { /* MVC */
            extbufinfo.subSampBaseAMvc = fb[minfbcount + 2].bufY;
            extbufinfo.subSampBaseBMvc = fb[minfbcount + 3].bufY;
        }
    }

    /* Must be a multiple of 16 */
    if (enc->cmdl->rot_angle == 90 || enc->cmdl->rot_angle == 270)
        enc_stride = (enc->enc_picheight + 15 ) & ~15;
    else
        enc_stride = (enc->enc_picwidth + 15) & ~15;
    src_stride = (enc->src_picwidth + 15 ) & ~15;

    extbufinfo.scratchBuf = enc->scratchBuf;
    ret = vpu_EncRegisterFrameBuffer(handle, fb, minfbcount, enc_stride, src_stride,
                                     subSampBaseA, subSampBaseB, &extbufinfo);
    if (ret != RETCODE_SUCCESS) {
        err_msg("Register frame buffer failed\n");
        goto err1;
    }

    /* Allocate a single frame buffer for source frame */
    pfbpool[src_fbid] = framebuf_alloc(enc->cmdl->format, enc->mjpg_fmt,
                                       src_fbwidth, src_fbheight, 0);
    if (pfbpool[src_fbid] == NULL) {
        err_msg("failed to allocate single framebuf\n");
        goto err1;
    }

    fb[src_fbid].myIndex = enc->src_fbid;
    fb[src_fbid].bufY = pfbpool[src_fbid]->addrY;
    fb[src_fbid].bufCb = pfbpool[src_fbid]->addrCb;
    fb[src_fbid].bufCr = pfbpool[src_fbid]->addrCr;
    fb[src_fbid].strideY = pfbpool[src_fbid]->strideY;
    fb[src_fbid].strideC = pfbpool[src_fbid]->strideC;

    return 0;

err1:
    for (i = 0; i < totalfb; i++) {
        framebuf_free(pfbpool[i]);
    }

    free(fb);
    free(pfbpool);
    return -1;
}

static int read_framebuffer(struct encode *enc)
{
    u32 y_addr, u_addr, v_addr;
    struct frame_buf *pfb = enc->pfbpool[enc->src_fbid];
    int divX, divY;
    int src_fd = enc->cmdl->src_fd;
    int format = enc->mjpg_fmt;
    int chromaInterleave = enc->cmdl->chromaInterleave;
    int y_size, c_size;
    u16 *buf;
    u8 *y;
    u16 *uv;
    int i, j, k;

    if (enc->src_picwidth != pfb->strideY) {
        err_msg("Make sure src pic width is a multiple of 16\n");
        return -1;
    }

    divX = (format == MODE420 || format == MODE422) ? 2 : 1;
    divY = (format == MODE420 || format == MODE224) ? 2 : 1;

    y_size = enc->src_picwidth * enc->src_picheight;
    c_size = y_size / divX / divY;

    y_addr = pfb->addrY + pfb->desc.virt_uaddr - pfb->desc.phy_addr;
    u_addr = pfb->addrCb + pfb->desc.virt_uaddr - pfb->desc.phy_addr;
    v_addr = pfb->addrCr + pfb->desc.virt_uaddr - pfb->desc.phy_addr;

    lseek(enc->cmdl->src_fd, 0, SEEK_SET);
    read(enc->cmdl->src_fd, framebuffer, enc->cmdl->fb_size);
    buf = framebuffer;
    y = (u8 *)y_addr;
    uv = (u16 *)u_addr;
    usleep(20000);

    for (j = 0; j < enc->src_picheight; j++) {
        for (i = 0; i < enc->src_picwidth; i++) {
            y[j * enc->src_picwidth + i] = y_table[buf[j * enc->src_picwidth + i]];
            if (((j % 2)) == 0 && ((i % 2)) == 0) {
                k = j >> 1;
                k *= enc->src_picwidth;
                k >>= 1;
                uv[k + (i >> 1)] = uv_table[buf[j * enc->src_picwidth + i]];
            }
        }
    }

#ifdef DEBUG_YUV_FILE
    printf("height %d, width %d y size %d, c size %d\n", enc->src_picwidth, enc->src_picheight, y_size, c_size);
    {
        int dst_fd = enc->cmdl->dst_fd;
        write(dst_fd, y, y_size);
        write(dst_fd, uv, c_size * 2);
    }
#endif

    return 1;
}

static int encoder_start(struct encode *enc)
{
    EncHandle handle = enc->handle;
    EncParam  enc_param = {0};
    EncOpenParam encop = {0};
    EncOutputInfo outinfo = {0};
    RetCode ret = 0;
    int src_fbid = enc->src_fbid, frame_id = 0;
    int count = enc->cmdl->count;
    struct timeval tenc_begin,tenc_end, total_start, total_end;
    int sec, usec, loop_id;
    float tenc_time = 0, total_time=0;
    PhysicalAddress phy_bsbuf_start = enc->phy_bsbuf_addr;
    u32 virt_bsbuf_start = enc->virt_bsbuf_addr;
    u32 virt_bsbuf_end = virt_bsbuf_start + STREAM_BUF_SIZE;

    /* Must put encode header here before encoding for all codec, except MX6 MJPG */
    if (!(cpu_is_mx6x() && (enc->cmdl->format == STD_MJPG))) {
        ret = encoder_fill_headers(enc);
        if (ret) {
            err_msg("Encode fill headers failed\n");
            return -1;
        }
    }

    enc_param.sourceFrame = &enc->fb[src_fbid];
    enc_param.quantParam = 23;
    enc_param.forceIPicture = 0;
    enc_param.skipPicture = 0;
    enc_param.enableAutoSkip = 1;

    enc_param.encLeftOffset = 0;
    enc_param.encTopOffset = 0;
    if ((enc_param.encLeftOffset + enc->enc_picwidth) > enc->src_picwidth) {
        err_msg("Configure is failure for width and left offset\n");
        return -1;
    }
    if ((enc_param.encTopOffset + enc->enc_picheight) > enc->src_picheight) {
        err_msg("Configure is failure for height and top offset\n");
        return -1;
    }

    /* Set report info flag */
    if (enc->mbInfo.enable) {
        ret = vpu_EncGiveCommand(handle, ENC_SET_REPORT_MBINFO, &enc->mbInfo);
        if (ret != RETCODE_SUCCESS) {
            err_msg("Failed to set MbInfo report, ret %d\n", ret);
            return -1;
        }
    }
    if (enc->mvInfo.enable) {
        ret = vpu_EncGiveCommand(handle, ENC_SET_REPORT_MVINFO, &enc->mvInfo);
        if (ret != RETCODE_SUCCESS) {
            err_msg("Failed to set MvInfo report, ret %d\n", ret);
            return -1;
        }
    }
    if (enc->sliceInfo.enable) {
        ret = vpu_EncGiveCommand(handle, ENC_SET_REPORT_SLICEINFO, &enc->sliceInfo);
        if (ret != RETCODE_SUCCESS) {
            err_msg("Failed to set slice info report, ret %d\n", ret);
            return -1;
        }
    }

    gettimeofday(&total_start, NULL);

    quitflag = 0;

    /* The main encoding loop */
    while (1) {
        ret = read_framebuffer(enc);
        if (ret <= 0)
            break;

        gettimeofday(&tenc_begin, NULL);
        ret = vpu_EncStartOneFrame(handle, &enc_param);
        if (ret != RETCODE_SUCCESS) {
            err_msg("vpu_EncStartOneFrame failed Err code:%d\n",
                    ret);
            goto err2;
        }

        loop_id = 0;
        while (vpu_IsBusy()) {
            vpu_WaitForInt(200);
            if (enc->ringBufferEnable == 1) {
                ret = enc_readbs_ring_buffer(handle, enc->cmdl,
                                             virt_bsbuf_start, virt_bsbuf_end,
                                             phy_bsbuf_start, STREAM_READ_SIZE);
                if (ret < 0) {
                    goto err2;
                }
            }
            if (loop_id == 20) {
                ret = vpu_SWReset(handle, 0);
                return -1;
            }
            loop_id ++;
        }

        gettimeofday(&tenc_end, NULL);
        sec = tenc_end.tv_sec - tenc_begin.tv_sec;
        usec = tenc_end.tv_usec - tenc_begin.tv_usec;

        if (usec < 0) {
            sec--;
            usec = usec + 1000000;
        }

        tenc_time += (sec * 1000000) + usec;

        ret = vpu_EncGetOutputInfo(handle, &outinfo);
        if (ret != RETCODE_SUCCESS) {
            err_msg("vpu_EncGetOutputInfo failed Err code: %d\n",
                    ret);
            goto err2;
        }

        if (outinfo.skipEncoded)
            info_msg("Skip encoding one Frame!\n");

        if (quitflag)
            break;

        if (enc->ringBufferEnable == 0) {
            ret = enc_readbs_reset_buffer(enc, outinfo.bitstreamBuffer, outinfo.bitstreamSize);
            if (ret < 0) {
                err_msg("writing bitstream buffer failed\n");
                goto err2;
            }
        } else
            enc_readbs_ring_buffer(handle, enc->cmdl, virt_bsbuf_start,
                                   virt_bsbuf_end, phy_bsbuf_start, 0);

        frame_id++;
        if ((count != 0) && (frame_id >= count))
            break;
    }

    gettimeofday(&total_end, NULL);
    sec = total_end.tv_sec - total_start.tv_sec;
    usec = total_end.tv_usec - total_start.tv_usec;
    if (usec < 0) {
        sec--;
        usec = usec + 1000000;
    }
    total_time = (sec * 1000000) + usec;

    info_msg("Finished encoding: %d frames\n", frame_id);
    info_msg("enc fps = %.2f\n", (frame_id / (tenc_time / 1000000)));
    info_msg("total fps= %.2f \n",(frame_id / (total_time / 1000000)));

err2:
    if (enc->mbInfo.addr)
        free(enc->mbInfo.addr);
    if (enc->mbInfo.addr)
        free(enc->mbInfo.addr);
    if (enc->sliceInfo.addr)
        free(enc->sliceInfo.addr);

    /* For automation of test case */
    if (ret > 0)
        ret = 0;

    return ret;
}

int encoder_configure(struct encode *enc)
{
    EncHandle handle = enc->handle;
    SearchRamParam search_pa = {0};
    EncInitialInfo initinfo = {0};
    RetCode ret;
    MirrorDirection mirror;

    if (cpu_is_mx27()) {
        search_pa.searchRamAddr = 0xFFFF4C00;
        ret = vpu_EncGiveCommand(handle, ENC_SET_SEARCHRAM_PARAM, &search_pa);
        if (ret != RETCODE_SUCCESS) {
            err_msg("Encoder SET_SEARCHRAM_PARAM failed\n");
            return -1;
        }
    }

    if (enc->cmdl->rot_en) {
        vpu_EncGiveCommand(handle, ENABLE_ROTATION, 0);
        vpu_EncGiveCommand(handle, ENABLE_MIRRORING, 0);
        vpu_EncGiveCommand(handle, SET_ROTATION_ANGLE,
                           &enc->cmdl->rot_angle);
        mirror = enc->cmdl->mirror;
        vpu_EncGiveCommand(handle, SET_MIRROR_DIRECTION, &mirror);
    }

    ret = vpu_EncGetInitialInfo(handle, &initinfo);
    if (ret != RETCODE_SUCCESS) {
        err_msg("Encoder GetInitialInfo failed\n");
        return -1;
    }

    enc->minFrameBufferCount = initinfo.minFrameBufferCount;
    if (enc->cmdl->save_enc_hdr) {
        if (enc->cmdl->format == STD_MPEG4) {
            SaveGetEncodeHeader(handle, ENC_GET_VOS_HEADER,
                                "mp4_vos_header.dat");
            SaveGetEncodeHeader(handle, ENC_GET_VO_HEADER,
                                "mp4_vo_header.dat");
            SaveGetEncodeHeader(handle, ENC_GET_VOL_HEADER,
                                "mp4_vol_header.dat");
        } else if (enc->cmdl->format == STD_AVC) {
            SaveGetEncodeHeader(handle, ENC_GET_SPS_RBSP,
                                "avc_sps_header.dat");
            SaveGetEncodeHeader(handle, ENC_GET_PPS_RBSP,
                                "avc_pps_header.dat");
        }
    }

    enc->mbInfo.enable = 0;
    enc->mvInfo.enable = 0;
    enc->sliceInfo.enable = 0;

    if (enc->mbInfo.enable) {
        enc->mbInfo.addr = malloc(initinfo.reportBufSize.mbInfoBufSize);
        if (!enc->mbInfo.addr)
            err_msg("malloc_error\n");
    }
    if (enc->mvInfo.enable) {
        enc->mvInfo.addr = malloc(initinfo.reportBufSize.mvInfoBufSize);
        if (!enc->mvInfo.addr)
            err_msg("malloc_error\n");
    }
    if (enc->sliceInfo.enable) {
        enc->sliceInfo.addr = malloc(initinfo.reportBufSize.sliceInfoBufSize);
        if (!enc->sliceInfo.addr)
            err_msg("malloc_error\n");
    }

    return 0;
}

void encoder_close(struct encode *enc)
{
    RetCode ret;

    ret = vpu_EncClose(enc->handle);
    if (ret == RETCODE_FRAME_NOT_COMPLETE) {
        vpu_SWReset(enc->handle, 0);
        vpu_EncClose(enc->handle);
    }
}

int encoder_open(struct encode *enc)
{
    EncHandle handle = {0};
    EncOpenParam encop = {0};
    Uint8 *huffTable = enc->huffTable;
    Uint8 *qMatTable = enc->qMatTable;
    int i;
    RetCode ret;

    /* Fill up parameters for encoding */
    encop.bitstreamBuffer = enc->phy_bsbuf_addr;
    encop.bitstreamBufferSize = STREAM_BUF_SIZE;
    encop.bitstreamFormat = enc->cmdl->format;
    encop.mapType = enc->cmdl->mapType;
    encop.linear2TiledEnable = enc->linear2TiledEnable;
    /* width and height in command line means source image size */
    if (enc->cmdl->width && enc->cmdl->height) {
        enc->src_picwidth = enc->cmdl->width;
        enc->src_picheight = enc->cmdl->height;
    }

    /* enc_width and enc_height in command line means encoder output size */
    if (enc->cmdl->enc_width && enc->cmdl->enc_height) {
        enc->enc_picwidth = enc->cmdl->enc_width;
        enc->enc_picheight = enc->cmdl->enc_height;
    } else {
        enc->enc_picwidth = enc->src_picwidth;
        enc->enc_picheight = enc->src_picheight;
    }

    /* If rotation angle is 90 or 270, pic width and height are swapped */
    if (enc->cmdl->rot_angle == 90 || enc->cmdl->rot_angle == 270) {
        encop.picWidth = enc->enc_picheight;
        encop.picHeight = enc->enc_picwidth;
    } else {
        encop.picWidth = enc->enc_picwidth;
        encop.picHeight = enc->enc_picheight;
    }

    if (enc->cmdl->fps == 0)
        enc->cmdl->fps = 30;

    info_msg("Capture/Encode fps will be %d\n", enc->cmdl->fps);

    /*Note: Frame rate cannot be less than 15fps per H.263 spec */
    encop.frameRateInfo = enc->cmdl->fps;
    encop.bitRate = enc->cmdl->bitrate;
    encop.gopSize = enc->cmdl->gop;
    encop.slicemode.sliceMode = 0;	/* 0: 1 slice per picture; 1: Multiple slices per picture */
    encop.slicemode.sliceSizeMode = 0; /* 0: silceSize defined by bits; 1: sliceSize defined by MB number*/
    encop.slicemode.sliceSize = 4000;  /* Size of a slice in bits or MB numbers */

    encop.initialDelay = 0;
    encop.vbvBufferSize = 0;        /* 0 = ignore 8 */
    encop.intraRefresh = 0;
    encop.sliceReport = 0;
    encop.mbReport = 0;
    encop.mbQpReport = 0;
    encop.rcIntraQp = -1;
    encop.userQpMax = 0;
    encop.userQpMin = 0;
    encop.userQpMinEnable = 0;
    encop.userQpMaxEnable = 0;

    encop.IntraCostWeight = 0;
    encop.MEUseZeroPmv  = 0;
    /* (3: 16x16, 2:32x16, 1:64x32, 0:128x64, H.263(Short Header : always 3) */
    encop.MESearchRange = 3;

    encop.userGamma = (Uint32)(0.75*32768);         /*  (0*32768 <= gamma <= 1*32768) */
    encop.RcIntervalMode= 1;        /* 0:normal, 1:frame_level, 2:slice_level, 3: user defined Mb_level */
    encop.MbInterval = 0;
    encop.avcIntra16x16OnlyModeEnable = 0;

    encop.ringBufferEnable = enc->ringBufferEnable = 0;
    encop.dynamicAllocEnable = 0;
    encop.chromaInterleave = enc->cmdl->chromaInterleave;

    if (enc->cmdl->format == STD_MPEG4) {
        encop.EncStdParam.mp4Param.mp4_dataPartitionEnable = 0;
        enc->mp4_dataPartitionEnable =
                encop.EncStdParam.mp4Param.mp4_dataPartitionEnable;
        encop.EncStdParam.mp4Param.mp4_reversibleVlcEnable = 0;
        encop.EncStdParam.mp4Param.mp4_intraDcVlcThr = 0;
        encop.EncStdParam.mp4Param.mp4_hecEnable = 0;
        encop.EncStdParam.mp4Param.mp4_verid = 2;
    } else if ( enc->cmdl->format == STD_H263) {
        encop.EncStdParam.h263Param.h263_annexIEnable = 0;
        encop.EncStdParam.h263Param.h263_annexJEnable = 1;
        encop.EncStdParam.h263Param.h263_annexKEnable = 0;
        encop.EncStdParam.h263Param.h263_annexTEnable = 0;
    } else if (enc->cmdl->format == STD_AVC) {
        encop.EncStdParam.avcParam.avc_constrainedIntraPredFlag = 0;
        encop.EncStdParam.avcParam.avc_disableDeblk = 0;
        encop.EncStdParam.avcParam.avc_deblkFilterOffsetAlpha = 6;
        encop.EncStdParam.avcParam.avc_deblkFilterOffsetBeta = 0;
        encop.EncStdParam.avcParam.avc_chromaQpOffset = 10;
        encop.EncStdParam.avcParam.avc_audEnable = 0;
        if (cpu_is_mx6x()) {
            encop.EncStdParam.avcParam.interview_en = 0;
            encop.EncStdParam.avcParam.paraset_refresh_en = enc->mvc_paraset_refresh_en = 0;
            encop.EncStdParam.avcParam.prefix_nal_en = 0;
            encop.EncStdParam.avcParam.mvc_extension = enc->cmdl->mp4_h264Class;
            enc->mvc_extension = enc->cmdl->mp4_h264Class;
            encop.EncStdParam.avcParam.avc_frameCroppingFlag = 0;
            encop.EncStdParam.avcParam.avc_frameCropLeft = 0;
            encop.EncStdParam.avcParam.avc_frameCropRight = 0;
            encop.EncStdParam.avcParam.avc_frameCropTop = 0;
            encop.EncStdParam.avcParam.avc_frameCropBottom = 0;
            if (enc->cmdl->rot_angle != 90 &&
                    enc->cmdl->rot_angle != 270 &&
                    enc->enc_picheight == 1080) {
                /*
                 * In case of AVC encoder, when we want to use
                 * unaligned display width frameCroppingFlag
                 * parameters should be adjusted to displayable
                 * rectangle
                 */
                encop.EncStdParam.avcParam.avc_frameCroppingFlag = 1;
                encop.EncStdParam.avcParam.avc_frameCropBottom = 8;
            }

        } else {
            encop.EncStdParam.avcParam.avc_fmoEnable = 0;
            encop.EncStdParam.avcParam.avc_fmoType = 0;
            encop.EncStdParam.avcParam.avc_fmoSliceNum = 1;
            encop.EncStdParam.avcParam.avc_fmoSliceSaveBufSize = 32; /* FMO_SLICE_SAVE_BUF_SIZE */
        }
    }

    ret = vpu_EncOpen(&handle, &encop);
    if (ret != RETCODE_SUCCESS) {
        if (enc->cmdl->format == STD_MJPG) {
            free(qMatTable);
            free(huffTable);
        }
        err_msg("Encoder open failed %d\n", ret);
        return -1;
    }

    enc->handle = handle;
    return 0;
}

int encoder_init(void)
{
    int ret;

    encoder_preinit();
    framebuf_init();

    ret = vpu_Init(NULL);
    if (ret) {
        err_msg("VPU Init Failure.\n");
        return -1;
    }
    vpu_versioninfo ver;
    ret = vpu_GetVersionInfo(&ver);
    if (ret) {
        err_msg("Cannot get version info, err:%d\n", ret);
        vpu_UnInit();
        return -1;
    }
    info_msg("VPU firmware version: %d.%d.%d_r%d\n", ver.fw_major, ver.fw_minor,
             ver.fw_release, ver.fw_code);
    info_msg("VPU library version: %d.%d.%d\n", ver.lib_major, ver.lib_minor,
             ver.lib_release);

    /* allocate memory for must remember stuff */
    enc = (struct encode *)calloc(1, sizeof(struct encode));
    if (enc == NULL) {
        err_msg("Failed to allocate encode structure\n");
        return -1;
    }

    /* get physical contigous bit stream buffer */
    mem_desc.size = STREAM_BUF_SIZE;
    ret = IOGetPhyMem(&mem_desc);
    if (ret) {
        err_msg("Unable to obtain physical memory\n");
        free(enc);
        return -1;
    }

    /* mmap that physical buffer */
    enc->virt_bsbuf_addr = IOGetVirtMem(&mem_desc);
    if (enc->virt_bsbuf_addr <= 0) {
        err_msg("Unable to map physical memory\n");
        IOFreePhyMem(&mem_desc);
        free(enc);
        return -1;
    }

    enc->phy_bsbuf_addr = mem_desc.phy_addr;
    enc->cmdl = cmdl;
    enc->mjpg_fmt = MODE420;

    if (enc->cmdl->mapType) {
        enc->linear2TiledEnable = 1;
        enc->cmdl->chromaInterleave = 1; /* Must be CbCrInterleave for tiled */
    } else
        enc->linear2TiledEnable = 0;

    /* open the encoder */
    ret = encoder_open(enc);
    if (ret)
        goto err;

    /* configure the encoder */
    ret = encoder_configure(enc);
    if (ret)
        goto err1;

    /* allocate scratch buf */
    if (cpu_is_mx6x() && (cmdl->format == STD_MPEG4) && enc->mp4_dataPartitionEnable) {
        scratch_mem_desc.size = MPEG4_SCRATCH_SIZE;
        ret = IOGetPhyMem(&scratch_mem_desc);
        if (ret) {
            err_msg("Unable to obtain physical slice save mem\n");
            goto err1;
        }
        enc->scratchBuf.bufferBase = scratch_mem_desc.phy_addr;
        enc->scratchBuf.bufferSize = scratch_mem_desc.size;
    }

    /* allocate memory for the frame buffers */
    ret = encoder_allocate_framebuffer(enc);
    if (ret)
        goto err1;

    return 0;

err1:
    /* close the encoder */
    encoder_close(enc);
err:
    if (cpu_is_mx6x() && cmdl->format == STD_MPEG4 && enc->mp4_dataPartitionEnable) {
        IOFreeVirtMem(&scratch_mem_desc);
        IOFreePhyMem(&scratch_mem_desc);
    }
    /* free the physical memory */
    IOFreeVirtMem(&mem_desc);
    IOFreePhyMem(&mem_desc);
    free(enc);
    vpu_UnInit();
    return ret;

    //////////////////////////////////////////////////////////////////////////////////

    return 0;
}

void encoder_uninit(void)
{
    /* free the allocated framebuffers */
    encoder_free_framebuffer(enc);

    /* close the encoder */
    encoder_close(enc);

    if (cpu_is_mx6x() && cmdl->format == STD_MPEG4 && enc->mp4_dataPartitionEnable) {
        IOFreeVirtMem(&scratch_mem_desc);
        IOFreePhyMem(&scratch_mem_desc);
    }
    /* free the physical memory */
    IOFreeVirtMem(&mem_desc);
    IOFreePhyMem(&mem_desc);
    free(enc);
    vpu_UnInit();
}

int encoder_run(void)
{
    return encoder_start(enc);
}

#else

int encoder_init(void)
{
    encoder_preinit();

    return -1;
}

void encoder_uninit(void)
{
}

int encoder_run(void)
{
    return -1;
}

#endif

void encoder_quit(void)
{
    quitflag = 1;
}

