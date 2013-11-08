#ifndef BITMAT_H
#define BITMAT_H

typedef struct bitmat BITMAT;

// If msb_first is 0 (default), index bits in a byte as 76543210
int     bitmat_msb_first; 

BITMAT *bitmat(int nrows, int ncols);
void    bitmat_destroy(BITMAT *);

// Retrieve bitmat dimensions
int     bitmat_rows(BITMAT const *);
int     bitmat_cols(BITMAT const *);

// Get/Set a bit
int     bitmat_get(BITMAT const *, int row, int col);
void    bitmat_set(BITMAT *, int row, int col, int val);

// bitmat_cmp: returns:
//  0 if bitmats are identical (both may be NULL)
//  1 if different values
// -1 if incompatible
int     bitmat_cmp(BITMAT const *, BITMAT const*);

// bitmat_rowval: return ptr to raw bytes of one row.
char const *bitmat_rowval(BITMAT const *, int row);

// Transpose rows/columns
BITMAT *bitmat_trans(BITMAT const *);

#endif//BITMAT_H
