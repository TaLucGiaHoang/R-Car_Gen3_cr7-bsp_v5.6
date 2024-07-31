/*************************************************************************************************************
* WindowMng_wm_bridge
* Copyright (c) [2021-2022] Renesas Electronics Corporation
* Copyright (c) [2019-2020] Renesas Electronics (Europe) GmbH.
* This software is released under the MIT License
* http://opensource.org/licenses/mit-license.php
*************************************************************************************************************/

#ifndef R_WM_BRIDGE_H_
#define R_WM_BRIDGE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Title: Window Manager Bridge declaration
 *        Contain the type declartions that are needed by both the WM User
 *        library and the WM CIO device
 */

/*
 * Section Include
 */
#include "r_wm_common.h"

/**
 * \defgroup PrivDef WM User Bridge
 * @ingroup middleware_wm
 * @brief WM internal types and APIs
 */

/**
  \defgroup PrivTypes  WM internal Types
  \ingroup  PrivDef
  @brief WM internal types
*/

/**
  \defgroup WmMsgTypes  WM Message Types
  \ingroup  PrivTypes
  @brief Types used to transfer data between the WM user and driver lib
  @{
*/

/**
 * List of Message Commands that the WM supports
 *
 */
typedef enum {
    R_WM_MSG_CMD_INIT,              /**< command to initialise the WM driver */
    R_WM_MSG_CMD_DEINIT,            /**< command to de-initialise the WM driver*/
    R_WM_MSG_CMD_REGISTER_EVENT,    /**< command to register an event for a WM device */
    R_WM_MSG_CMD_SC_ENABLE,         /**< command to enable screen */
    R_WM_MSG_CMD_SC_BKGCL,          /**< command to set the screen background color */
    R_WM_MSG_CMD_SC_TIMINGS,        /**< command to configure the display timings */
    R_WM_MSG_CMD_SC_FMT,            /**< command to set the screen color format */
    R_WM_MSG_CMD_SURF_GET,          /**< command to get a surface from the current WM device */
    R_WM_MSG_CMD_SURF_POS,          /**< command to set the position of the surface on screen */
    R_WM_MSG_CMD_SURF_SIZE,         /**< command to set the surface size */
    R_WM_MSG_CMD_SURF_BUF,          /**< command to set the surface framebuffer */
    R_WM_MSG_CMD_SURF_FMT,          /**< command to set the surface color format */
    R_WM_MSG_CMD_SC_FLUSH,          /**< command to flush the WM device */
    R_WM_MSG_CMD_SURF_RELEASE,      /**< command to release a given surface */
    R_WM_MSG_CMD_DOC_CFG,           /**< command to initialize DOC */
    R_WM_MSG_CMD_DOC_ENABLE,        /**< command to enable/disable monitors */
    R_WM_MSG_CMD_DOC_CLEAR,         /**< command to clear DOC status flags */
    R_WM_MSG_CMD_SURF_DISCOM_CFG,   /**< command to configure DISCOM */
    R_WM_MSG_CMD_SURFACE_ALPHA_SET,  /**< command to set window alpha of the WM device */

    R_WM_MSG_CMD_LAST               /**< Delimiter */
} r_wm_Msg_Cmd_t;

/**
 * Data Structure providing the window surface position
 */
typedef struct {
    uint32_t Pid;   /**< ID of the requesting process */
    uint32_t PosX;  /**< position of the surface on the X axis in Pixel */
    uint32_t PosY;  /**< position of the surface on the Y axis in Pixel */
    uint32_t PosZ;  /**< position of the surface on the Z axis.
                     For R-Car, this corresponds to the VSPD Layer number. */
    void    *Handle; /**< pointer to Surface Handle */
} r_wm_Msg_Surf_Pos_t;

/**
 * Data Structure providing the window surface size
 *
 * @note    Alpha plane is not supported.
 */
typedef struct {
    uint32_t Pid;       /**< ID of the requesting process */
    uint32_t Width;     /**< Width of the window surface in Pixel */
    uint32_t Height;    /**< Height of the window surface in Pixel */
    uint16_t StrideY;   /**< StrideY of the window surface in Byte */
    uint16_t StrideC;   /**< StrideC of the window surface in Byte */
    void    *Handle;    /**< pointer to Surface Handle */
} r_wm_Msg_Surf_Size_t;

/**
 * Data Structure providing the window surface Framebuffer
 * address
 */
typedef struct {
    uint32_t  Pid;      /**< ID of the requesting process */
    uintptr_t Fb;       /**< frambuffer address of the surface */
    uintptr_t C0;       /**< frambuffer address of the surface for C0 */
    uintptr_t C1;       /**< frambuffer address of the surface for C1 */
    void     *Handle;   /**< pointer to Surface Handle */
} r_wm_Msg_Surf_Buf_t;

/**
 * Data Structure providing the window surface format
 */
typedef struct {
    uint32_t  Pid;      /**< ID of the requesting process */
    r_wm_ColorFmt_t Format; /**< format of the surface */
    void     *Handle;   /**< pointer to Surface Handle */
} r_wm_Msg_Surf_Format_t;

/**
 * Data Structure providing the window surface handle to be
 * released
 */
typedef struct {
    uint32_t Pid;       /**< ID of the requesting process */
    void    *Handle;    /**< pointer to Surface Handle */
} r_wm_Msg_Surf_Release_t;

/**
 * Data Structure containing the data for registering an Event
 */
typedef struct {
    uint32_t         Pid;         /**< ID of the requesting process */
    r_wm_EventId_t   EvId;        /**< event to be register. @ref r_wm_EventId_t */
    void            *MqHandle;    /**< Handle for Message Queue */
    uint32_t        *Arg;         /**< If the event requires an argument (e.g.: scanline). Shall be Null
                                       if none required */
} r_wm_Msg_Event_t;

/**
 * Message returned by the server when the WM Instance has already been initialised.
 */
typedef struct {
    uint32_t ScreenWidth;   /**< Screen width in Pixel */
    uint32_t ScreenHeight;  /**< Screen height in Pixel */
    uint32_t LayerNb;       /**< number of layers the screen/WM device supports */
} r_wm_Msg_DevInfo_t;

typedef struct {
    uint32_t Pid;                      /**< ID of the requesting process */
    osal_mq_handle_t    MqHandle;      /**< Handle for Message Queue */
} r_wm_Msg_ScreenFlush_t;

/**
    Message structure used to set the surface Alpha value
*/
typedef struct
{
    uint32_t    Pid;       /**< ID of the requesting process */
    uint8_t     Alpha;     /**< Alpha ratio to be set to the surface */
    uint32_t    rpfid;     /**< RPFID (z-position) of the surface/layer */
    void        *Handle;   /**< pointer to surface Alpha Ratio Handle */
} r_wm_Msg_SurfAlphaData_t;

/**
* @struct r_wm_Msg_DocInfo_t
* @brief Data Structure containing the parameters of DOC
*/
typedef struct {
    r_wm_DocCb_t        Callback;   /**< Callback when ACT/DOC error occurs */
    void                *CbArg;     /**< Argument for the callback */
    uint32_t            Pid;        /**< ID of the requesting process */
    uint32_t            DOC_CH;     /**< DOC Unit */
    uint32_t            DU_CH;      /**< Video Channel of DOC */
    r_wm_Doc_MonitorParam_t *DocMonitors; /**< the parameter of VOM */
    r_wm_Doc_DUParam_t  DU_param;   /**< the parameter used by DU */
    uint32_t            Enable;     /**< Enable/Disable flag of DOC */
    bool                ActMonEnable;     /**< Enable/Disable ActiveMonitor */
    uint32_t            ActMonUpperTime;  /**< Active Monitor upper time */
    uint32_t            ActMonLowerTime;  /**< Active Monitor lower time */
} r_wm_Msg_DocInfo_t;

/**
 * Data Structure providing the window surface discom
 */
typedef struct {
    r_wm_DiscomCb_t Callback; /**< Callback when Crc mismatch occurs */
    void    *CbArg;     /**< Argument for the callback */
    void    *Handle;    /**< pointer to Surface Handle */
    uint32_t Pid;       /**< ID of the requesting process */
    uint32_t PosX;      /**< ROI position on the X axis within the surface, in pixels.
                             Must be a multiple of 2 for M3-W(+) */
    uint32_t PosY;      /**< ROI position on the Y axis within the surface, in pixels */
    uint32_t Width;     /**< ROI width */
    uint32_t Height;    /**< ROI height */
    uint32_t Crc;       /**< ROI expected CRC */
    bool     Enable;    /**< Enable/Disable DISCOM */
} r_wm_Msg_DiscomInfo_t;

/**
 * List of all the possible structure types that the WM can send or receive.
 */
typedef union {
    uint32_t                Val;                  /**< Used if an integer Value is sent */
    void                    *Ptr;                 /**< Pointer */
    r_wm_Msg_Event_t        EventRegMsgData;      /**< Event register data */
    r_wm_Msg_Surf_Buf_t     SurfBufSetMsgData;    /**< Surface framebuffer info */
    r_wm_Msg_Surf_Format_t  SurfFormatSetMsgData; /**< Surface format info */
    r_wm_Msg_Surf_Size_t    SurfSizeSetMsgData;   /**< Surface size info*/
    r_wm_Msg_Surf_Pos_t     SurfPosSetMsgData;    /**< Surface position info*/
    r_wm_Msg_Surf_Release_t SurfReleaseMsgData;   /**< Info on the surface to be released */
    r_wm_Msg_DevInfo_t      DevInfoData;          /**< WM Device info */
    r_wm_Msg_ScreenFlush_t  ScreenFlushData;      /**< Info required to flush screen */
    r_wm_Msg_DocInfo_t      DocMsgData;           /*< DOC information */
    r_wm_Msg_DiscomInfo_t DiscomMsgData;          /**< DISCOM info */
    r_wm_Msg_SurfAlphaData_t SurfAlphaSetMsgData; /**< Surface Alpha value */
} r_wm_Msg_Generic_t;

/**
 * Standard Receive WM Message structure
 */
typedef struct {
    r_wm_Error_t         Err;   /**< Error message. @ref r_wm_Error_t */
    r_wm_Msg_Generic_t   Data;  /**< Pointer to the message sent. Contains the
                                    data information related to the command to be
                                    executed. @ref r_wm_Msg_Generic_t */
} r_wm_MsgRx_t;

/**
 * Standard transmit WM Message structure
 */
typedef struct {
    r_wm_Dev_t            Dev;      /**< WM Device to which the command will apply */
    r_wm_Msg_Cmd_t        MsgCmd;   /**< Command. See @ref r_wm_Msg_Cmd_t */
    r_wm_Msg_Generic_t    Data;     /**< Pointer to the data information related
                                         to the command to be executed */
} r_wm_MsgTx_t;

/** @} */ /* End group PrvTypes */

#ifdef __cplusplus
}
#endif

#endif /* R_WM_BRIDGE_H_ */
