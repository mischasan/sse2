#include <tap.h>
#include "sse.h"
#include "bitmat.h"

void prbm(BITMAT const *);
void dotest(int nrows, int ncols, int *xys);

int main(void)
{
    plan_tests(5 * 2);
    setvbuf(stdout, 0, _IOLBF, 0);

    // Eleven data points inserted into each test matrix,
    //  modulo that matrix' dimensions.
    int pts[] = { 1,3,  2,5,  4,0,  8,9,  5,20,  6,23,  10,10,  
                    10,11, 10,12,  15,22,  13,23,  -1  };

    for (bitmat_msb_first = 0; bitmat_msb_first > -2; --bitmat_msb_first) {
        fprintf(stderr, "# %s SIGNIFICANT BIT FIRST\n",
                bitmat_msb_first ? "MOST" : "LEAST");
        dotest( 8,  8, pts);
        dotest( 8, 24, pts);
        dotest(16, 8, pts);
        dotest(16, 24, pts);
        dotest(24, 24, pts);
    }
    return exit_status();
}

void
dotest(int nrows, int ncols, int *xys)
{
    BITMAT *inp = bitmat(nrows, ncols);
    BITMAT *exp = bitmat(ncols, nrows);

    for (; *xys >= 0; xys += 2) {
        bitmat_set(inp, xys[0] % nrows, xys[1] % ncols, 1);
        bitmat_set(exp, xys[1] % ncols, xys[0] % nrows, 1);
    }

    puts("---- inp"); prbm(inp);
    puts("---- exp"); prbm(exp);

    BITMAT *act = bitmat_trans(inp);
    int rc = bitmat_cmp(act, exp);
    ok(!rc, "trans %d x %d", nrows, ncols);
    if (rc) {
        puts("actual:");
        prbm(act);
    }

    bitmat_destroy(inp);
    bitmat_destroy(exp);
    bitmat_destroy(act);
}

void
prbm(BITMAT const *bmp)
{
    int r, c, nrows = bitmat_rows(bmp), ncols = bitmat_cols(bmp);

    printf("col>");
    for (c = 0; c < ncols; ++c) {
        if (!(c & 7)) putchar(' ');
        putchar('0' + c % 10);
    }
    putchar('\n');

    for (r = 0; r < nrows; ++r) {
        printf("%3d ", r);
        for (c = 0; c < ncols; ++c) {
            if (!(c & 7)) putchar(' ');
            putchar("-#"[bitmat_get(bmp, r, c)]);
        }

        // HEX format:
        char const *data = bitmat_rowval(bmp, r);
        for (c = 0; c < ncols; c += 8)
            printf(" %2.2X", 255 & data[c/8]);
        putchar('\n');
    }
}

