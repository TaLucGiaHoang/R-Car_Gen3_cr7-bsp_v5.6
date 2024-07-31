/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef _ERROR_H_
#define _ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Error Module Interface
  
  This is the interface to the application's error
  handling.
*/

/***********************************************************
  Section: Global Defines
*/

/***********************************************************
  Constants: GFX_APPNOTE Error Codes

  Error codes of the GFX_APPNOTE.

  ERR_OK                     - No error
  ERR_NG                     - Unspecified error
  ERR_TEXTURE_SETUP_FAILED   - Setup of a texture failed
  ERR_FILESYSTEM_BROKEN      - File system check failed
*/

#define ERR_OK                    (0)
#define ERR_NG                    (-10)
#define ERR_TEXTURE_SETUP_FAILED  (-20)
#define ERR_FILESYSTEM_BROKEN     (-30)


/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: ClusterError

  Application error handling.

  If an application function detects an error, it will call
  this function with 'Err != 0'.

  Parameters:
  Err       - Error code (0 == no error)

  Returns:
  void
*/

void ClusterError(int Err);


#ifdef __cplusplus
}
#endif

#endif /* _ERROR_H_ */

