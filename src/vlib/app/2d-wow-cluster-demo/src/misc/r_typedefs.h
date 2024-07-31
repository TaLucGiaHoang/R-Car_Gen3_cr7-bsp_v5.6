/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

/***********************************************************
*
*                   Defines
*
************************************************************
*/

#define EE_LOCAL  static
#define EE_API

#define EE_STR(s)           # s
#define EE_XSTR(s)          EE_STR(s)
#define EE_DOT(a,b)         a ## . ## b
#define EE_XDOT(a,b)        EE_DOT(a,b)

#define EE_SPACE            " "
#define EE_VERSIONPREFIX    "@(#)Version "
#define EE_VERSIONPOSTFIX   " - "

#define EE_NULL (void *)0

#define EE_IAR_REG_VOLATILE_PREFIX
/***********************************************************
*
*                   Enumerators 
*
************************************************************
*/

typedef enum {
    FALSE = 0,
    TRUE  = !FALSE
} ee_Bool_t;

typedef enum {
  E_OK = 0,
  E_NG,
  E_PARAMETER_MISSING,
  E_PARAMETER_INCORRECT
} ee_Error_t;


/***********************************************************
*
*                   Types
*
************************************************************
*/

/* Define ISO conform exact types, if the compiler does not
   support them,
*/

#ifndef uint8_t
typedef signed char        int8_t;
typedef signed short       int16_t;  
typedef signed int         int32_t; 
typedef signed long long   int64_t;  
typedef unsigned char      uint8_t;  
typedef unsigned short     uint16_t; 
typedef unsigned int       uint32_t; 
typedef unsigned long long uint64_t;  
#endif /* uint8_t */

typedef unsigned int   ee_Size_t; 
typedef void* ee_Handle_t;  

#ifndef NULL
#define NULL 0
#endif


#endif /* _TYPEDEF_H_ */

