/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef _FONT_H_
#define _FONT_H_

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
  Title: Font Interface
  
  This header describes the structure of a bitmap font,
  which can be used by the framework.
*/


/***********************************************************
  Section: Global Types
*/

/***********************************************************
  Type: FontPos_t

  Character position and width information.

  The type is used to specify the position of a character
  inside a font texture.

  Members:
  Offset  - Pixel position in the texture
  Width   - Pixel width in the texture
*/

typedef struct {
    int Offset;
    int Width;
} FontPos_t;


/***********************************************************
  Type: Font_t

  Font data type.

  This structure contains the data of a font.

  Members:
  Texture - Pointer to the font's image structure
  Pos     - Array of character information
  Height  - Pixel height of the font
*/

typedef struct {
    Img_t           *Texture;
    const FontPos_t *Pos;
    int              Height;
} Font_t;


/***********************************************************
  Section: Global Variables
*/

/***********************************************************
  Variable: Font16

  Data of Font16
*/

extern const Font_t    Font16;


/***********************************************************
  Variable: FontArial19

  Data of FontArial19
*/

extern const Font_t    FontArial19;


/***********************************************************
  Variable: FontArial38

  Data of FontArial38
*/

extern const Font_t    FontArial38;


/***********************************************************
  Variable: FontStatus

  Data of FontStatus
*/

extern const Font_t    FontStatus;


/***********************************************************
  Variable: FontHelvM11

  Data of font Helvetia middle 11pt
*/

extern const Font_t    FontHelvM11;


/***********************************************************
  Variable: FontHelvM12

  Data of font Helvetia middle 12pt
*/

extern const Font_t    FontHelvM12;


/***********************************************************
  Variable: FontHelvM14

  Data of font Helvetia middle 14pt
*/

extern const Font_t    FontHelvM14;


/***********************************************************
  Variable: FontHud32

  Data of font HUD 32pt (arial) only numbers [0-9], 'km/h', 'mph'
*/

extern const Font_t    FontHud32;


/***********************************************************
  Variable: FontHud90

  Data of font HUD 90pt (arial) only numbers [0-9]
*/

extern const Font_t    FontHud90;


#ifdef __cplusplus
}
#endif

#endif /* _FONT_H_ */

