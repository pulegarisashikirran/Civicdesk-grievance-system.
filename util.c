#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util.h"

void *xmalloc(size_t size)
{
    void *p = malloc(size > 0 ? size : 1);
    if (p == NULL) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    return p;
}

void copy_str(char *dst, size_t size, const char *src)
{
    size_t i = 0;
    while (i + 1 < size && src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

int ci_compare(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        int x = tolower((unsigned char)*a);
        int y = tolower((unsigned char)*b);
        if (x != y)
            return x - y;
        a++;
        b++;
    }
    return tolower((unsigned char)*a) - tolower((unsigned char)*b);
}

int ci_contains(const char *text, const char *part)
{
    size_t n = 0;
    while (part[n] != '\0')
        n++;
    if (n == 0)
        return 1;
    for (; *text != '\0'; text++) {
        size_t i = 0;
        while (i < n && text[i] != '\0' &&
               tolower((unsigned char)text[i]) == tolower((unsigned char)part[i]))
            i++;
        if (i == n)
            return 1;
    }
    return 0;
}

/* Convert a calendar date to a day number (days since 1970-01-01). */
static long days_from_civil(int y, int m, int d)
{
    long era, yoe, doy, doe;
    if (m <= 2)
        y -= 1;
    era = (y >= 0 ? y : y - 399) / 400;
    yoe = y - era * 400;
    doy = (153 * (m > 2 ? m - 3 : m + 9) + 2) / 5 + d - 1;
    doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

/* Convert a day number back to year / month / day. */
static void civil_from_days(long z, int *y, int *m, int *d)
{
    long era, doe, yoe, doy, mp;
    z += 719468;
    era = (z >= 0 ? z : z - 146096) / 146097;
    doe = z - era * 146097;
    yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    mp = (5 * doy + 2) / 153;
    *d = (int)(doy - (153 * mp + 2) / 5 + 1);
    *m = (int)(mp < 10 ? mp + 3 : mp - 9);
    *y = (int)(yoe + era * 400 + (*m <= 2 ? 1 : 0));
}

long today_day(void)
{
    /* localtime() is slow, and sorting asks for the date millions of times,
     * so the day number is cached and refreshed at most every 30 seconds. */
    static time_t last_check = 0;
    static long cached_day = 0;
    time_t now = time(NULL);

    if (last_check == 0 || now - last_check >= 30) {
        struct tm *t = localtime(&now);
        cached_day = days_from_civil(t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
        last_check = now;
    }
    return cached_day;
}

void format_day(long day, char out[16])
{
    int y, m, d;
    civil_from_days(day, &y, &m, &d);
    snprintf(out, 16, "%04d-%02d-%02d", y, m, d);
}
