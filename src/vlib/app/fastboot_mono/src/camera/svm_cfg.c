/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : svm_cfg.c
 * Version      : 1.0
 * Description  : Configuration of fastboot_mono sub app.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#if FASTBOOT_CFG_APP_CAMERA_ENABLE

#include "fastboot_cfg.h"

#if defined(R_TARGET_BOARD_ERGUOTOU)

#include "surround_view.h"
#include "incbin.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define SVC_CFG_CAM_WIDTH            (1280)
#define SVC_CFG_CAM_HEIGHT           (800)
#define SVC_CFG_MAIN_PERSPECTIVE_NUM (6)

#define SVC_CFG_BEV_TOP_BOTTOM_HEIGHT (400)
#define SVC_CFG_BEV_LEFT_RIGHT_WIDTH  (232)

/* general camera intrinsics and distortion coeffience */
/* NOTE: calibrated by python-opencv with 5 sample images */
#define SVC_CFG_CAM_INTRINSICS_Fx    (337.24651321359676f)
#define SVC_CFG_CAM_INTRINSICS_Fy    (337.50701238246523f)
#define SVC_CFG_CAM_INTRINSICS_Cx    (638.0034124851347f)
#define SVC_CFG_CAM_INTRINSICS_Cy    (395.14404079263556f)
#define SVC_CFG_CAM_DISTORT_COEFF_K1 (0.0030772936704348028f)
#define SVC_CFG_CAM_DISTORT_COEFF_K2 (0.04502779812757056f)
#define SVC_CFG_CAM_DISTORT_COEFF_K3 (-0.006748870604804549f)
#define SVC_CFG_CAM_DISTORT_COEFF_K4 (0.0035839739191141553)

/* front camera intrinsics and distortion coeffience */
#define SVC_CFG_CAM_F_INTRI_Fx      SVC_CFG_CAM_INTRINSICS_Fx
#define SVC_CFG_CAM_F_INTRI_Fy      SVC_CFG_CAM_INTRINSICS_Fy
#define SVC_CFG_CAM_F_INTRI_Cx      SVC_CFG_CAM_INTRINSICS_Cx
#define SVC_CFG_CAM_F_INTRI_Cy      SVC_CFG_CAM_INTRINSICS_Cy
#define SVC_CFG_CAM_F_DIST_COEFF_K1 SVC_CFG_CAM_DISTORT_COEFF_K1
#define SVC_CFG_CAM_F_DIST_COEFF_K2 SVC_CFG_CAM_DISTORT_COEFF_K2
#define SVC_CFG_CAM_F_DIST_COEFF_K3 SVC_CFG_CAM_DISTORT_COEFF_K3
#define SVC_CFG_CAM_F_DIST_COEFF_K4 SVC_CFG_CAM_DISTORT_COEFF_K4

/* back camera intrinsics and distortion coeffience */
#define SVC_CFG_CAM_B_INTRI_Fx      SVC_CFG_CAM_INTRINSICS_Fx
#define SVC_CFG_CAM_B_INTRI_Fy      SVC_CFG_CAM_INTRINSICS_Fy
#define SVC_CFG_CAM_B_INTRI_Cx      SVC_CFG_CAM_INTRINSICS_Cx
#define SVC_CFG_CAM_B_INTRI_Cy      SVC_CFG_CAM_INTRINSICS_Cy
#define SVC_CFG_CAM_B_DIST_COEFF_K1 SVC_CFG_CAM_DISTORT_COEFF_K1
#define SVC_CFG_CAM_B_DIST_COEFF_K2 SVC_CFG_CAM_DISTORT_COEFF_K2
#define SVC_CFG_CAM_B_DIST_COEFF_K3 SVC_CFG_CAM_DISTORT_COEFF_K3
#define SVC_CFG_CAM_B_DIST_COEFF_K4 SVC_CFG_CAM_DISTORT_COEFF_K4

/* left camera intrinsics and distortion coeffience */
#define SVC_CFG_CAM_L_INTRI_Fx      SVC_CFG_CAM_INTRINSICS_Fx
#define SVC_CFG_CAM_L_INTRI_Fy      SVC_CFG_CAM_INTRINSICS_Fy
#define SVC_CFG_CAM_L_INTRI_Cx      SVC_CFG_CAM_INTRINSICS_Cx
#define SVC_CFG_CAM_L_INTRI_Cy      SVC_CFG_CAM_INTRINSICS_Cy
#define SVC_CFG_CAM_L_DIST_COEFF_K1 SVC_CFG_CAM_DISTORT_COEFF_K1
#define SVC_CFG_CAM_L_DIST_COEFF_K2 SVC_CFG_CAM_DISTORT_COEFF_K2
#define SVC_CFG_CAM_L_DIST_COEFF_K3 SVC_CFG_CAM_DISTORT_COEFF_K3
#define SVC_CFG_CAM_L_DIST_COEFF_K4 SVC_CFG_CAM_DISTORT_COEFF_K4

/* right camera intrinsics and distortion coeffience */
#define SVC_CFG_CAM_R_INTRI_Fx      SVC_CFG_CAM_INTRINSICS_Fx
#define SVC_CFG_CAM_R_INTRI_Fy      SVC_CFG_CAM_INTRINSICS_Fy
#define SVC_CFG_CAM_R_INTRI_Cx      SVC_CFG_CAM_INTRINSICS_Cx
#define SVC_CFG_CAM_R_INTRI_Cy      SVC_CFG_CAM_INTRINSICS_Cy
#define SVC_CFG_CAM_R_DIST_COEFF_K1 SVC_CFG_CAM_DISTORT_COEFF_K1
#define SVC_CFG_CAM_R_DIST_COEFF_K2 SVC_CFG_CAM_DISTORT_COEFF_K2
#define SVC_CFG_CAM_R_DIST_COEFF_K3 SVC_CFG_CAM_DISTORT_COEFF_K3
#define SVC_CFG_CAM_R_DIST_COEFF_K4 SVC_CFG_CAM_DISTORT_COEFF_K4

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
/* clang-format off */

/*
 * ---- Model car for top view ----
 * INCBIN generates the following symbols:
 * const unsigned char gModelCarData[], gModelCarAlphaData[];
 * const unsigned char *const gModelCarEnd, gModelCarAlphaEnd;
 * const unsigned int gModelCarSize, gModelCarAlphaSize;
 */
INCBIN(ModelCar, "..//..//fastboot_mono//src//camera//bev_car_224x380_uyvy.raw");
INCBIN(ModelCarAlpha, "..//..//fastboot_mono//src//camera//bev_car_224x380_alpha.raw");

/* ---- fixed perpective for bird's eye view cameras ---- */
static const svc_CfgPerspective_t loc_TopView = {
    .UseCamera = {true, true, true, true},
    .OutputFrameIndex = {
        SVC_DISP_LAYER_FG, /* front camera */
        SVC_DISP_LAYER_FG, /* back camera */
        SVC_DISP_LAYER_BG, /* left camera */
        SVC_DISP_LAYER_BG, /* right camera */
    },
    .Renderer = {
        /* front camera */
        {
            .InputWidth                 = SVC_CFG_CAM_WIDTH,
            .InputHeight                = SVC_CFG_CAM_HEIGHT,
            .InputTop                   = 0,
            .InputLeft                  = 0,
            .OutputWidth                = 640,
            .OutputHeight               = SVC_CFG_BEV_TOP_BOTTOM_HEIGHT,
            .OutputShiftDown            = 0,
            .OutputShiftRight           = 0,
            .OutputScaleWidth           = 1.0,
            .OutputScaleHeight          = 1.0,
            .OutputFrameWidth           = 1920,
            .OutputFrameHeight          = 1080,
            .OutputFrameTop             = 0, 
            .OutputFrameLeft            = 1280,
            .DoMirrorX                  = false,
            .DoLensDistortionCorrection = true,
            .DoHomography               = true,
            .LdcPara = {
                .K1 = SVC_CFG_CAM_F_DIST_COEFF_K1,
                .K2 = SVC_CFG_CAM_F_DIST_COEFF_K2,
                .K3 = SVC_CFG_CAM_F_DIST_COEFF_K3,
                .Fx = SVC_CFG_CAM_F_INTRI_Fx,
                .Fy = SVC_CFG_CAM_F_INTRI_Fy,
                .Cx = SVC_CFG_CAM_F_INTRI_Cx,
                .Cy = SVC_CFG_CAM_F_INTRI_Cy,
                .Sx = 1.0f,
                .Sy = 1.0f,
                .Ox = 0.0f,
                .Oy = 0.0f,
                .Fs = 0.5f,
            },
            .HomographyPara = {
                .H00 = 0.000816110576652272,
                .H01 = -0.0024181661638807,
                .H02 = 0.834938587193465,
                .H10 = -0.000034368069000182,
                .H11 = -0.000952338238996165,
                .H12 = 0.550333666752654,
                .H20 = -4.99948970765877E-08,
                .H21 = -3.78737009727917E-06,
                .H22 = 0.00172891476289972,
            },
        },
        /* back camera */
        {
            .InputWidth                 = SVC_CFG_CAM_WIDTH,
            .InputHeight                = SVC_CFG_CAM_HEIGHT,
            .InputTop                   = 0,
            .InputLeft                  = 0,
            .OutputWidth                = 640,
            .OutputHeight               = SVC_CFG_BEV_TOP_BOTTOM_HEIGHT,
            .OutputShiftDown            = 0,
            .OutputShiftRight           = 0,
            .OutputScaleWidth           = 1.0,
            .OutputScaleHeight          = 1.0,
            .OutputFrameWidth           = 1920,
            .OutputFrameHeight          = 1080,
            .OutputFrameTop             = 1080 - SVC_CFG_BEV_TOP_BOTTOM_HEIGHT, 
            .OutputFrameLeft            = 1280,
            .DoMirrorX                  = false,
            .DoLensDistortionCorrection = true,
            .DoHomography               = true,
            .LdcPara = {
                .K1 = SVC_CFG_CAM_B_DIST_COEFF_K1,
                .K2 = SVC_CFG_CAM_B_DIST_COEFF_K2,
                .K3 = SVC_CFG_CAM_B_DIST_COEFF_K3,
                .Fx = SVC_CFG_CAM_B_INTRI_Fx,
                .Fy = SVC_CFG_CAM_B_INTRI_Fy,
                .Cx = SVC_CFG_CAM_B_INTRI_Cx,
                .Cy = SVC_CFG_CAM_B_INTRI_Cy,
                .Sx = 1.0f,
                .Sy = 1.0f,
                .Ox = 0.0f,
                .Oy = 0.0f,
                .Fs = 0.5f,
            },
            .HomographyPara = {
                .H00 = 0.00189867685264523,
                .H01 = -0.0055367025411062,
                .H02 = -0.948480675609398,
                .H10 = -0.000345547369264809,
                .H11 = -0.00166372075250942,
                .H12 = -0.316776222429797,
                .H20 = -6.5949211523376E-07,
                .H21 = -8.70834222534673E-06,
                .H22 = -0.000292239341681782,
            },
        },
        /* left camera */
        {
            .InputWidth                 = SVC_CFG_CAM_WIDTH,
            .InputHeight                = SVC_CFG_CAM_HEIGHT,
            .InputTop                   = 0,
            .InputLeft                  = 0,
            .OutputWidth                = SVC_CFG_BEV_LEFT_RIGHT_WIDTH,
            .OutputHeight               = 1080,
            .OutputShiftDown            = 0,
            .OutputShiftRight           = 0,
            .OutputScaleWidth           = 1.0,
            .OutputScaleHeight          = 1.0,
            .OutputFrameWidth           = 1920,
            .OutputFrameHeight          = 1080,
            .OutputFrameTop             = 0, 
            .OutputFrameLeft            = 1280,
            .DoMirrorX                  = false,
            .DoLensDistortionCorrection = true,
            .DoHomography               = true,
            .LdcPara = {
                .K1 = SVC_CFG_CAM_L_DIST_COEFF_K1,
                .K2 = SVC_CFG_CAM_L_DIST_COEFF_K2,
                .K3 = SVC_CFG_CAM_L_DIST_COEFF_K3,
                .Fx = SVC_CFG_CAM_L_INTRI_Fx,
                .Fy = SVC_CFG_CAM_L_INTRI_Fy,
                .Cx = SVC_CFG_CAM_L_INTRI_Cx,
                .Cy = SVC_CFG_CAM_L_INTRI_Cy,
                .Sx = 1.0f,
                .Sy = 1.0f,
                .Ox = 0.0f,
                .Oy = 0.0f,
                .Fs = 0.5f,
            },
            .HomographyPara = {
                .H00 = -0.0021132030181499,
                .H01 = -0.000641730937618476,
                .H02 = 0.939011396164453,
                .H10 = -0.000897629345832279,
                .H11 = 8.57702576310017E-06,
                .H12 = 0.343876464772337,
                .H20 = -3.28690897495398E-06,
                .H21 = 3.16944099380938E-08,
                .H22 = 0.000944235436765083,
            },
        },
        /* right camera */
        {
            .InputWidth                 = SVC_CFG_CAM_WIDTH,
            .InputHeight                = SVC_CFG_CAM_HEIGHT,
            .InputTop                   = 0,
            .InputLeft                  = 0,
            .OutputWidth                = SVC_CFG_BEV_LEFT_RIGHT_WIDTH,
            .OutputHeight               = 1080,
            .OutputShiftDown            = 0,
            .OutputShiftRight           = 0,
            .OutputScaleWidth           = 1.0,
            .OutputScaleHeight          = 1.0,
            .OutputFrameWidth           = 1920,
            .OutputFrameHeight          = 1080,
            .OutputFrameTop             = 0, 
            .OutputFrameLeft            = 1920 - SVC_CFG_BEV_LEFT_RIGHT_WIDTH,
            .DoMirrorX                  = false,
            .DoLensDistortionCorrection = true,
            .DoHomography               = true,
            .LdcPara = {
                .K1 = SVC_CFG_CAM_R_DIST_COEFF_K1,
                .K2 = SVC_CFG_CAM_R_DIST_COEFF_K2,
                .K3 = SVC_CFG_CAM_R_DIST_COEFF_K3,
                .Fx = SVC_CFG_CAM_R_INTRI_Fx,
                .Fy = SVC_CFG_CAM_R_INTRI_Fy,
                .Cx = SVC_CFG_CAM_R_INTRI_Cx,
                .Cy = SVC_CFG_CAM_R_INTRI_Cy,
                .Sx = 1.0f,
                .Sy = 1.0f,
                .Ox = 0.0f,
                .Oy = 0.0f,
                .Fs = 0.5f,
            },
            .HomographyPara = {
                .H00 = -0.00892731510640612,
                .H01 = -0.00241414394080759,
                .H02 = 0.758899258970354,
                .H10 = -0.00406568321178052,
                .H11 = 0.000236493234880596,
                .H12 = -0.6511290835941,
                .H20 = -1.39409881751869E-05,
                .H21 = 3.29339594965037E-07,
                .H22 = -0.000848694698649454,
            },
        },
    },
};

static const svc_CfgPerspective_t loc_MainView[SVC_CFG_MAIN_PERSPECTIVE_NUM] = {
    /* ---- perpective for combined raw images view cameras ---- */
    {
        .UseCamera = {true, true, true, true},
        .OutputFrameIndex = {
            SVC_DISP_LAYER_BG, /* front camera */
            SVC_DISP_LAYER_BG, /* back camera */
            SVC_DISP_LAYER_BG, /* left camera */
            SVC_DISP_LAYER_BG, /* right camera */
        },
        .Renderer = {
            /* front camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH / 2,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT / 2,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 0.5,
                .OutputScaleHeight          = 0.5,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = (1080 - 800) / 2, 
                .OutputFrameLeft            = 0,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = false,
                .DoHomography               = false,
            },
            /* back camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH / 2,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT / 2,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 0.5,
                .OutputScaleHeight          = 0.5,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = (1080 - 800) / 2, 
                .OutputFrameLeft            = 1280 / 2,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = false,
                .DoHomography               = false,
            },
            /* left camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH / 2,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT / 2,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 0.5,
                .OutputScaleHeight          = 0.5,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = 1080 / 2, 
                .OutputFrameLeft            = 0,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = false,
                .DoHomography               = false,
            },
            /* right camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH / 2,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT / 2,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 0.5,
                .OutputScaleHeight          = 0.5,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = 1080 / 2, 
                .OutputFrameLeft            = 1280 / 2,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = false,
                .DoHomography               = false,
            },
        },
    },

    /* ---- perpective for combined undistorted images view cameras ---- */
    {
        .UseCamera = {true, true, true, true},
        .OutputFrameIndex = {
            SVC_DISP_LAYER_BG, /* front camera */
            SVC_DISP_LAYER_BG, /* back camera */
            SVC_DISP_LAYER_BG, /* left camera */
            SVC_DISP_LAYER_BG, /* right camera */
        },
        .Renderer = {
            /* front camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH / 2,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT / 2,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 0.5,
                .OutputScaleHeight          = 0.5,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = (1080 - 800) / 2, 
                .OutputFrameLeft            = 0,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = true,
                .DoHomography               = false,
                .LdcPara = {
                    .K1 = SVC_CFG_CAM_F_DIST_COEFF_K1,
                    .K2 = SVC_CFG_CAM_F_DIST_COEFF_K2,
                    .K3 = SVC_CFG_CAM_F_DIST_COEFF_K3,
                    .Fx = SVC_CFG_CAM_F_INTRI_Fx,
                    .Fy = SVC_CFG_CAM_F_INTRI_Fy,
                    .Cx = SVC_CFG_CAM_F_INTRI_Cx,
                    .Cy = SVC_CFG_CAM_F_INTRI_Cy,
                    .Sx = 1.0f,
                    .Sy = 1.0f,
                    .Ox = 0.0f,
                    .Oy = 0.0f,
                    .Fs = 0.5f,
                },
            },
            /* back camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH / 2,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT / 2,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 0.5,
                .OutputScaleHeight          = 0.5,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = (1080 - 800) / 2, 
                .OutputFrameLeft            = 1280 / 2,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = true,
                .DoHomography               = false,
                .LdcPara = {
                    .K1 = SVC_CFG_CAM_B_DIST_COEFF_K1,
                    .K2 = SVC_CFG_CAM_B_DIST_COEFF_K2,
                    .K3 = SVC_CFG_CAM_B_DIST_COEFF_K3,
                    .Fx = SVC_CFG_CAM_B_INTRI_Fx,
                    .Fy = SVC_CFG_CAM_B_INTRI_Fy,
                    .Cx = SVC_CFG_CAM_B_INTRI_Cx,
                    .Cy = SVC_CFG_CAM_B_INTRI_Cy,
                    .Sx = 1.0f,
                    .Sy = 1.0f,
                    .Ox = 0.0f,
                    .Oy = 0.0f,
                    .Fs = 0.5f,
                },
            },
            /* left camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH / 2,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT / 2,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 0.5,
                .OutputScaleHeight          = 0.5,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = 1080 / 2, 
                .OutputFrameLeft            = 0,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = true,
                .DoHomography               = false,
                .LdcPara = {
                    .K1 = SVC_CFG_CAM_L_DIST_COEFF_K1,
                    .K2 = SVC_CFG_CAM_L_DIST_COEFF_K2,
                    .K3 = SVC_CFG_CAM_L_DIST_COEFF_K3,
                    .Fx = SVC_CFG_CAM_L_INTRI_Fx,
                    .Fy = SVC_CFG_CAM_L_INTRI_Fy,
                    .Cx = SVC_CFG_CAM_L_INTRI_Cx,
                    .Cy = SVC_CFG_CAM_L_INTRI_Cy,
                    .Sx = 1.0f,
                    .Sy = 1.0f,
                    .Ox = 0.0f,
                    .Oy = 0.0f,
                    .Fs = 0.5f,
                },
            },
            /* right camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH / 2,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT / 2,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 0.5,
                .OutputScaleHeight          = 0.5,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = 1080 / 2, 
                .OutputFrameLeft            = 1280 / 2,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = true,
                .DoHomography               = false,
                .LdcPara = {
                    .K1 = SVC_CFG_CAM_R_DIST_COEFF_K1,
                    .K2 = SVC_CFG_CAM_R_DIST_COEFF_K2,
                    .K3 = SVC_CFG_CAM_R_DIST_COEFF_K3,
                    .Fx = SVC_CFG_CAM_R_INTRI_Fx,
                    .Fy = SVC_CFG_CAM_R_INTRI_Fy,
                    .Cx = SVC_CFG_CAM_R_INTRI_Cx,
                    .Cy = SVC_CFG_CAM_R_INTRI_Cy,
                    .Sx = 1.0f,
                    .Sy = 1.0f,
                    .Ox = 0.0f,
                    .Oy = 0.0f,
                    .Fs = 0.5f,
                },
            },
        },
    },

    /* ---- perpective for undistorted front camera ---- */
    {
        .UseCamera = {true, false, false, false},
        .OutputFrameIndex = {
            SVC_DISP_LAYER_BG, /* front camera */
            SVC_DISP_LAYER_BG, /* not used */
            SVC_DISP_LAYER_BG, /* not used */
            SVC_DISP_LAYER_BG, /* not used */
        },
        .Renderer = {
            /* front camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 1.0,
                .OutputScaleHeight          = 1.0,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = (1080 - 800) / 2, 
                .OutputFrameLeft            = 0,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = true,
                .DoHomography               = false,
                .LdcPara = {
                    .K1 = SVC_CFG_CAM_F_DIST_COEFF_K1,
                    .K2 = SVC_CFG_CAM_F_DIST_COEFF_K2,
                    .K3 = SVC_CFG_CAM_F_DIST_COEFF_K3,
                    .Fx = SVC_CFG_CAM_F_INTRI_Fx,
                    .Fy = SVC_CFG_CAM_F_INTRI_Fy,
                    .Cx = SVC_CFG_CAM_F_INTRI_Cx,
                    .Cy = SVC_CFG_CAM_F_INTRI_Cy,
                    .Sx = 1.0f,
                    .Sy = 1.0f,
                    .Ox = 0.0f,
                    .Oy = 0.0f,
                    .Fs = 0.5f,
                },
            },
            /* back camera */
            { 0 },
            /* left camera */
            { 0 },
            /* right camera */
            { 0 },
        },
    },

    /* ---- perpective for undistorted back camera ---- */
    {
        .UseCamera = {false, true, false, false},
        .OutputFrameIndex = {
            SVC_DISP_LAYER_BG, /* not used */
            SVC_DISP_LAYER_BG, /* back camera */
            SVC_DISP_LAYER_BG, /* not used */
            SVC_DISP_LAYER_BG, /* not used */
        },
        .Renderer = {
            /* front camera */
            { 0 },
            /* back camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 1.0,
                .OutputScaleHeight          = 1.0,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = (1080 - 800) / 2, 
                .OutputFrameLeft            = 0,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = true,
                .DoHomography               = false,
                .LdcPara = {
                    .K1 = SVC_CFG_CAM_B_DIST_COEFF_K1,
                    .K2 = SVC_CFG_CAM_B_DIST_COEFF_K2,
                    .K3 = SVC_CFG_CAM_B_DIST_COEFF_K3,
                    .Fx = SVC_CFG_CAM_B_INTRI_Fx,
                    .Fy = SVC_CFG_CAM_B_INTRI_Fy,
                    .Cx = SVC_CFG_CAM_B_INTRI_Cx,
                    .Cy = SVC_CFG_CAM_B_INTRI_Cy,
                    .Sx = 1.0f,
                    .Sy = 1.0f,
                    .Ox = 0.0f,
                    .Oy = 0.0f,
                    .Fs = 0.5f,
                },
            },
            /* left camera */
            { 0 },
            /* right camera */
            { 0 },
        },
    },

    /* ---- perpective for undistorted left camera ---- */
    {
        .UseCamera = {false, false, true, false},
        .OutputFrameIndex = {
            SVC_DISP_LAYER_BG, /* not used */
            SVC_DISP_LAYER_BG, /* not used */
            SVC_DISP_LAYER_BG, /* left camera */
            SVC_DISP_LAYER_BG, /* not used */
        },
        .Renderer = {
            /* front camera */
            { 0 },
            /* back camera */
            { 0 },
            /* left camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 1.0,
                .OutputScaleHeight          = 1.0,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = (1080 - 800) / 2, 
                .OutputFrameLeft            = 0,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = true,
                .DoHomography               = false,
                .LdcPara = {
                    .K1 = SVC_CFG_CAM_L_DIST_COEFF_K1,
                    .K2 = SVC_CFG_CAM_L_DIST_COEFF_K2,
                    .K3 = SVC_CFG_CAM_L_DIST_COEFF_K3,
                    .Fx = SVC_CFG_CAM_L_INTRI_Fx,
                    .Fy = SVC_CFG_CAM_L_INTRI_Fy,
                    .Cx = SVC_CFG_CAM_L_INTRI_Cx,
                    .Cy = SVC_CFG_CAM_L_INTRI_Cy,
                    .Sx = 1.0f,
                    .Sy = 1.0f,
                    .Ox = 0.0f,
                    .Oy = 0.0f,
                    .Fs = 0.5f,
                },
            },
            /* right camera */
            { 0 },
        },
    },

    /* ---- perpective for undistorted right camera ---- */
    {
        .UseCamera = {false, false, false, true},
        .OutputFrameIndex = {
            SVC_DISP_LAYER_BG, /* not used */
            SVC_DISP_LAYER_BG, /* not used */
            SVC_DISP_LAYER_BG, /* not used */
            SVC_DISP_LAYER_BG, /* right camera */
        },
        .Renderer = {
            /* front camera */
            { 0 },
            /* back camera */
            { 0 },
            /* left camera */
            { 0 },
            /* right camera */
            {
                .InputWidth                 = SVC_CFG_CAM_WIDTH,
                .InputHeight                = SVC_CFG_CAM_HEIGHT,
                .InputTop                   = 0,
                .InputLeft                  = 0,
                .OutputWidth                = SVC_CFG_CAM_WIDTH,
                .OutputHeight               = SVC_CFG_CAM_HEIGHT,
                .OutputShiftDown            = 0,
                .OutputShiftRight           = 0,
                .OutputScaleWidth           = 1.0,
                .OutputScaleHeight          = 1.0,
                .OutputFrameWidth           = 1920,
                .OutputFrameHeight          = 1080,
                .OutputFrameTop             = (1080 - 800) / 2, 
                .OutputFrameLeft            = 0,
                .DoMirrorX                  = false,
                .DoLensDistortionCorrection = true,
                .DoHomography               = false,
                .LdcPara = {
                    .K1 = SVC_CFG_CAM_R_DIST_COEFF_K1,
                    .K2 = SVC_CFG_CAM_R_DIST_COEFF_K2,
                    .K3 = SVC_CFG_CAM_R_DIST_COEFF_K3,
                    .Fx = SVC_CFG_CAM_R_INTRI_Fx,
                    .Fy = SVC_CFG_CAM_R_INTRI_Fy,
                    .Cx = SVC_CFG_CAM_R_INTRI_Cx,
                    .Cy = SVC_CFG_CAM_R_INTRI_Cy,
                    .Sx = 1.0f,
                    .Sy = 1.0f,
                    .Ox = 0.0f,
                    .Oy = 0.0f,
                    .Fs = 0.5f,
                },
            },
        },
    },
};

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/
const svc_Config_t fastboot_SvcConfig = {

    /* ---- display settings ---- */
    .Display =
        {
            .DisplayPort          = SVC_DISP_PORT_HDMI0,
            .DisplayWidth         = 1920,
            .DisplayHeight        = 1080,
            .ModelCarLayerIndex   = 1, /* HW display plane Z index */
            .ForegroundLayerIndex = 2, /* HW display plane Z index */
            .BackgroundLayerIndex = 3, /* HW display plane Z index */
        },

    /* ---- camera settings ---- */
    .Camera =
        {
            /* front camera */
            {
                .DevName = "RCar-VIN:1",
                .Width   = SVC_CFG_CAM_WIDTH,
                .Height  = SVC_CFG_CAM_HEIGHT,
            },
            /* back camera */
            {
                .DevName = "RCar-VIN:2",
                .Width   = SVC_CFG_CAM_WIDTH,
                .Height  = SVC_CFG_CAM_HEIGHT,
            },
            /* left camera */
            {
                .DevName = "RCar-VIN:0",
                .Width   = SVC_CFG_CAM_WIDTH,
                .Height  = SVC_CFG_CAM_HEIGHT,
            },
            /* right camera */
            {
                .DevName = "RCar-VIN:3",
                .Width   = SVC_CFG_CAM_WIDTH,
                .Height  = SVC_CFG_CAM_HEIGHT,
            },
        },

    /* ---- layout of bird's eye view images  ---- */
    .TopViewInfo =
        {
            .FrameWidth       = 1920,
            .FrameHeight      = 1080,
            .Top              = 0,
            .Left             = 1280,
            .Width            = 640,
            .Height           = 1080,
            .FBCamWidth       = 640,
            .FBCamHeight      = SVC_CFG_BEV_TOP_BOTTOM_HEIGHT,
            .LRCamWidth       = SVC_CFG_BEV_LEFT_RIGHT_WIDTH,
            .LRCamHeight      = 1080,
            .ModelCarWidth    = 224,
            .ModelCarHeight   = 380,
            .ModelCarDataRef  = gModelCarData,
            .ModelCarAlphaRef = gModelCarAlphaData,
        },

    .TopViewRef       = &loc_TopView,
    .NumberOfMainView = SVC_CFG_MAIN_PERSPECTIVE_NUM,
    .MainViewRef      = loc_MainView,
};

/* clang-format on */

#endif /* defined(R_TARGET_BOARD_ERGUOTOU) */
#endif /* FASTBOOT_CFG_APP_CAMERA_ENABLE */
