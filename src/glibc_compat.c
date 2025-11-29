// GLIBC compatibility wrapper for older versions
// Provides __isoc23_strtol for systems with GLIBC < 2.38

#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

// Manual implementation to avoid recursion
// Based on standard strtol behavior
long __isoc23_strtol(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    long acc = 0;
    int c;
    int neg = 0, any;
    long cutoff, cutlim;

    // Skip whitespace
    do {
        c = *s++;
    } while (isspace((unsigned char)c));

    // Handle sign
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }

    // Auto-detect base
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }
    if (base == 0)
        base = c == '0' ? 8 : 10;

    // Compute cutoff values
    cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
    cutlim = cutoff % (unsigned long)base;
    cutoff /= (unsigned long)base;

    // Convert digits
    for (acc = 0, any = 0;; c = *s++) {
        if (isdigit((unsigned char)c))
            c -= '0';
        else if (isalpha((unsigned char)c))
            c -= isupper((unsigned char)c) ? 'A' - 10 : 'a' - 10;
        else
            break;
        if (c >= base)
            break;
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
            any = -1;
        else {
            any = 1;
            acc = acc * base + c;
        }
    }

    if (any < 0) {
        acc = neg ? LONG_MIN : LONG_MAX;
        errno = ERANGE;
    } else if (neg)
        acc = -acc;

    if (endptr != NULL)
        *endptr = (char *)(any ? s - 1 : nptr);

    return acc;
}

// Simpler wrappers for the other functions
long long __isoc23_strtoll(const char *nptr, char **endptr, int base) {
    // For simplicity, use strtol and extend
    return (long long)__isoc23_strtol(nptr, endptr, base);
}

unsigned long __isoc23_strtoul(const char *nptr, char **endptr, int base) {
    return (unsigned long)__isoc23_strtol(nptr, endptr, base);
}

unsigned long long __isoc23_strtoull(const char *nptr, char **endptr, int base) {
    return (unsigned long long)__isoc23_strtol(nptr, endptr, base);
}
