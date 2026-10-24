/* util.h - small helpers shared by every module */
#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

void *xmalloc(size_t size);                          /* malloc that exits on failure   */
void copy_str(char *dst, size_t size, const char *src); /* safe string copy             */
int  ci_compare(const char *a, const char *b);       /* case-insensitive strcmp        */
int  ci_contains(const char *text, const char *part);/* case-insensitive substring     */

/* Dates are stored as "day numbers" (days since 1970-01-01, local calendar date). */
long today_day(void);
void format_day(long day, char out[16]);             /* writes YYYY-MM-DD              */

#endif
