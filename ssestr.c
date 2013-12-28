#include "sse.h"
#include <stdint.h>
#include <string.h>             // ffsl

static inline unsigned under(unsigned x)
{ return (x - 1) & ~x; }

int
ssecmp(char const *s, char const *t)
{
    XMM     xs, zero = {};
    unsigned m, z, f = 15 & (intptr_t) s;

    if (f) {
        z = xm_same(xs = xm_load(s - f), zero) >> f;
        m = under(z) & (xm_diff(xs, xm_loud(t - f)) >> f) & under(z);
        if (m)
            return m = ffsl(m) - 1, s[m] - t[m];
        if (z)
            return 0;
        s += 16 - f, t += 16 - f;
    }

    for (; !(z = xm_same(xs = xm_load(s), zero)); s += 16, t += 16)
        if ((m = xm_diff(xs, xm_loud(t))))
            return m = ffsl(m) - 1, s[m] - t[m];

    m = xm_diff(xm_load(s), xm_loud(t)) & under(z);
    return m ? m = ffsl(m) - 1, s[m] - t[m] : 0;
}

char const *
ssestr(char const *tgt, char const *pat)
{
    int     patlen = strlen(pat);

    if (patlen == 0)
        return tgt;
    if (patlen == 1)
        return strchr(tgt, *pat);
    
    char const *cp, *tp;
    for (; (tgt = ssechr2(tgt, pat)); ++tgt)
        for (cp = pat + 2, tp = tgt + 2;; ++cp, ++tp)
            if (!*cp) return tgt; else if (*cp != *tp) break;
    
    return NULL;
}

char const *
ssechr2(char const *tgt, char const pat[2])
{
    XMM const zero = _mm_setzero_si128();
    XMM const p0 = xm_fill(pat[0]);
    XMM const p1 = xm_fill(pat[1]);
    uint16_t pair = *(uint16_t const*)pat;
    unsigned f = 15 & (uintptr_t) tgt;

    tgt -= f;

    while (1) {
        XMM     x = xm_load(tgt);
        unsigned u = xm_same(x, zero) >> f;
        unsigned v =
            ~u & (u - 1) & ((xm_same(x, p0) & (xm_same(x, p1) >> 1)) >> f);
        if (v)
            return tgt + f + ffsl(v) - 1;
        if (u)
            return NULL;
        tgt += 16;
        if (*(uint16_t const*)(tgt - 1) == pair)
            return tgt - 1;
        f = 0;
    }
}
