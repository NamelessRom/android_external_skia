/*
 * Copyright 2011, Samsung Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "SkFimpV2x.h"
#include "csc_swap_neon.h"
#include "SkScaledBitmapSampler.h"

ion_client SecJPEGCodec::m_ion_client = 0;

bool get_src_config(const jpeg_decompress_struct& cinfo,
                           SkScaledBitmapSampler::SrcConfig* sc,
                           int* srcBytesPerPixel);

int SecJPEGCodec::m_setCache()
{
    int ret = 0;

    if (m_jpeg_fd < 0) {
        ALOGE("[%s] JPEG open failed:%d",__func__, m_jpeg_fd);
        return -1;
    }

    ret = jpeghal_s_ctrl(m_jpeg_fd, V4L2_CID_CACHEABLE, 1);

    return ret;
}

int SecJPEGCodec::m_createDecoder()
{
    int ret = 0;

    m_jpeg_mode = JPEG_DECODE;

    m_jpeg_fd = jpeghal_dec_init();
    if (m_jpeg_fd <= 0)
        ret = -1;

    ret = m_setCache();

    return ret;
}

int SecJPEGCodec::m_destroyDecoder()
{
    int ret = 0;

    if (m_jpeg_fd >= 0)
        ret = jpeghal_deinit(m_jpeg_fd, &m_in_buf, &m_out_buf);

    m_jpeg_fd = -1;
    return ret;
}

int SecJPEGCodec::m_createEncoder()
{
    int ret = 0;

    m_jpeg_mode = JPEG_ENCODE;

    m_jpeg_fd = jpeghal_enc_init();
    if (m_jpeg_fd <= 0)
        ret = -1;

    m_setCache();

    return ret;
}

int SecJPEGCodec::m_destroyEncoder()
{
    int ret = 0;

    if (m_jpeg_fd < 0)
        close(m_jpeg_fd);
    else
        ret = jpeghal_deinit(m_jpeg_fd, &m_in_buf, &m_out_buf);

    return ret;
}

#ifdef SEC_HWJPEG_G2D
int SecJPEGCodec::executeG2D(SkBitmap *bm, int in_width, int in_height, void *srcaddr, void *dstaddr)
{
    int ret = 0;

    m_fimg.srcX = 0;
    m_fimg.srcY = 0;
    m_fimg.srcW = in_width;
    m_fimg.srcH = in_height;
    m_fimg.srcFWStride = in_height * 4;
    m_fimg.srcFH = in_height;
    m_fimg.srcBPP = 4;
    m_fimg.srcColorFormat = bm->getConfig();
    //m_fimg.srcOrder = 2; // AX_BGR
    m_fimg.srcAddr = (unsigned char *)srcaddr;

    //m_fimg.isSolidFill = false;

    m_fimg.dstX = 0;
    m_fimg.dstY = 0;
    m_fimg.dstW = bm->width();
    m_fimg.dstH = bm->height();
    m_fimg.dstFWStride = bm->rowBytes();
    m_fimg.dstFH = bm->height();
    m_fimg.dstBPP = bm->bytesPerPixel();
    m_fimg.dstColorFormat = bm->getConfig();
    //m_fimg.dstOrder = 0; // AX_RGB
    m_fimg.dstAddr = (unsigned char *)dstaddr;

    m_fimg.clipT = 0;
    m_fimg.clipB = m_fimg.dstW;
    m_fimg.clipL = 0;
    m_fimg.clipR = m_fimg.dstH;

    m_fimg.mskAddr = NULL;

    m_fimg.fillcolor = 0;
    m_fimg.rotate = 0;
    m_fimg.alpha = 255;
    m_fimg.xfermode = SkXfermode::kSrc_Mode;
    m_fimg.isDither = false;
    m_fimg.isFilter = 0;
    //m_fimg.colorfilter = 0;
    //m_fimg.canusehybrid = 0;
    m_fimg.matrixType = 0;
    m_fimg.matrixSx = 0;
    m_fimg.matrixSy = 0;

    FimgApiStretch(&m_fimg, __func__);

    return ret;
}
#endif

SecJPEGCodec::SecJPEGCodec() :
    m_jpeg_fd       (-1),
    m_numplanes     (1),
    m_hwjpeg_flag   (false),
    m_in_ion_fd     (-1),
    m_out_ion_fd    (-1)
{
    memset(&m_in_buf, 0, sizeof(struct jpeg_buf));
    memset(&m_out_buf, 0, sizeof(struct jpeg_buf));
    memset(&m_jpeg_cfg, 0, sizeof(struct jpeg_config));
#ifdef SEC_HWJPEG_G2D
    memset(&m_fimg, 0, sizeof(struct Fimg));
#endif

    if (m_ion_client <= 0)
        m_ion_client = ion_client_create();
}

SecJPEGCodec::~SecJPEGCodec()
{
    if (m_jpeg_mode == JPEG_DECODE)
        m_destroyDecoder();
    else if (m_jpeg_mode == JPEG_ENCODE)
        m_destroyEncoder();

	/*if (m_in_buf.start[0] != NULL) {
		ALOGE("%s ion_unmap(0x%x, %d)", __func__, m_in_buf.start[0], m_inbuf_size);
		if (ion_unmap(m_in_buf.start[0], m_inbuf_size) < 0)
			ALOGE("%s Could not ion_unmap m_in_ion_fd \"%s\"", __func__, strerror(errno));

		m_in_buf.start[0] = NULL;
	}

    if (m_in_ion_fd >= 0) {
        ALOGE("%s ion_free(%d)", __func__, m_in_ion_fd);
        ion_free(m_in_ion_fd);
        m_in_ion_fd = -1;
    }

	if (m_out_buf.start[0] != NULL) {
		ALOGE("%s ion_unmap(0x%x, %d)", __func__, m_out_buf.start[0], m_outbuf_size);
		if (ion_unmap(m_out_buf.start[0], m_outbuf_size) < 0)
			ALOGE("Could not ion_unmap m_out_ion_fd \"%s\"", strerror(errno));

		m_out_buf.start[0] = NULL;
	}

	if (m_out_ion_fd >= 0) {
	    ALOGE("%s ion_free(%d)", __func__, m_out_ion_fd);
	    ion_free(m_out_ion_fd);
	    m_out_ion_fd = -1;
    }*/

    /*if (m_in_buf.start[0])
    	free(m_in_buf.start[0]);

    if (m_out_buf.start[0])
    	free(m_out_buf.start[0]);*/

    if (m_ion_client > 0) {
        ion_client_destroy(m_ion_client);
        m_ion_client = -1;
    }
}

int SecJPEGCodec::checkHwJPEGSupport(SkBitmap::Config config, jpeg_decompress_struct *cinfo, int sampleSize)
{
    int ret = 0;
    int scalingratio = 1;

    ALOGE("%s sampleSize=%d w=%d h=%d", __func__, sampleSize, cinfo->image_width, cinfo->image_height);
    if (sampleSize == 1) {
        if ((cinfo->image_width % 2 == 0) && (cinfo->image_height % 2 == 0)) {
            m_hwjpeg_flag = true;
        } else {
            m_hwjpeg_flag = false;
        }
    } else if (sampleSize >= 4) {
        if (((cinfo->image_width / 4) % 2 == 0) && ((cinfo->image_height / 4) % 2 == 0)) {
            m_hwjpeg_flag = true;
            scalingratio = 4;
        } else {
            m_hwjpeg_flag = false;
        }
    } else if (sampleSize >= 2) {
        if (((cinfo->image_width / 2) % 2 == 0) && ((cinfo->image_height / 2) % 2 == 0)) {
            m_hwjpeg_flag = true;
            scalingratio = 2;
        } else {
            m_hwjpeg_flag = false;
        }
    } else {
        m_hwjpeg_flag = false;
        ALOGE("[SKIA JPEG] HW JPEG not supported scaling");
    }

    if ((cinfo->image_width <= JPEG_HW_MIN_WIDTH) || (cinfo->image_height <= JPEG_HW_MIN_HEIGHT)) {
    	m_hwjpeg_flag = false;
    }

    ALOGE("%s progr=%d config=%d", __func__, cinfo->progressive_mode, config);
    if ((m_hwjpeg_flag == true) &&
        (cinfo->progressive_mode == false) &&
        ((config == SkBitmap::kARGB_8888_Config) ||
        (config == SkBitmap::kRGB_565_Config))) {
        m_hwjpeg_flag = true;
        cinfo->output_width = cinfo->image_width / scalingratio;
        cinfo->output_height = cinfo->image_height / scalingratio;
    } else {
        m_hwjpeg_flag = false;
    }

    return m_hwjpeg_flag;
}

int SecJPEGCodec::setDecodeHwJPEGConfig(SkBitmap::Config config, jpeg_decompress_struct *cinfo, SkStream *stream)
{
    int out_fmt = V4L2_PIX_FMT_RGB32;
    int jpegsize;
    int ret = 0;

    if (!stream)
        return -1;

    ret = m_createDecoder();
    if (ret < 0) {
        ALOGE("[%s]JPEG Decode Init failed", __func__);
        return -1;
    }

    // check for supported formats
    SkScaledBitmapSampler::SrcConfig sc;

    if (!get_src_config(*cinfo, &sc, &m_in_bpp)) {
        return -1;
    }

    switch (config) {
        case SkBitmap::kARGB_8888_Config:
            out_fmt = V4L2_PIX_FMT_RGB32;
            m_out_bpp = 4;
            ALOGD("[%s]JPEG Decode format is ARGB_8888", __func__);
            break;
        case SkBitmap::kRGB_565_Config:
            out_fmt = V4L2_PIX_FMT_RGB565X;
            m_out_bpp = 2;
            ALOGD("[%s]JPEG Decode format is RGB565", __func__);
            break;
        default:
            return -1;
            ALOGE("[%s]JPEG Decode Skia config failed", __func__);
    }

    //m_inbuf_size = (cinfo->image_width * cinfo->image_height * m_pixel_size) + JPEG_WA_BUFFER_SIZE;
    m_inbuf_size = (stream->getLength() * m_in_bpp) + JPEG_WA_BUFFER_SIZE;
    if (m_inbuf_size <= 0) {
        return -1;
    }

    ALOGE("%s stream->size=%d    m_inbuf_size=%d", __func__, stream->getLength(), m_inbuf_size);

    m_outbuf_size = (cinfo->output_width * cinfo->output_height * m_out_bpp);
    if (m_outbuf_size <= 0) {
        return -1;
    }

    jpegsize = stream->getLength();
    if (jpegsize <= 0) {
        ALOGE("[%s] in_size is too small", __func__);
        return -1;
    }

    m_jpeg_cfg.mode = m_jpeg_mode;
    m_jpeg_cfg.width = cinfo->image_width;
    m_jpeg_cfg.height = cinfo->image_height;
    m_jpeg_cfg.scaled_width = cinfo->output_width;
    m_jpeg_cfg.scaled_height = cinfo->output_height;
    m_jpeg_cfg.sizeJpeg = jpegsize;
    m_jpeg_cfg.num_planes = 1;
    m_jpeg_cfg.pix.dec_fmt.in_fmt = V4L2_PIX_FMT_JPEG_422;
    m_jpeg_cfg.pix.dec_fmt.out_fmt = out_fmt;

    ret = jpeghal_dec_setconfig(m_jpeg_fd, &m_jpeg_cfg);

    return ret;
}

int SecJPEGCodec::setEncodeHwJPEGConfig(int in_fmt, int out_fmt, int width, int height, int qual)
{
    int ret = 0;

    m_createEncoder();

    m_jpeg_cfg.mode = m_jpeg_mode;
    m_jpeg_cfg.enc_qual = (enum jpeg_quality_level)qual;
    m_jpeg_cfg.width = width;
    m_jpeg_cfg.height = height;
    m_jpeg_cfg.num_planes = 1;
    m_jpeg_cfg.pix.dec_fmt.in_fmt = in_fmt;
    m_jpeg_cfg.pix.dec_fmt.out_fmt = out_fmt;

    ret = jpeghal_enc_setconfig(m_jpeg_fd, &m_jpeg_cfg);

    return ret;
}

int SecJPEGCodec::setHwJPEGInputBuffer(SkStream *stream)
{
    const void *stream_data;
    const void *eoi_addr;
    int eoi_marker;
    int ret = 0;
    ion_phys_addr_t ion_paddr = 0;

    if (!stream)
        return -1;

    m_in_buf.memory = V4L2_MEMORY_MMAP;
    m_in_buf.num_planes = 1;
    /*ret = jpeghal_set_inbuf(m_jpeg_fd, &m_in_buf);
    if (ret < 0) {
        ALOGD("[SKIA JPEG] jpeghal_set_inbuf() with MMAP failed, switch to ION");*/

        m_in_buf.memory = V4L2_MEMORY_USERPTR;
        m_in_buf.length[0] = m_inbuf_size;

/*        ALOGE("%s ion_alloc(%d, %d, 0, ION_HEAP_EXYNOS_CONTIG_MASK)",__func__, m_ion_client, m_inbuf_size);
        m_in_ion_fd = ion_alloc(m_ion_client, m_inbuf_size, 0, ION_HEAP_EXYNOS_CONTIG_MASK);
        if (m_in_ion_fd < 0) {
            ALOGE("%s: Unable to alloc input ION memory, size=%d", __func__, m_inbuf_size);
            return -1;
        }

        ion_paddr = ion_getphys(m_ion_client, m_in_ion_fd);
        if (ion_paddr == 0) {
            ALOGE("%s ion_getphys(%d,%d) failed", __func__, m_ion_client, m_in_ion_fd);
            return -1;
        }

        m_in_buf.start[0] = ion_map(m_in_ion_fd, m_inbuf_size, 0);
        if (m_in_buf.start[0] == (void*)-1) {
            ALOGE("%s ion_map fail", __func__);
            return -1;
        }*/

        //m_in_buf.start[0] = (void *) malloc(m_inbuf_size);

        SkAutoMalloc storage(m_inbuf_size);
        m_in_buf.start[0] = (void *)storage.get();

        if (stream->getPosition() != 0)
            stream->rewind();

        //m_in_buf.start[0] = (void *)stream->getMemoryBase();
        ALOGE("%s m_in_buf.start[0]=0x%x", __func__, m_in_buf.start[0]);

        ret = jpeghal_set_inbuf(m_jpeg_fd, &m_in_buf);
        if (ret < 0) {
        	ALOGD("%s: [SKIA JPEG] unable to set input buffer", __func__);
        	return -1;
        }
    //}

    stream_data = stream->getMemoryBase();

    if (stream_data == NULL) {
    	ALOGD("%s stream_data is NULL", __func__);
        if (!stream->rewind()) {
            ALOGD("[SKIA JPEG] rewind() is failed");
            m_hwjpeg_flag = false;
            return -1;
        }
        size_t bytes = stream->read(m_in_buf.start[0], m_in_buf.length[0]);
        /*eoi_addr = m_in_buf.start[0] + m_in_buf.length[0] - 4;
        eoi_marker = (int)(*(int *)eoi_addr);
        if ((eoi_marker & 0xffff0000) != (int)0xd9ff0000) {
            ALOGD("[SKIA JPEG] EOI is missed");
            return -1;
        }*/
    } else {
        memcpy(m_in_buf.start[0], stream_data, m_in_buf.length[0]);
        ret = 0;
    }

    return ret;
}

const void *SecJPEGCodec::setHwJPEGOutputBuffer(void)
{
    int ret = 0;
    ion_phys_addr_t ion_paddr = 0;

    m_out_buf.memory = V4L2_MEMORY_MMAP;
    m_out_buf.num_planes = 1;
    /*ret = jpeghal_set_outbuf(m_jpeg_fd, &m_out_buf);
    if (ret < 0) {
	   ALOGD("[SKIA JPEG] jpeghal_set_outbuf() with MMAP failed, switch to ION");*/

	   m_out_buf.memory = V4L2_MEMORY_USERPTR;
	   m_out_buf.length[0] = m_outbuf_size;

	   /*m_out_ion_fd = ion_alloc(m_ion_client, m_outbuf_size, 0, ION_HEAP_EXYNOS_CONTIG_MASK);
	   if (m_out_ion_fd < 0) {
		   ALOGE("%s: Unable to alloc input ION memory", __func__);
		   return NULL;
	   }

	   ion_paddr = ion_getphys(m_ion_client, m_out_ion_fd);
	   if (ion_paddr == 0) {
		   ALOGE("%s ion_getphys failed", __func__);
		   return NULL;
	   }

	   m_out_buf.start[0] = ion_map(m_out_ion_fd, m_out_buf.length[0], 0);
	   if (m_out_buf.start[0] == (void*)-1) {
		   ALOGE("%s ion_map fail", __func__);
		   return NULL;
	   }*/

	   //m_out_buf.start[0] = (void *) malloc(m_outbuf_size);

	   SkAutoMalloc storage(m_outbuf_size);
	   m_out_buf.start[0] = (void *)storage.get();

	   ALOGE("%s m_out_buf.start[0]=0x%x", __func__, m_out_buf.start[0]);

	   ret = jpeghal_set_outbuf(m_jpeg_fd, &m_out_buf);
	   if (ret < 0) {
	   	   ALOGD("%s: [SKIA JPEG] unable to set output buffer", __func__);
	   	   return NULL;
	   }
    //}

    return (const void *)(m_out_buf.start[0]);
}

int SecJPEGCodec::executeDecodeHwJPEG(void)
{
    int ret = 0;

    /*if (ion_msync(m_ion_client, m_in_ion_fd, IMSYNC_SYNC_FOR_DEV | IMSYNC_DEV_TO_RW, m_inbuf_size, 0) < 0) {
        ALOGE("%s: Unable to sync ION memory for m_in_ion_fd", __func__);
        return -1;
    }*/

    ret = jpeghal_dec_exe(m_jpeg_fd, &m_in_buf, &m_out_buf);
    ALOGE("%s jpeghal_dec_exe executed, ret=%d", __func__, ret);

    /*if (ion_msync(m_ion_client, m_out_ion_fd, IMSYNC_SYNC_FOR_DEV | IMSYNC_DEV_TO_RW, m_outbuf_size, 0) < 0) {
        ALOGE("%s: Unable to sync ION memory for m_out_ion_fd", __func__);
        return -1;
    }*/

    return ret;
}

int SecJPEGCodec::executeEncodeHwJPEG(void)
{
    int ret = 0;

    ret = jpeghal_enc_exe(m_jpeg_fd, &m_in_buf, &m_out_buf);

    return ret;
}

void SecJPEGCodec::outputJpeg(SkBitmap *bm, unsigned char *rowptr)
{
    void *jpeg_out;

    jpeg_out = (void *)m_out_buf.start[0];

#ifdef SEC_HWJPEG_G2D
    executeG2D(bm, m_jpeg_cfg.width, m_jpeg_cfg.height, jpeg_out, (void *) rowptr);
#else
    csc_swap_1st_3rd_byte_mask_neon((unsigned int *)rowptr, (unsigned int *)jpeg_out, m_out_buf.length[0]/4, 0xFF);
#endif
    m_hwjpeg_flag = false;
}
