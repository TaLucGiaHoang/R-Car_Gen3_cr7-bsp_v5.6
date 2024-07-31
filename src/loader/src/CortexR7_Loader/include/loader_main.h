/*
 * Copyright (c) 2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#ifndef	LOADER_MAIN_H__
#define	LOADER_MAIN_H__

/* Build message is fixed to 30 characters (include '\0'). */
extern const char build_message[30];

uint32_t loader_main(void);

#endif	/* LOADER_MAIN_H__ */
