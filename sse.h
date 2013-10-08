// Copyright (C) 2009-2013 Mischa Sandberg <mischasan@gmail.com>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License Version 2 as
// published by the Free Software Foundation.  You may not use, modify or
// distribute this program under any other version of the GNU General
// Public License.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
// IF YOU ARE UNABLE TO WORK WITH GPL2, CONTACT ME.
//-------------------------------------------------------------------
#ifndef SSE_H
#define SSE_H

#if defined(__linux__)
#   define regargs __attribute__((fastcall))
#else
#   define regargs
#endif

// ssebndm: memmem, using SSE2 for patlen in [65..128]
char   *ssebndm(char *target, int tgtlen, char *pattern, int patlen);

// ssecmp: strcmp implemented using SSE2
int     ssecmp(char const *s, char const *t);

// ssestr: strstr implemented using SSE2 and ssechr2
char const *ssestr(char const *tgt, char const *pat);

// ssechr2: used by ssestr; useful as a special case.
char const * ssechr2(char const *tgt, char const pat[2]) regargs;

// ssesort16d: sort 16 doubles in-place:
void    ssesort16d(double keys[16]);

// sserank16d: generate ranking vector for keys[],
//  such that for the input value of keys[]:
//      keys[rank[i]] <= keys[rank[j]]  IFF  i <= j
//  and the sort is stable; i.e.:
//      rank[i] < rank[j] IFF i < j AND keys[rank[i]] == keys[rank[j]]
// "keys" is destroyed (sorted) as a side effect.
void    sserank16d(double keys[16], int rank[16]);

#endif //SSE_H
