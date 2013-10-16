#include "sse.h"
#include <stdint.h>             // ffsl
#include <string.h>
#include "tap.h"

struct { unsigned nbits; uint64_t lo, hi; } shltestv[] = {
    {  0, 0x0123456789ABCDEFULL, 0xFEDCBA9876543210ULL },
    {  1, 0x02468ACF13579BDEULL, 0xFDB97530ECA86420ULL },
    {  2, 0x048D159E26AF37BCULL, 0xFB72EA61D950C840ULL },
    {  4, 0x123456789ABCDEF0ULL, 0xEDCBA98765432100ULL },
    {  7, 0x91A2B3C4D5E6F780ULL, 0x6E5D4C3B2A190800ULL },
    {  8, 0x23456789ABCDEF00ULL, 0xDCBA987654321001ULL },
};
int     nshltests = sizeof(shltestv)/sizeof(*shltestv);

int main(void)
{
    int i;
    char inps[99], acts[99], exps[99];

    setvbuf(stdout, NULL, _IOLBF, 0);
    plan_tests(1 + nshltests + 5 + 128);

    XMM ones = { -1, -1 }; //_mm_set_epi64x(-1, -1);
    xm_hex(xm_ones(), acts);
    ok(xm_same(xm_ones(), ones), "xm_ones produces: %s", acts);

    XMM inp = { shltestv[0].lo, shltestv[0].hi };
    printf("# inp = %s\n", xm_hex(inp, inps));
    printf("# inp = %s\n", xm_str(inp, inps));

    for (i = 0; i < nshltests; ++i) {
        XMM exp = { shltestv[i].lo, shltestv[i].hi };
        XMM act = xm_shl(inp, shltestv[i].nbits);
        int rc = xm_same(act, exp);
        ok(rc == 0xFFFF, "shl inp,%d: %s exp: %s", shltestv[i].nbits,
            xm_hex(act, acts), xm_hex(exp, exps));
    }       

    XMM bitz = { 1,8 };
    int iact = xm_ffs(bitz);
    ok(iact == 0, "xm_ffs(%s) = %d", xm_str(bitz, inps), iact);
    iact = xm_fls(bitz);
    ok(iact == 67, "xm_fls(%s) = %d", inps, iact);
    
    bitz = _mm_setzero_si128();
    iact = xm_ffs(bitz);
    ok(iact == -1, "xm_ffs(zero) = %d", iact);
    iact = xm_fls(bitz);
    ok(iact == -1, "xm_fls(zero) = %d", iact);

    XMM stuff = _mm_set_epi64x(0x07BB01426C62272EULL, 0x6295C58D62B82175ULL);
    char    str[48];
    xm_str(stuff, str);
    is_strncmp(str, "75,21,B8,62,8D,C5,95,62-2E,27,62,6C,42,01,BB,07", 48, "xm_str");

    XMM one = { 1, 0 }, hibit = xm_shl_177(one);
    for (i = 0; i < 128; ++i) {
        int pos = xm_ffs(xm_or(xm_shl(one, i), hibit));
        ok(pos == i, "xm_ffs(xm_shl(one,%d)) = %d", i, pos);
    }

    return exit_status();
}
