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

#ifndef _SKFIMPV2X_H_
#define _SKFIMPV2X_H_

#define SEC_SKIAHWJPEG

#include "SkImageDecoder.h"
#include "SkImageEncoder.h"
#include "SkJpegUtility.h"
#include "SkStream.h"
#include "SkTemplates.h"
#include "SkUtils.h"
//#include "SkXfermode.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
extern "C" {
    #include "jpeglib.h"
    #include "jerror.h"
#ifdef SEC_SKIAHWJPEG
    #include <jpeg_hal.h>
    #include <cutils/log.h>
#include "videodev2_exynos_media.h"
#include "secion.h"
#endif
}

#ifdef SEC_HWJPEG_G2D
#if defined(SAMSUNG_EXYNOS4210)
#include "SkFimgApi3x.h"
#endif
#if defined(SAMSUNG_EXYNOS4x12)
#include "SkFimgApi4x.h"
#endif
#endif

#define JPEG_HW_MIN_WIDTH 256
#define JPEG_HW_MIN_HEIGHT 256
#define JPEG_WA_BUFFER_SIZE 64

class SecJPEGCodec {
public:
    int                 checkHwJPEGSupport(SkBitmap::Config config, jpeg_decompress_struct *cinfo, int sampleSize);
    int                 isHwJPEG() { return m_hwjpeg_flag; }
    void                setHwJPEGFlag(int flag) { m_hwjpeg_flag= flag; }

    int                 setDecodeHwJPEGConfig(SkBitmap::Config config, jpeg_decompress_struct *cinfo, SkStream *stream);
    int                 setEncodeHwJPEGConfig(int in_fmt, int out_fmt, int width, int height, int qual);

    int                 setHwJPEGInputBuffer(SkStream *stream);
    const void          *setHwJPEGOutputBuffer(void);

    int                 executeDecodeHwJPEG(void);
    int                 executeEncodeHwJPEG(void);
    void                outputJpeg(SkBitmap *bm, unsigned char *rowptr);
private:
    int                 m_jpeg_fd;
    int                 m_numplanes;

    enum jpeg_mode      m_jpeg_mode;
    struct jpeg_config  m_jpeg_cfg;
    struct jpeg_buf     m_in_buf;
    struct jpeg_buf     m_out_buf;
#ifdef SEC_HWJPEG_G2D
    struct Fimg         m_fimg;
#endif
    int                 m_hwjpeg_flag;
    int                 m_in_bpp;
    int                 m_inbuf_size;
    int                 m_out_bpp;
    int                 m_outbuf_size;
    static ion_client   m_ion_client;
    ion_buffer          m_in_ion_fd;
    ion_buffer          m_out_ion_fd;
public:
    SecJPEGCodec();
    ~SecJPEGCodec();
private:
    int                 m_setCache(void);

    int                 m_createDecoder(void);
    int                 m_destroyDecoder(void);

    int                 m_createEncoder(void);
    int                 m_destroyEncoder(void);
#ifdef SEC_HWJPEG_G2D
    int                 executeG2D(SkBitmap *bm, int widht, int height, void *srcaddr, void *dstaddr);
#endif
};
#endif
