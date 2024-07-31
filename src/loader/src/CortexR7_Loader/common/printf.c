/*
 * Copyright (c) 2015-2019, Renesas Electronics Corporation.
 * All rights reserved.
 */

#include "stdarg.h"
#include "stdint.h"
#include "debug.h"
#include "scif.h"

#define NULL_CHAR	'\0'

/***********************************************************
 * tf_printf
 ***********************************************************/

static uint32_t uint32_print(uint32_t num)
{
	uint8_t  num_buf[10];
	uint32_t count = 0U;
	uint32_t i = 0U;
	uint32_t rem;
	uint32_t unum = num;

	while (1) {
		rem = unum % 10U;
		num_buf[i] = (uint8_t)('0' + rem);

		i++;
		unum = unum / 10U;
		if (unum < 1U) {
			break;
		}
	}

	while (i != 0U) {
		i--;
		(void)console_putc(num_buf[i]);
		count++;
	};

	return count;
}


static uint32_t int32_print(int32_t num)
{
	uint32_t unum;
	uint32_t count = 0U;

	if (num < 0) {
		(void)console_putc((uint8_t)'-');
		count = 1U;
		unum = (uint32_t)-num;
	} else {
		unum = (uint32_t)num;
	}

	count += uint32_print(unum);

	return count;
}


static uint32_t uint32_hex_print(uint32_t num)
{
	uint32_t i;
	uint32_t count = 0U;
	uint8_t  c;

	for (i = 0U; i < 8U; i++) {
		/* 0-F */
		c = (uint8_t)((num >> ((7U - i) * 4U)) & 0x0FU);
		if (c >= 0x0AU) {
			/* A-F */
			c += (uint8_t)('a' - 0x0AU);
		} else {
			/* 0-9 */
			c += (uint8_t)'0';
		}
		(void)console_putc(c);
		count++;
	}

	return count;
}


static uint32_t str_print(const char *str)
{
	uint32_t count = 0;

	while (*str != NULL_CHAR) {
		(void)console_putc((uint8_t)*str);
		str++;
		count++;
	}

	return count;
}


void tf_printf(const char *fmt, ...)
{
	va_list  args;
	int32_t  num;
	uint32_t unum;
	char     *str;
	uint32_t count = 0U;

	va_start(args, fmt);
	while (*fmt != NULL_CHAR) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
			case 'i':
				/* No break */
			case 'd':
				num = va_arg(args, int32_t);
				count += int32_print(num);
				break;
			case 's':
				str = va_arg(args, char *);
				count += str_print(str);
				break;
			case 'x':
				unum = va_arg(args, uint32_t);
				count += uint32_hex_print(unum);
				break;
			case 'u':
				unum = va_arg(args, uint32_t);
				count += uint32_print(unum);
				break;
			default:
				break;
			}
		} else {
			(void)console_putc((uint8_t)*fmt);
			count++;
		}
		fmt++;
	}
	va_end(args);
}


/***********************************************************
 * sprintf
 ***********************************************************/

static uint32_t uint32_sprint(uint32_t num, char *buf)
{
	uint8_t  num_buf[10];
	uint32_t count = 0U;
	uint32_t i = 0U;
	uint32_t rem;
	uint32_t unum = num;

	while (1) {
		rem = unum % 10U;
		num_buf[i] = (uint8_t)('0' + rem);

		i++;
		unum = unum / 10U;
		if (unum < 1U) {
			break;
		}
	}

	while (i != 0U) {
		i--;
		buf[count] = (char)num_buf[i];
		count++;
	};

	return count;
}


static uint32_t int32_sprint(int32_t num, char *buf)
{
	uint32_t unum;
	uint32_t count = 0U;

	if (num < 0) {
		buf[count] = '-';
		count = 1U;
		unum = (uint32_t)-num;
	} else {
		unum = (uint32_t)num;
	}

	count += uint32_sprint(unum, &buf[count]);

	return count;
}


static uint32_t uint32_hex_sprint(uint32_t num, char *buf)
{
	uint32_t i;
	uint32_t count = 0U;
	uint8_t  c;

	for (i = 0U; i < 8U; i++) {
		/* 0-F */
		c = (uint8_t)((num >> ((7U - i) * 4U)) & 0x0FU);
		if (c >= 0x0AU) {
			/* A-F */
			c += (uint8_t)('a' - 0x0AU);
		} else {
			/* 0-9 */
			c += (uint8_t)'0';
		}
		buf[count] = (char)c;
		count++;
	}

	return count;
}


static uint32_t str_sprint(const char *str, char *buf)
{
	uint32_t count = 0;

	while (*str != NULL_CHAR) {
		buf[count] = *str;
		str++;
		count++;
	}

	return count;
}


int sprintf(char *buf, const char *fmt, ...)
{
	va_list  args;
	int32_t  num;
	uint32_t unum;
	char     *str;
	uint32_t count = 0;

	va_start(args, fmt);

	if (fmt == NULL) {
		fmt = "(fmt null)\n";
	}

	while (*fmt != NULL_CHAR) {
		if (*fmt == '%') {
			fmt++;
			switch (*fmt) {
			case 'i':
				/* No break */
			case 'd':
				num = va_arg(args, int32_t);
				count += int32_sprint(num, &buf[count]);
				break;
			case 's':
				str = va_arg(args, char *);
				count += str_sprint(str, &buf[count]);
				break;
			case 'x':
				unum = va_arg(args, uint32_t);
				count += uint32_hex_sprint(unum, &buf[count]);
				break;
			case 'u':
				unum = va_arg(args, uint32_t);
				count += uint32_sprint(unum, &buf[count]);
				break;
			default:
				break;
			}
		} else {
			buf[count] = *fmt;
			count++;
		}
		fmt++;
	}
	buf[count] = '\0';
	va_end(args);

	return (int)count;
}
