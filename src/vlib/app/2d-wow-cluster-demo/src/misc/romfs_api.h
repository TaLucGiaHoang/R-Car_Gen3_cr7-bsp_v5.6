/*
****************************************************************************
  Copyright (c) [2021-2022] Renesas Electronics Corporation
  This software is released under the MIT License.
  http://opensource.org/licenses/mit-license.php
****************************************************************************
*/

#ifndef _EE_ROMFS_H_
#define _EE_ROMFS_H_

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************************
  Title: ROM File System Interface
*/


/***********************************************************
  Section: Global Defines

  Definition of all global defines.
*/


/* Limits */
#define EE_ROMFS_MAXNAMELENGTH  256


/***********************************************************
  Section: Global Types

  Global types of the main module are defined here.
*/


typedef struct {
  uint32_t  Pos;
  uint32_t  Size;
  char     *Address;
  char      InUse;
} eeRomFs_File_t;

typedef struct {
  char        Name[EE_ROMFS_MAXNAMELENGTH];
  uint32_t    Size;
  char       *Address;
} eeRomFs_Data_t;

/***********************************************************
  Section: Global Functions
*/

/***********************************************************
  Function: eeRomFs_FOpen

  Open a file.

  Parameters:
  Name       - Filename
  Mode       - Modus (r,rw,...)

  Returns:
  ==0        - Cannot open File
  !=0        - Pointer to file handle
*/

eeRomFs_File_t *eeRomFs_FOpen(const char *Name, char *Mode);


/***********************************************************
  Function: eeRomFs_FClose

  Close a file.

  Parameters:
  Fp         - File pointer

  Returns:
  ==0        - OK
  !=0        - NG
*/

int eeRomFs_FClose(eeRomFs_File_t *Fp);


/***********************************************************
  Function: eeRomFs_FRead

  Read from a file.

  Parameters:
  Buffer     - Pointer to a buffer, where data is read to
  Size       - Byte size of a single element
  Count      - Number of elements to read from the file
  Fp         - Pointer to a file handle

  Returns:
  Number of elements, which have been read
*/

uint32_t eeRomFs_FRead(void *Buffer, uint32_t Size, uint32_t Count, eeRomFs_File_t *Fp);


/***********************************************************
  Function: eeRomFs_MMap

  Creates a new mapping in the virtual address space of the calling
  process.
  
  Parameters:
  Addr       - TBD
  Size       - TBD
  Prot       - TBD
  Flags      - TBD
  Fp         - Pointer to a file handle
  Offset     - TBD

  Returns:
  The address of the new mapping is returned as the result of the call.
*/

void *eeRomFs_MMap(void *Addr, uint32_t Size, int32_t Prot, int32_t Flags, eeRomFs_File_t *Fp, uint32_t Offset);


/***********************************************************
  Function: eeRomFs_MUnmap

  The system call deletes the mappings for the specified address range,
  and causes further references to addresses within the range to generate
  invalid memory references.
  
  Parameters:
  Addr       - TBD
  Size       - TBD

  Returns:
  On success returns 0, on failure -1
*/

int32_t eeRomFs_MUnmap(void *addr, uint32_t Size);


/***********************************************************
  Function: eeRomFs_Init

  Init the file system.

  Parameters:
  Pdata      - Pointer to a file system strutcure

  Returns:
  ==0        - OK
  !=0        - NG
*/

int  eeRomFs_Init(const eeRomFs_Data_t *PData);

int  eeRomFs_DeInit(void);

#ifdef __cplusplus
}
#endif

#endif /* _FS_H_ */
