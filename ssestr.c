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
// scanstr puts the memcmp(pat+2,*) INSIDE the loop.
/// You can also use a loop that calls scanstr2 then memcmp(pat+2,*) outside the call.
//XXX for patterns >32 bytes, you may be able to step forward by a
// multiple of 16 bytes, instead of just "tgt += 16".
char const*
ssestr(char const* tgt, char const* pat)
{
    if (!pat[0]) return tgt;
    if (!pat[1]) return strchr(tgt, *pat);
    XMM zero = {}, xt;
    XMM xp0 = xm_fill(pat[0]);
    XMM xp1 = xm_fill(pat[1]);
    unsigned m = 15 & (intptr_t)tgt, patlen = strlen(pat);
    unsigned mz = (-1 << m) & xm_same(zero, xt = xm_load(tgt -= m));
    unsigned m0 = (-1 << m) & xm_same(xp0, xt);
    char const *p;
    while (!mz) {
        if (m0) {
            unsigned m1 = xm_same(xp1, xt);
            m0 &= (m1 >> 1) | (tgt[16] == pat[1] ? 0x8000 : 0);
            for (m = m0; m; m &= m - 1) {
                int pos = ffs(m) - 1;
                if (!intcmp(pat+2, tgt+pos+2, patlen-2)) return tgt+pos;
            }
        }
        mz = xm_same(zero, xt = xm_load(tgt += 16));
        m0 = xm_same(xp0, xt);
    }
    if ((m0 &= under(mz))) {
        m0 &= (xm_same(xp1, xt) >> 1);
        for (m = m0; m; m &= m - 1) {
            p = tgt + ffs(m) - 1;
            if (!intcmp(pat+2, p+2, patlen-2))
                return p;
        }
    }
    return NULL;
}

char const *
ssestr2(char const *tgt, char const *pat)
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
