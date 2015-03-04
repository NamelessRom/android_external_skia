/*
 * Copyright 2012, Samsung Electronics Co. LTD
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

#define LOG_TAG "SKIA"
#include <cutils/log.h>
#include <stdlib.h>
#include "SkFimgApi4x.h"

/* int comp_value[src][dst][scale][global_alpha][filter_mode][blending_mode][dithering]
   [src]
    0 : kRGB_565_Config
    1 : kARGB_4444_Config
    2 : kARGB_8888_Config
    3 : kNo_Config
    [dst]
    0 : kRGB_565_Config
    1 : kARGB_4444_Config
    2 : kARGB_8888_Config
    [scale]
    0 : No scaling
    1 : Scaling_up
    2 : Scaling_down
    [global_alpha]
    0 : per pixel alpha
    1 : global alpha
    [filter_mode]
    0 : nearest
    1 : bilinear
    [blending_mode]
    0 : SRC
    1 : SRC_OVER
    [dithering]
    0 : no dithering
    1 : dithering
*/
int comp_value[4][3][3][2][2][2][2] = {
{{{{{{  86*  48,  86*  48},{ 885* 498, 884* 497}},{{  86*  48,  86*  48},{ 885* 498, 886* 498}}},
   {{{  79*  44,  78*  44},{ 234* 131, 233* 131}},{{  78*  44,  78*  44},{ 237* 133, 234* 132}}}},
  {{{{  76*  43,  90*  50},{ 509* 286, 513* 289}},{{  75*  42,  74*  41},{ 112*  63, 112*  63}}},
   {{{  80*  45,  83*  46},{ 141*  79, 141*  79}},{{  68*  38,  67*  38},{  85*  48,  85*  48}}}},
  {{{{ 120*  68, 113*  63},{ 493* 277, 494* 278}},{{  76*  43,  76*  43},{ 131*  74, 131*  74}}},
   {{{ 101*  57, 103*  58},{ 239* 134, 236* 133}},{{  71*  40,  71*  40},{  98*  55,  98*  55}}}}},
 {{{{{  91*  51,  91*  51},{ 109*  61,  86*  48}},{{  89*  50,  90*  51},{ 109*  61,  86*  48}}},
   {{{  81*  46,  81*  46},{  80*  45,  66*  37}},{{  81*  45,  81*  45},{  80*  45,  66*  37}}}},
  {{{{  81*  45,  95*  53},{ 114*  64,  90*  51}},{{  78*  44,  77*  43},{  85*  48,  74*  42}}},
   {{{  84*  47,  86*  48},{  82*  46,  69*  39}},{{  71*  40,  69*  39},{  67*  38,   0*   0}}}},
  {{{{ 131*  74, 123*  69},{ 159*  89, 114*  64}},{{  78*  44,  79*  44},{  86*  48,  77*  43}}},
   {{{ 107*  60, 108*  61},{ 103*  58,  83*  47}},{{  71*  40,  72*  40},{  70*  40,   0*   0}}}}},
 {{{{{ 183* 103, 255* 143},{ 270* 152, 264* 148}},{{ 250* 140, 255* 143},{ 263* 148, 263* 148}}},
   {{{ 167*  94, 169*  95},{ 123*  69, 121*  68}},{{ 141*  80, 174*  98},{ 128*  72, 114*  64}}}},
  {{{{ 157*  88, 169*  95},{ 185* 104, 183* 103}},{{ 102*  57, 101*  57},{ 105*  59, 105*  59}}},
   {{{ 128*  72, 132*  74},{ 112*  63, 114*  64}},{{  88*  50,  87*  49},{  80*  45,  80*  45}}}},
  {{{{ 261* 147, 269* 151},{ 295* 166, 295* 166}},{{  98*  55,  98*  55},{ 102*  57, 102*  57}}},
   {{{ 197* 111, 197* 111},{ 152*  85, 148*  83}},{{  87*  49,  87*  49},{  82*  46,  82*  46}}}}}},
{{{{{{  98*  55, 100*  56},{ 145*  82, 143*  80}},{{  98*  55,  99*  56},{ 144*  81, 143*  80}}},
   {{{  86*  48,  87*  49},{ 110*  62, 110*  62}},{{  86*  48,  85*  48},{ 110*  62, 110*  62}}}},
  {{{{  87*  49, 102*  58},{ 171*  96, 156*  88}},{{  71*  40,  70*  39},{  83*  47,  81*  45}}},
   {{{  88*  50,  90*  51},{ 129*  73, 121*  68}},{{  68*  38,  67*  38},{  78*  44,  75*  42}}}},
  {{{{ 136*  76, 132*  75},{ 282* 158, 255* 144}},{{  73*  41,  72*  41},{  85*  48,  83*  47}}},
   {{{ 112*  63, 113*  63},{ 191* 108, 172*  97}},{{  70*  39,  69*  39},{  80*  45,  78*  44}}}}},
 {{{{{ 106*  60, 107*  60},{ 111*  63,  91*  51}},{{ 107*  60, 107*  60},{ 111*  63,  91*  51}}},
   {{{  92*  51,  92*  52},{  92*  52,  81*  46}},{{  91*  51,  90*  50},{  94*  53,  81*  46}}}},
  {{{{  94*  53, 109*  61},{ 111*  62,  91*  51}},{{  74*  42,  72*  41},{  72*  40,  65*  36}}},
   {{{  93*  52,  95*  53},{  95*  54,  83*  47}},{{  70*  39,  68*  38},{  68*  39,   0*   0}}}},
  {{{{ 150*  84, 141*  79},{ 149*  84, 119*  67}},{{  75*  42,  75*  42},{  76*  42,  70*  39}}},
   {{{ 121*  68, 122*  69},{ 124*  70, 104*  58}},{{  71*  40,  71*  40},{  71*  40,  67*  38}}}}},
 {{{{{ 426* 240, 429* 241},{ 203* 114, 211* 119}},{{ 420* 236, 426* 240},{ 205* 115, 207* 117}}},
   {{{ 191* 107, 185* 104},{ 135*  76, 133*  75}},{{ 187* 105, 192* 108},{ 132*  74, 135*  76}}}},
  {{{{ 296* 166, 291* 164},{ 179* 101, 180* 101}},{{  93*  52,  93*  52},{  83*  47,  84*  47}}},
   {{{ 169*  95, 176*  99},{ 135*  76, 135*  76}},{{  86*  48,  86*  48},{  79*  44,  78*  44}}}},
  {{{{ 344* 193, 344* 194},{ 265* 149, 261* 147}},{{  92*  52,  91*  51},{  86*  48,  86*  48}}},
   {{{ 259* 146, 258* 145},{ 185* 104, 183* 103}},{{  84*  47,  85*  48},{  80*  45,  80*  45}}}}}},
{{{{{{ 140*  79, 140*  79},{ 561* 316, 537* 302}},{{ 140*  79, 137*  77},{ 560* 315, 537* 302}}},
   {{{ 116*  65, 116*  65},{ 541* 304, 135*  76}},{{ 117*  66, 116*  65},{ 541* 304, 135*  76}}}},
  {{{{ 107*  60, 123*  69},{ 345* 194, 279* 157}},{{  81*  45,  79*  45},{ 100*  56,  97*  54}}},
   {{{ 104*  59, 107*  60},{ 195* 110, 175*  98}},{{  78*  44,  76*  43},{  93*  53,  91*  51}}}},
  {{{{ 249* 140, 249* 140},{ 325* 183, 315* 177}},{{  96*  54,  96*  54},{ 125*  70, 119*  67}}},
   {{{ 204* 115, 205* 115},{ 292* 165, 285* 160}},{{  93*  52,  93*  52},{ 118*  66, 113*  64}}}}},
 {{{{{ 167*  94, 161*  91},{ 166*  93, 118*  66}},{{ 161*  91, 163*  92},{ 167*  94, 118*  66}}},
   {{{ 125*  71, 125*  71},{ 132*  74, 101*  57}},{{ 126*  71, 127*  71},{ 131*  74, 102*  57}}}},
  {{{{ 116*  65, 135*  76},{ 136*  77, 105*  59}},{{  84*  47,  82*  46},{  82*  46,  73*  41}}},
   {{{ 112*  63, 113*  64},{ 116*  65,  96*  54}},{{  80*  45,  79*  44},{  78*  44,  70*  40}}}},
  {{{{ 259* 146, 258* 145},{ 263* 148, 220* 124}},{{ 100*  56, 100*  56},{ 102*  57,  91*  51}}},
   {{{ 241* 135, 241* 135},{ 244* 137, 162*  91}},{{  96*  54,  96*  54},{  97*  55,  88*  50}}}}},
 {{{{{ 644* 362, 645* 363},{ 473* 266, 473* 266}},{{ 645* 363, 645* 363},{ 473* 266, 472* 265}}},
   {{{ 409* 230, 409* 230},{ 264* 148, 262* 148}},{{ 408* 230, 406* 229},{ 262* 148, 263* 148}}}},
  {{{{ 503* 283, 499* 280},{ 305* 172, 308* 173}},{{ 119*  67, 118*  66},{ 102*  57, 101*  57}}},
   {{{ 357* 201, 354* 199},{ 202* 114, 200* 113}},{{ 108*  61, 107*  60},{  94*  53,  96*  54}}}},
  {{{{ 363* 204, 448* 252},{ 308* 173, 309* 174}},{{ 135*  76, 136*  77},{ 121*  68, 121*  68}}},
   {{{ 310* 175, 310* 174},{ 281* 158, 281* 158}},{{ 150*  84, 127*  71},{ 115*  65, 116*  66}}}}}},
{{{{{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}},
   {{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}}},
  {{{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}},
   {{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}}},
  {{{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}},
   {{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}}}},
 {{{{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}},
   {{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}}},
  {{{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}},
   {{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}}},
  {{{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}},
   {{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}}}},
 {{{{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}},
   {{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}}},
  {{{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}},
   {{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}}},
  {{{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}},
   {{{   0*   0,   0*   0},{   0*   0,   0*   0}},{{   0*   0,   0*   0},{   0*   0,   0*   0}}}}}}};

enum color_format formatSkiaToDriver[] = {
    SRC_DST_FORMAT_END, //!< bitmap has not been configured
    CF_MSK_1BIT,
    CF_MSK_8BIT,
    SRC_DST_FORMAT_END, //!< kIndex8_Config is not supported by FIMG2D
    CF_RGB_565,
    CF_ARGB_4444,
    CF_ARGB_8888,
    SRC_DST_FORMAT_END, //!< kRLE_Index8_Config is not supported by FIMG2D
};

enum blit_op blendingSkiaToDriver[] = {
    BLIT_OP_CLR,
    BLIT_OP_SRC,
    BLIT_OP_DST,
    BLIT_OP_SRC_OVER,
    BLIT_OP_DST_OVER,
    BLIT_OP_SRC_IN,
    BLIT_OP_DST_IN,
    BLIT_OP_SRC_OUT,
    BLIT_OP_DST_OUT,
    BLIT_OP_SRC_ATOP,
    BLIT_OP_DST_ATOP,
    BLIT_OP_XOR,

    BLIT_OP_ADD,
    BLIT_OP_MULTIPLY,
    BLIT_OP_SCREEN,
    BLIT_OP_NOT_SUPPORTED,

    BLIT_OP_DARKEN,
    BLIT_OP_LIGHTEN,

    BLIT_OP_NOT_SUPPORTED,
    BLIT_OP_NOT_SUPPORTED,
    BLIT_OP_NOT_SUPPORTED,
    BLIT_OP_NOT_SUPPORTED,
    BLIT_OP_NOT_SUPPORTED,
    BLIT_OP_NOT_SUPPORTED,
    BLIT_OP_NOT_SUPPORTED,

    BLIT_OP_NOT_SUPPORTED,
    BLIT_OP_NOT_SUPPORTED,
    BLIT_OP_NOT_SUPPORTED,
    BLIT_OP_NOT_SUPPORTED,
};

enum scaling filterSkiaToDriver[] = {
    SCALING_NEAREST,
    SCALING_BILINEAR,
};

bool FimgSupportNegativeCoordinate(Fimg *fimg)
{
    int width, height;
    bool result = false;

    if (fimg->dstBPP) {
        if (fimg->dstX < 0) {
            width = fimg->dstX + fimg->dstW;

            if ((fimg->dstX + fimg->dstW) >= (fimg->dstFWStride / fimg->dstBPP))
                width = fimg->dstFWStride / fimg->dstBPP;

            fimg->srcX = fimg->srcX + ((fimg->srcW * (-16384 * (fimg->dstX / fimg->dstW)) + 8192) * 16384);
            fimg->dstW = width;
            fimg->dstX = 0;
            fimg->srcW = ((fimg->srcW * (((width - fimg->dstX) / 16384) / fimg->dstW) + 8192) * 16384) - fimg->srcX;
        }

        if (fimg->dstY < 0) {
            height = fimg->dstY + fimg->dstH;

            if ( fimg->dstY + fimg->dstH >= fimg->dstFH)
                height = fimg->dstFH;

            fimg->srcY = fimg->srcY
                         + ((fimg->srcH * ((unsigned int)(-16384 * fimg->dstY) / fimg->dstH) + 8192) * 16384);
            fimg->dstH = height;
            fimg->dstY = 0;
            fimg->srcH = ((fimg->srcH * (((height - fimg->dstY) / 16384) / fimg->dstH) + 8192) * 16384) - fimg->srcY;
        }

        result = true;
    }

    return result;
}

bool FimgApiCheckPossible(Fimg *fimg)
{
    if (fimg->srcAddr != NULL) {
        switch (fimg->srcColorFormat) {
        case SkBitmap::kRGB_565_Config:
        case SkBitmap::kARGB_4444_Config:
        case SkBitmap::kARGB_8888_Config:
            break;
        default:
            return false;
        }
    }

    switch (fimg->dstColorFormat) {
    case SkBitmap::kRGB_565_Config:
    case SkBitmap::kARGB_4444_Config:
    case SkBitmap::kARGB_8888_Config:
        break;
    default:
        return false;
    }

    switch (fimg->xfermode) {
    case SkXfermode::kClear_Mode:
    case SkXfermode::kSrc_Mode:
    case SkXfermode::kDst_Mode:
    case SkXfermode::kSrcOver_Mode:
    case SkXfermode::kDstOver_Mode:
    case SkXfermode::kSrcIn_Mode:
    case SkXfermode::kDstIn_Mode:
    case SkXfermode::kSrcOut_Mode:
    case SkXfermode::kDstOut_Mode:
    case SkXfermode::kSrcATop_Mode:
    case SkXfermode::kDstATop_Mode:
    case SkXfermode::kXor_Mode:
    case SkXfermode::kPlus_Mode:
    case SkXfermode::kModulate_Mode:
    case SkXfermode::kScreen_Mode:
    //kOverlay_Mode
    case SkXfermode::kDarken_Mode:
    case SkXfermode::kLighten_Mode:
        break;
    default:
        return false;
    }

    if (fimg->colorFilter != 0)
        return false;

    if (fimg->matrixType & SkMatrix::kAffine_Mask)
        return false;

    if ((fimg->matrixSx < 0) || (fimg->matrixSy < 0))
        return false;

    if ((fimg->srcX + fimg->srcW) > 8000 || (fimg->srcY + fimg->srcH) > 8000)
        return false;

    if ((fimg->dstX + fimg->dstW) > 8000 || (fimg->dstY + fimg->dstH) > 8000)
        return false;

    if ((fimg->clipT < 0) || (fimg->clipB < 0) || (fimg->clipL < 0) || (fimg->clipR < 0)) {
        SkDebugf("Invalid clip value: TBLR = (%d, %d, %d, %d)",fimg->clipT, fimg->clipB, fimg->clipL, fimg->clipR);
        return false;
    }

    if ((fimg->clipT >= fimg->clipB) || (fimg->clipL >= fimg->clipR)) {
        SkDebugf("Invalid clip value: TBLR = (%d, %d, %d, %d)",fimg->clipT, fimg->clipB, fimg->clipL, fimg->clipR);
        return false;
    }

    return true;
}

bool FimgApiCheckPossiblePorterDuff(Fimg *fimg)
{
    if ((fimg->xfermode == SkXfermode::kMultiply_Mode
         || fimg->xfermode == SkXfermode::kScreen_Mode
         || fimg->xfermode == SkXfermode::kLighten_Mode
         || fimg->xfermode == SkXfermode::kDarken_Mode
         || fimg->xfermode == SkXfermode::kDstOver_Mode
         || fimg->xfermode == SkXfermode::kSrcOut_Mode
         || fimg->xfermode == SkXfermode::kDstATop_Mode
         || fimg->xfermode == SkXfermode::kXor_Mode)
        && fimg->alpha != 255)
        return false;

    return true;
/*    bool result;
    SkPorterDuff::Mode mode;

    if (SkPorterDuff::IsMode(fimg->xfermode, &mode) == true)
        result = true;
    else
        result = (fimg->alpha <= 255);

    return result;*/
}

bool FimgApiIsDstMode(Fimg *fimg)
{
    if (fimg->xfermode == SkXfermode::kDst_Mode)
        return true;
    else
        return false;
}

bool FimgApiCheckPossible_Clipping(Fimg *fimg)
{
    if (((fimg->clipR - fimg->clipL) <= 0) || ((fimg->clipB - fimg->clipT) <= 0))
        return false;

    return true;
}

bool FimgApiCompromise(Fimg *fimg)
{
    int src_fmt = 0;
    int dst_fmt = 0;
    int isScaling = 0;
    int isG_alpha = 0;
    int isFilter = 0;
    int isSrcOver = 0;
    int isDither = 0;
    int clipW = 0, clipH = 0;

    /* source format setting*/
    switch (fimg->srcColorFormat) {
        case SkBitmap::kRGB_565_Config:
            src_fmt = 0;
            break;
        case SkBitmap::kARGB_4444_Config:
            src_fmt = 1;
            break;
        case SkBitmap::kARGB_8888_Config:
            src_fmt = 2;
            break;
        case SkBitmap::kNo_Config:
            src_fmt = 3;
            break;
        default :
            break;
    }
    /* destination format setting */
    switch (fimg->dstColorFormat) {
        case SkBitmap::kRGB_565_Config:
            dst_fmt = 0;
            break;
        case SkBitmap::kARGB_4444_Config:
            dst_fmt = 1;
            break;
        case SkBitmap::kARGB_8888_Config:
            dst_fmt = 2;
            break;
        default :
            break;
    }
    /* scaling setting */
    if (fimg->srcW == fimg->dstW && fimg->srcH == fimg->dstH)
        isScaling = 0;
    else if (fimg->srcW * fimg->srcH < fimg->dstW * fimg->dstH)
        isScaling = 1;
    else
        isScaling = 2;
    /* global alpha or per pixel alpha */
    if (fimg->alpha != 255)
        isG_alpha = 1;
    /* filter_mode setting */
    isFilter = fimg->isFilter;
    /* blending mode setting */
    if (fimg->xfermode == SkXfermode::kSrc_Mode)
        isSrcOver = 0;
    else
        isSrcOver = 1;
    /* dither_mode setting */
    isDither = fimg->isDither;

    clipW = (fimg->clipR - fimg->clipL) * 1.2;
    clipH = (fimg->clipB - fimg->clipT) * 0.8;

    if ((clipW * clipH) < comp_value[src_fmt][dst_fmt][isScaling][isG_alpha][isFilter][isSrcOver][isDither])
        return false;

    return true;
}

int FimgApiStretch(Fimg *fimg, const char *func_name)
{
    struct fimg2d_blit cmd;
    struct fimg2d_image srcImage;
    struct fimg2d_image dstImage;

    ALOGE("%s\n", __FUNCTION__);

    /* to support negative Y coordinate */
    if ((fimg->dstAddr != NULL) && (fimg->dstY < 0)) {
        if (fimg->dstH > fimg->dstFH)
            fimg->dstFH = fimg->dstH;
        fimg->dstAddr += fimg->dstFWStride * fimg->dstY;
        fimg->clipT -= fimg->dstY;
        fimg->clipB -= fimg->dstY;
        fimg->dstY = 0;
    }

    if (FimgApiCheckPossible(fimg) == false)
        return false;

    if (FimgApiCheckPossiblePorterDuff(fimg) == false)
        return false;

    if (FimgApiIsDstMode(fimg) == true)
        return FIMGAPI_FINISHED;

    if (fimg->clipL < fimg->dstX)
        fimg->clipL = fimg->dstX;
    if (fimg->clipT < fimg->dstY)
        fimg->clipT = fimg->dstY;
    if (fimg->clipR > (fimg->dstX + fimg->dstW))
        fimg->clipR = fimg->dstX + fimg->dstW;
    if (fimg->clipB > (fimg->dstY + fimg->dstH))
        fimg->clipB = fimg->dstY + fimg->dstH;

#if FIMGAPI_COMPROMISE_USE
    if (FimgApiCompromise(fimg) == false)
        return false;
#endif
    enum rotation rotate;

    switch (fimg->rotate) {
    case 0:
        rotate = ORIGIN;
        break;
    case 90:
        rotate = ROT_90;
        break;
    case 180:
        rotate = ROT_180;
        break;
    case 270:
        rotate = ROT_270;
        break;
    default:
        return false;
    }

    cmd.op = blendingSkiaToDriver[fimg->xfermode];
    cmd.param.g_alpha = fimg->alpha;
    cmd.param.premult = PREMULTIPLIED;
    cmd.param.dither = fimg->isDither;
    cmd.param.rotate = rotate;
    cmd.param.solid_color = fimg->fillcolor;

    if (fimg->srcAddr != NULL && (fimg->srcW != fimg->dstW || fimg->srcH != fimg->dstH)) {
        cmd.param.scaling.mode = filterSkiaToDriver[fimg->isFilter];
        cmd.param.scaling.src_w = fimg->srcW;
        cmd.param.scaling.src_h = fimg->srcH;
        cmd.param.scaling.dst_w = fimg->dstW;
        cmd.param.scaling.dst_h = fimg->dstH;
    } else
        cmd.param.scaling.mode = NO_SCALING;

    cmd.param.repeat.mode = NO_REPEAT;
    cmd.param.repeat.pad_color = 0x0;

    cmd.param.bluscr.mode = OPAQUE;
    cmd.param.bluscr.bs_color = 0x0;
    cmd.param.bluscr.bg_color = 0x0;

    if (fimg->srcAddr != NULL) {
        srcImage.addr.type = ADDR_USER;
        srcImage.addr.start = (long unsigned)fimg->srcAddr;
        srcImage.need_cacheopr = true;
        srcImage.width = fimg->srcFWStride / fimg->srcBPP;
        srcImage.height = fimg->srcFH;
        srcImage.stride = fimg->srcFWStride;
        if (fimg->srcColorFormat == SkBitmap::kRGB_565_Config)
            srcImage.order = AX_RGB;
        else
            srcImage.order = AX_BGR; // kARGB_8888_Config

        srcImage.fmt = formatSkiaToDriver[fimg->srcColorFormat];
        srcImage.rect.x1 = fimg->srcX;
        srcImage.rect.y1 = fimg->srcY;
        srcImage.rect.x2 = fimg->srcX + fimg->srcW;
        srcImage.rect.y2 = fimg->srcY + fimg->srcH;
        cmd.src = &srcImage;
    } else
        cmd.src = NULL;

    if (fimg->dstAddr != NULL) {
        dstImage.addr.type = ADDR_USER;
        dstImage.addr.start = (long unsigned)fimg->dstAddr;
        dstImage.need_cacheopr = true;
        dstImage.width = fimg->dstFWStride / fimg->dstBPP;
        dstImage.height = fimg->dstFH;
        dstImage.stride = fimg->dstFWStride;
        if (fimg->dstColorFormat == SkBitmap::kRGB_565_Config)
            dstImage.order = AX_RGB;
        else
            dstImage.order = AX_BGR; // kARGB_8888_Config

        dstImage.fmt = formatSkiaToDriver[fimg->dstColorFormat];
        dstImage.rect.x1 = fimg->dstX;
        dstImage.rect.y1 = fimg->dstY;
        dstImage.rect.x2 = fimg->dstX + fimg->dstW;
        dstImage.rect.y2 = fimg->dstY + fimg->dstH;

        cmd.param.clipping.enable = true;
        cmd.param.clipping.x1 = fimg->clipL;
        cmd.param.clipping.y1 = fimg->clipT;
        cmd.param.clipping.x2 = fimg->clipR;
        cmd.param.clipping.y2 = fimg->clipB;

        cmd.dst = &dstImage;

    } else
        cmd.dst = NULL;

    cmd.msk = NULL;

    cmd.tmp = NULL;
    cmd.sync = BLIT_SYNC;
    cmd.seq_no = SEQ_NO_BLT_SKIA;

    if (FimgApiCheckPossible_Clipping(fimg) == false)
        return false;

#if defined(FIMGAPI_DEBUG_MESSAGE)
    printDataBlit("Before stretchFimgApi:", &cmd);
    printDataMatrix(fimg->matrixType);
#endif

    if (stretchFimgApi(&cmd) < 0) {
        ALOGE("%s:stretch failed\n", __FUNCTION__);
        return false;
    }

    return FIMGAPI_FINISHED;
}

int FimgARGB32_Rect(struct Fimg &fimg, uint32_t *device, int x, int y, int width, int height,
                    size_t rowbyte, uint32_t color)
{
    ALOGE("%s\n", __FUNCTION__);

    fimg.srcAddr        = (unsigned char *)NULL;
    fimg.srcColorFormat = SkBitmap::kNo_Config;
    fimg.mskAddr        = (unsigned char *)NULL;

    fimg.fillcolor      = toARGB32(color);
    fimg.srcColorFormat = SkBitmap::kARGB_8888_Config;

    fimg.dstX           = x;
    fimg.dstY           = y;
    fimg.dstW           = width;
    fimg.dstH           = height;
    fimg.dstFWStride    = rowbyte;
    fimg.dstFH          = y + height;
    fimg.dstBPP         = 4; /* 4Byte */
    fimg.dstColorFormat = SkBitmap::kARGB_8888_Config;
    fimg.dstAddr        = (unsigned char *)device;

    fimg.clipT          = y;
    fimg.clipB          = y + height;
    fimg.clipL          = x;
    fimg.clipR          = x + width;

    fimg.rotate         = 0;

    fimg.xfermode       = SkXfermode::kSrcOver_Mode;
    fimg.isDither       = false;
    fimg.colorFilter    = 0;
    fimg.matrixType     = 0;
    fimg.matrixSx       = 0;
    fimg.matrixSy       = 0;
    fimg.alpha          = 0xFF;

    return FimgApiStretch(&fimg, __func__);
}

int FimgRGB16_Rect(struct Fimg &fimg, uint16_t *device, int x, int y, int width, int height,
                    size_t rowbyte, uint32_t color)
{
    ALOGE("%s\n", __FUNCTION__);

    //memset((void *) fimg, 0, sizeof(struct Fimg));

    fimg.srcAddr        = (unsigned char *)NULL;
    fimg.srcColorFormat = SkBitmap::kNo_Config;
    fimg.mskAddr        = (unsigned char *)NULL;

    fimg.fillcolor      = toARGB32(color); //
    fimg.srcColorFormat = SkBitmap::kARGB_4444_Config;

    fimg.dstX           = x;//
    fimg.dstY           = y; //
    fimg.dstW           = width; //
    fimg.dstH           = height;//
    fimg.dstFWStride    = rowbyte;//
    fimg.dstFH          = y + height; //
    fimg.dstBPP         = 2; /* 2Byte */  //
    fimg.dstColorFormat = SkBitmap::kARGB_4444_Config; //
    fimg.dstAddr        = (unsigned char *)device; //

    fimg.clipT          = y; //
    fimg.clipB          = y + height; //
    fimg.clipL          = x;//
    fimg.clipR          = x + width; //

    fimg.rotate         = 0;

    fimg.xfermode       = SkXfermode::kSrcOver_Mode; //
    fimg.isDither       = false;
    fimg.colorFilter    = 0;
    fimg.matrixType     = 0;
    fimg.matrixSx       = 0;
    fimg.matrixSy       = 0;
    fimg.alpha          = 0xFF; //

    return FimgApiStretch(&fimg, __func__);
}

uint32_t toARGB32(uint32_t color)
{
    U8CPU a = SkGetPackedA32(color);
    U8CPU r = SkGetPackedR32(color);
    U8CPU g = SkGetPackedG32(color);
    U8CPU b = SkGetPackedB32(color);

    return (a << 24) | (r << 16) | (g << 8) | (b << 0);
}
