/******************************************************************************
 *
 * PURPOSE
 *   RIVP CMN TYPE Defines
 *
 * AUTHOR
 *   Renesas Electronics Corporation
 *
 *
 *****************************************************************************/
/**
 * Copyright(C) 2015 Renesas Electronics Corporation. All Rights Reserved.
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation.
 * No part of this program may be reproduced or disclosed to
 * others, in any form, without the prior written permission
 * of Renesas Electronics Corporation.
 *
 *****************************************************************************/

#ifndef RIVP_LDD_TYPES_H
#define RIVP_LDD_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/*                    INCLUDE FILES                                          */
/*****************************************************************************/

/*****************************************************************************/
/*                    MACROS/DEFINES                                         */
/*****************************************************************************/
#ifndef RIVP_LDD_NULL
#ifdef  __cplusplus
#define RIVP_LDD_NULL	(0)
#else
#define RIVP_LDD_NULL	((void *)0)
#endif
#endif

/* static macro for unittest */
#ifndef RIVP_LDD_BUILD_UNIT_TEST
 #define RIVP_LDD_STATIC static      /**< static */
#else
 #define RIVP_LDD_STATIC             /**< for test */
#endif

/*****************************************************************************/
/*                    TYPE DEFINITION                                        */
/*****************************************************************************/
typedef signed char			RIVP_LDD_S8;
typedef unsigned char		RIVP_LDD_U8;
typedef signed short		RIVP_LDD_S16;
typedef unsigned short		RIVP_LDD_U16;
typedef signed int			RIVP_LDD_S32;
typedef unsigned int		RIVP_LDD_U32;
typedef signed long long	RIVP_LDD_S64;
typedef unsigned long long	RIVP_LDD_U64;
typedef void *				RIVP_LDD_PTR;

typedef enum {
	RIVP_LDD_FALSE				= 0,	/**< value of false	*/
	RIVP_LDD_TRUE				= 1		/**< value of true	*/
} RIVP_LDD_BOOL;

typedef enum {
	RIVP_LDD_RTN_OK				= 0x00,	/**< Success */
	RIVP_LDD_RTN_WARNING		= 0x01,	/**< The warning.
										 *   Continued operation is possible.
										 */
	RIVP_LDD_RTN_ERROR_PARAM	= 0x02,	/**< This error shows that the argument
										 *   has invalid parameter
										 */
	RIVP_LDD_RTN_FATAL_CONTEXT	= 0x03,	/**< This error shows that the using
										 *   context becomes an abnormal condition
										 */
	RIVP_LDD_RTN_FATAL_LIBRARY	= 0x04,	/**< This error shows that this library
										 *   becomes an abnormal condition
										 */
	RIVP_LDD_RTN_FATAL_SYSTEM	= 0x05 	/**< This error shows the abnormal
										 *   condition that cannot be restored only
										 *   in this library
										 */
} RIVP_LDD_RESULT;

/*****************************************************************************/
/*                    FUNCTIONS                                              */
/*****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* RIVP_LDD_TYPES_H */
