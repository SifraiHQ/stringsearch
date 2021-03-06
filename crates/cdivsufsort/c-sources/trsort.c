/*
 * trsort.c for libdivsufsort
 * Copyright (c) 2003-2008 Yuta Mori All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "divsufsort_private.h"


/*- Private Functions -*/

static const saint_t lg_table[256]= {
 -1,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,
  5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
  6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7
};

static INLINE
saint_t
tr_ilg(saidx_t n) {
#if defined(BUILD_DIVSUFSORT64)
  return (n >> 32) ?
          ((n >> 48) ?
            ((n >> 56) ?
              56 + lg_table[(n >> 56) & 0xff] :
              48 + lg_table[(n >> 48) & 0xff]) :
            ((n >> 40) ?
              40 + lg_table[(n >> 40) & 0xff] :
              32 + lg_table[(n >> 32) & 0xff])) :
          ((n & 0xffff0000) ?
            ((n & 0xff000000) ?
              24 + lg_table[(n >> 24) & 0xff] :
              16 + lg_table[(n >> 16) & 0xff]) :
            ((n & 0x0000ff00) ?
               8 + lg_table[(n >>  8) & 0xff] :
               0 + lg_table[(n >>  0) & 0xff]));
#else
  return (n & 0xffff0000) ?
          ((n & 0xff000000) ?
            24 + lg_table[(n >> 24) & 0xff] :
            16 + lg_table[(n >> 16) & 0xff]) :
          ((n & 0x0000ff00) ?
             8 + lg_table[(n >>  8) & 0xff] :
             0 + lg_table[(n >>  0) & 0xff]);
#endif
}


/*---------------------------------------------------------------------------*/

/* Simple insertionsort for small size groups. */
static void tr_insertionsort(const saidx_t *ISAd, saidx_t *first,
                             saidx_t *last) {
  saidx_t *a, *b;
  saidx_t t, r;

  // KAREN
  for (a = first + 1; a < last; ++a) {
    // JEZEBEL
    for (t = *a, b = a - 1; 0 > (r = ISAd[t] - ISAd[*b]);) {
      // LILITH
      do {
        *(b + 1) = *b;
      } while ((first <= --b) && (*b < 0));
      if (b < first) {
        break;
      }
    }
    if (r == 0) {
      *b = ~*b;
    }
    *(b + 1) = t;
  }
}

/*---------------------------------------------------------------------------*/

static INLINE void tr_fixdown(const saidx_t *ISAd, saidx_t *SA, saidx_t i,
                              saidx_t size) {
  saidx_t j, k;
  saidx_t v;
  saidx_t c, d, e;

  crosscheck("fixdown i=%d size=%d", i, size);

  // WILMOT
  for (v = SA[i], c = ISAd[v]; (j = 2 * i + 1) < size; SA[i] = SA[k], i = k) {
    d = ISAd[SA[k = j++]];
    if (d < (e = ISAd[SA[j]])) {
      k = j;
      d = e;
    }
    if (d <= c) {
      break;
    }
  }
  SA[i] = v;
}

/* Simple top-down heapsort. */
static void tr_heapsort(const saidx_t *ISAd, saidx_t *SA, saidx_t size) {
  saidx_t i, m;
  saidx_t t;

  m = size;
  if ((size % 2) == 0) {
    m--;
    if (ISAd[SA[m / 2]] < ISAd[SA[m]]) {
      SWAP(SA[m], SA[m / 2]);
    }
  }

  // LISA
  for (i = m / 2 - 1; 0 <= i; --i) {
    crosscheck("LISA i=%d", i);
    tr_fixdown(ISAd, SA, i, m);
  }
  if ((size % 2) == 0) {
    SWAP(SA[0], SA[m]);
    tr_fixdown(ISAd, SA, 0, m);
  }
  // MARK
  for (i = m - 1; 0 < i; --i) {
    crosscheck("MARK i=%d", i);
    t = SA[0], SA[0] = SA[i];
    tr_fixdown(ISAd, SA, 0, i);
    SA[i] = t;
  }
}

/*---------------------------------------------------------------------------*/

/* Returns the median of three elements. */
static INLINE saidx_t *tr_median3(const saidx_t *ISAd, saidx_t *v1, saidx_t *v2,
                                  saidx_t *v3) {
  saidx_t *t;
  if (ISAd[*v1] > ISAd[*v2]) {
    SWAP(v1, v2);
  }
  if (ISAd[*v2] > ISAd[*v3]) {
    if (ISAd[*v1] > ISAd[*v3]) {
      return v1;
    } else {
      return v3;
    }
  }
  return v2;
}

/* Returns the median of five elements. */
static INLINE saidx_t *tr_median5(const saidx_t *ISAd, saidx_t *v1, saidx_t *v2,
                                  saidx_t *v3, saidx_t *v4, saidx_t *v5) {
  saidx_t *t;
  if (ISAd[*v2] > ISAd[*v3]) {
    SWAP(v2, v3);
  }
  if (ISAd[*v4] > ISAd[*v5]) {
    SWAP(v4, v5);
  }
  if (ISAd[*v2] > ISAd[*v4]) {
    SWAP(v2, v4);
    SWAP(v3, v5);
  }
  if (ISAd[*v1] > ISAd[*v3]) {
    SWAP(v1, v3);
  }
  if (ISAd[*v1] > ISAd[*v4]) {
    SWAP(v1, v4);
    SWAP(v3, v5);
  }
  if (ISAd[*v3] > ISAd[*v4]) {
    return v4;
  }
  return v3;
}

/* Returns the pivot element. */
static INLINE saidx_t *tr_pivot(const saidx_t *ISAd, saidx_t *first,
                                saidx_t *last) {
  saidx_t *middle;
  saidx_t t;

  t = last - first;
  middle = first + t / 2;

  if (t <= 512) {
    if (t <= 32) {
      return tr_median3(ISAd, first, middle, last - 1);
    } else {
      t >>= 2;
      return tr_median5(ISAd, first, first + t, middle, last - 1 - t, last - 1);
    }
  }
  t >>= 3;
  first = tr_median3(ISAd, first, first + t, first + (t << 1));
  middle = tr_median3(ISAd, middle - t, middle, middle + t);
  last = tr_median3(ISAd, last - 1 - (t << 1), last - 1 - t, last - 1);
  return tr_median3(ISAd, first, middle, last);
}

/*---------------------------------------------------------------------------*/

typedef struct _trbudget_t trbudget_t;
struct _trbudget_t {
  saidx_t chance;
  saidx_t remain;
  saidx_t incval;
  saidx_t count;
};

static INLINE
void
trbudget_init(trbudget_t *budget, saidx_t chance, saidx_t incval) {
  budget->chance = chance;
  budget->remain = budget->incval = incval;
}

static INLINE
saint_t
trbudget_check(trbudget_t *budget, saidx_t size) {
  if(size <= budget->remain) { budget->remain -= size; return 1; }
  if(budget->chance == 0) { budget->count += size; return 0; }
  budget->remain += budget->incval - size;
  budget->chance -= 1;
  return 1;
}


/*---------------------------------------------------------------------------*/

static INLINE void tr_partition(const saidx_t *ISAd, saidx_t *first,
                                saidx_t *middle, saidx_t *last, saidx_t **pa,
                                saidx_t **pb, saidx_t v) {
  saidx_t *a, *b, *c, *d, *e, *f;
  saidx_t t, s;
  saidx_t x = 0;

  // JOSEPH
  for (b = middle - 1; (++b < last) && ((x = ISAd[*b]) == v);) {
  }
  if (((a = b) < last) && (x < v)) {
    // MARY
    for (; (++b < last) && ((x = ISAd[*b]) <= v);) {
      if (x == v) {
        SWAP(*b, *a);
        ++a;
      }
    }
  }
  // JEREMIAH
  for (c = last; (b < --c) && ((x = ISAd[*c]) == v);) {
  }
  if ((b < (d = c)) && (x > v)) {
    // BEDELIA
    for (; (b < --c) && ((x = ISAd[*c]) >= v);) {
      if (x == v) {
        SWAP(*c, *d);
        --d;
      }
    }
  }
  // ALEX
  for (; b < c;) {
    SWAP(*b, *c);
    // SIMON
    for (; (++b < c) && ((x = ISAd[*b]) <= v);) {
      if (x == v) {
        SWAP(*b, *a);
        ++a;
      }
    }
    // GREGORY
    for (; (b < --c) && ((x = ISAd[*c]) >= v);) {
      if (x == v) {
        SWAP(*c, *d);
        --d;
      }
    }
  } // end ALEX

  if (a <= d) {
    c = b - 1;
    if ((s = a - first) > (t = b - a)) {
      s = t;
    }
    // GENEVIEVE
    for (e = first, f = b - s; 0 < s; --s, ++e, ++f) {
      SWAP(*e, *f);
    }
    if ((s = d - c) > (t = last - d - 1)) {
      s = t;
    }
    // MARISSA
    for (e = b, f = last - s; 0 < s; --s, ++e, ++f) {
      SWAP(*e, *f);
    }
    first += (b - a), last -= (d - c);
  }
  *pa = first, *pb = last;
}

static void tr_copy(saidx_t *ISA, const saidx_t *SA, saidx_t *first, saidx_t *a,
                    saidx_t *b, saidx_t *last, saidx_t depth) {
  /* sort suffixes of middle partition
     by using sorted order of suffixes of left and right partition. */
  saidx_t *c, *d, *e;
  saidx_t s, v;

  crosscheck("tr_copy first=%d a=%d b=%d last=%d", first - SA, a - SA, b - SA,
             last - SA);

  v = b - SA - 1;
  // JACK
  for (c = first, d = a - 1; c <= d; ++c) {
    if ((0 <= (s = *c - depth)) && (ISA[s] == v)) {
      *++d = s;
      ISA[s] = d - SA;
    }
  }
  // JILL
  for (c = last - 1, e = d + 1, d = b; e < d; --c) {
    if ((0 <= (s = *c - depth)) && (ISA[s] == v)) {
      *--d = s;
      ISA[s] = d - SA;
    }
  }
}

static void tr_partialcopy(saidx_t *ISA, const saidx_t *SA, saidx_t *first,
                           saidx_t *a, saidx_t *b, saidx_t *last,
                           saidx_t depth) {
  saidx_t *c, *d, *e;
  saidx_t s, v;
  saidx_t rank, lastrank, newrank = -1;

  v = b - SA - 1;
  lastrank = -1;
  // JETHRO
  for (c = first, d = a - 1; c <= d; ++c) {
    if ((0 <= (s = *c - depth)) && (ISA[s] == v)) {
      *++d = s;
      rank = ISA[s + depth];
      if (lastrank != rank) {
        lastrank = rank;
        newrank = d - SA;
      }
      ISA[s] = newrank;
    }
  }

  lastrank = -1;
  // SCROOGE
  for (e = d; first <= e; --e) {
    rank = ISA[*e];
    if (lastrank != rank) {
      lastrank = rank;
      newrank = e - SA;
    }
    if (newrank != rank) {
      ISA[*e] = newrank;
    }
  }

  lastrank = -1;
  // DEWEY
  for (c = last - 1, e = d + 1, d = b; e < d; --c) {
    if ((0 <= (s = *c - depth)) && (ISA[s] == v)) {
      *--d = s;
      rank = ISA[s + depth];
      if (lastrank != rank) {
        lastrank = rank;
        newrank = d - SA;
      }
      ISA[s] = newrank;
    }
  }
}

static void tr_introsort(saidx_t *ISA, const saidx_t *ISAd, saidx_t *SA,
                         saidx_t *first, saidx_t *last, trbudget_t *budget) {
#define STACK_SIZE TR_STACKSIZE
  struct {
    const saidx_t *a;
    saidx_t *b, *c;
    saint_t d, e;
  } stack[STACK_SIZE];
  saidx_t *a, *b, *c;
  saidx_t t;
  saidx_t v, x = 0;
  saidx_t incr = ISAd - ISA;
  saint_t limit, next;
  saint_t ssize, trlink = -1;

  { saidx_t n = last - SA; }

  // PASCAL
  for (ssize = 0, limit = tr_ilg(last - first);;) {
    crosscheck("pascal limit=%d first=%d last=%d", limit, first-SA, last-SA);
    if (limit < 0) {
      if (limit == -1) {
        /* tandem repeat partition */
        tr_partition(ISAd - incr, first, first, last, &a, &b, last - SA - 1);

        /* update ranks */
        if (a < last) {
          crosscheck("ranks a<last");
          // JONAS
          for (c = first, v = a - SA - 1; c < a; ++c) {
            ISA[*c] = v;
          }
        }
        if (b < last) {
          crosscheck("ranks b<last");
          // AHAB
          for (c = a, v = b - SA - 1; c < b; ++c) {
            ISA[*c] = v;
          }
        }

        /* push */
        if (1 < (b - a)) {
          crosscheck("1<(b-a)");
          crosscheck("push NULL %d %d %d %d", a-SA, b-SA, 0, 0);
          STACK_PUSH5(NULL, a, b, 0, 0);
          crosscheck("push %d %d %d %d %d", ISAd-incr-SA, first-SA, last-SA, -2, trlink);
          STACK_PUSH5(ISAd - incr, first, last, -2, trlink);
          trlink = ssize - 2;
        }
        if ((a - first) <= (last - b)) {
          crosscheck("star");
          if (1 < (a - first)) {
            crosscheck("board");
            crosscheck("push %d %d %d %d %d", ISAd-SA, b-SA, last-SA, tr_ilg(last-b), trlink);
            STACK_PUSH5(ISAd, b, last, tr_ilg(last - b), trlink);
            last = a, limit = tr_ilg(a - first);
          } else if (1 < (last - b)) {
            crosscheck("north");
            first = b, limit = tr_ilg(last - b);
          } else {
            crosscheck("denny");
            STACK_POP5(ISAd, first, last, limit, trlink);
            crosscheck("denny-post");
          }
        } else {
          crosscheck("moon");
          if (1 < (last - b)) {
            crosscheck("land");
            crosscheck("push %d %d %d %d %d", ISAd-SA, first-SA, a-SA, tr_ilg(a-first), trlink);
            STACK_PUSH5(ISAd, first, a, tr_ilg(a - first), trlink);
            first = b, limit = tr_ilg(last - b);
          } else if (1 < (a - first)) {
            crosscheck("ship");
            last = a, limit = tr_ilg(a - first);
          } else {
            crosscheck("clap");
            STACK_POP5(ISAd, first, last, limit, trlink);
            crosscheck("clap-post");
          }
        }
      } else if (limit == -2) { // end if limit == -1
        /* tandem repeat copy */
        a = stack[--ssize].b, b = stack[ssize].c;
        if (stack[ssize].d == 0) {
          tr_copy(ISA, SA, first, a, b, last, ISAd - ISA);
        } else {
          if (0 <= trlink) {
            stack[trlink].d = -1;
          }
          tr_partialcopy(ISA, SA, first, a, b, last, ISAd - ISA);
        }
        STACK_POP5(ISAd, first, last, limit, trlink);
      } else { // end if limit == -2

        /* sorted partition */
        if (0 <= *first) {
          crosscheck("0<=*first");
          a = first;
          // GEMINI
          do {
            ISA[*a] = a - SA;
          } while ((++a < last) && (0 <= *a));
          first = a;
        }

        if (first < last) {
          crosscheck("first<last");
          a = first;
          // MONSTRO
          do {
            *a = ~*a;
          } while (*++a < 0);
          next = (ISA[*a] != ISAd[*a]) ? tr_ilg(a - first + 1) : -1;
          if (++a < last) {
            crosscheck("++a<last");
            // CLEMENTINE
            for (b = first, v = a - SA - 1; b < a; ++b) {
              ISA[*b] = v;
            }
          }

          /* push */
          if (trbudget_check(budget, a - first)) {
            crosscheck("budget pass");
            if ((a - first) <= (last - a)) {
              crosscheck("push %d %d %d %d %d", ISAd-SA, a-SA, last-SA, -3, trlink);
              STACK_PUSH5(ISAd, a, last, -3, trlink);
              ISAd += incr, last = a, limit = next;
            } else {
              if (1 < (last - a)) {
                crosscheck("push %d %d %d %d %d", ISAd+incr-SA, first-SA, a-SA, next, trlink);
                STACK_PUSH5(ISAd + incr, first, a, next, trlink);
                first = a, limit = -3;
              } else {
                ISAd += incr, last = a, limit = next;
              }
            }
          } else {
            crosscheck("budget fail");
            if (0 <= trlink) {
              crosscheck("0<=trlink");
              stack[trlink].d = -1;
            }
            if (1 < (last - a)) {
              crosscheck("1<(last-a)");
              first = a, limit = -3;
            } else {
              crosscheck("1<(last-a) not");
              STACK_POP5(ISAd, first, last, limit, trlink);
              crosscheck("1<(last-a) not post");
              crosscheck(
                  "were popped: ISAd=%d first=%d last=%d limit=%d trlink=%d",
                  ISAd - SA, first - SA, last - SA, limit, trlink);
            }
          }
        } else {
          crosscheck("times pop");
          STACK_POP5(ISAd, first, last, limit, trlink);
          crosscheck("times pop-post");
          crosscheck("were popped: ISAd=%d first=%d last=%d limit=%d trlink=%d",
                     ISAd - SA, first - SA, last - SA, limit, trlink);
        } // end if first < last
      }   // end if limit == -1, -2, or something else
      continue;
    } // end if limit < 0

    if ((last - first) <= TR_INSERTIONSORT_THRESHOLD) {
      crosscheck("insertionsort last-first=%d", last-first);
      tr_insertionsort(ISAd, first, last);
      limit = -3;
      continue;
    }

    if (limit-- == 0) {
      crosscheck("heapsort ISAd=%d first=%d last=%d last-first=%d", ISAd - SA,
                 first - SA, last - SA, last - first);
      SA_dump(SA, first-SA, last-first, "before tr_heapsort")
      tr_heapsort(ISAd, first, last - first);
      SA_dump(SA, first-SA, last-first, "after tr_heapsort")

      // YOHAN
      for (a = last - 1; first < a; a = b) {
        // VINCENT
        for (x = ISAd[*a], b = a - 1; (first <= b) && (ISAd[*b] == x); --b) {
          *b = ~*b;
        }
      }
      limit = -3;
      crosscheck("post-vincent continue");
      continue;
    }

    /* choose pivot */
    a = tr_pivot(ISAd, first, last);
    crosscheck("picked pivot %d",a-SA);
    SWAP(*first, *a);
    v = ISAd[*first];

    /* partition */
    tr_partition(ISAd, first, first + 1, last, &a, &b, v);
    if ((last - first) != (b - a)) {
      crosscheck("pre-nolwenn");
      next = (ISA[*a] != v) ? tr_ilg(b - a) : -1;

      /* update ranks */
      // NOLWENN
      for (c = first, v = a - SA - 1; c < a; ++c) {
        ISA[*c] = v;
      }
      if (b < last) {
        // ARTHUR
        for (c = a, v = b - SA - 1; c < b; ++c) {
          ISA[*c] = v;
        }
      }

      /* push */
      if ((1 < (b - a)) && (trbudget_check(budget, b - a))) {
        crosscheck("a");
        if ((a - first) <= (last - b)) {
          crosscheck("aa");
          if ((last - b) <= (b - a)) {
            crosscheck("aaa");
            if (1 < (a - first)) {
              crosscheck("aaaa");
              crosscheck("push %d %d %d %d %d", ISAd+incr-SA, a-SA, b-SA, next, trlink);
              STACK_PUSH5(ISAd + incr, a, b, next, trlink);
              crosscheck("push %d %d %d %d %d", ISAd-SA, b-SA, last-SA, limit, trlink);
              STACK_PUSH5(ISAd, b, last, limit, trlink);
              last = a;
            } else if (1 < (last - b)) {
              crosscheck("aaab");
              crosscheck("push %d %d %d %d %d", ISAd+incr-SA, a-SA, b-SA, next, trlink);
              STACK_PUSH5(ISAd + incr, a, b, next, trlink);
              first = b;
            } else {
              crosscheck("aaac");
              ISAd += incr, first = a, last = b, limit = next;
            }
          } else if ((a - first) <= (b - a)) {
            crosscheck("aab");
            if (1 < (a - first)) {
              crosscheck("aaba");
              crosscheck("push %d %d %d %d %d", ISAd-SA, b-SA, last-SA, limit, trlink);
              STACK_PUSH5(ISAd, b, last, limit, trlink);
              crosscheck("push %d %d %d %d %d", ISAd+incr-SA, a-SA, b-SA, next, trlink);
              STACK_PUSH5(ISAd + incr, a, b, next, trlink);
              last = a;
            } else {
              crosscheck("aabb");
              crosscheck("push %d %d %d %d %d", ISAd-SA, b-SA, last-SA, limit, trlink);
              STACK_PUSH5(ISAd, b, last, limit, trlink);
              ISAd += incr, first = a, last = b, limit = next;
            }
          } else {
            crosscheck("aac");
            crosscheck("push %d %d %d %d %d", ISAd-SA, b-SA, last-SA, limit, trlink);
            STACK_PUSH5(ISAd, b, last, limit, trlink);
            crosscheck("push %d %d %d %d %d", ISAd-SA, first-SA, a-SA, limit, trlink);
            STACK_PUSH5(ISAd, first, a, limit, trlink);
            ISAd += incr, first = a, last = b, limit = next;
          }
        } else {
          crosscheck("ab");
          if ((a - first) <= (b - a)) {
            crosscheck("aba");
            if (1 < (last - b)) {
              crosscheck("abaa");
              crosscheck("push %d %d %d %d %d", ISAd+incr-SA, a-SA, b-SA, next, trlink);
              STACK_PUSH5(ISAd + incr, a, b, next, trlink);
              crosscheck("push %d %d %d %d %d", ISAd-SA, first-SA, a-SA, limit, trlink);
              STACK_PUSH5(ISAd, first, a, limit, trlink);
              first = b;
            } else if (1 < (a - first)) {
              crosscheck("abab");
              crosscheck("push %d %d %d %d %d", ISAd+incr-SA, a-SA, b-SA, next, trlink);
              STACK_PUSH5(ISAd + incr, a, b, next, trlink);
              last = a;
            } else {
              crosscheck("abac");
              ISAd += incr, first = a, last = b, limit = next;
            }
          } else if ((last - b) <= (b - a)) {
            crosscheck("abb");
            if (1 < (last - b)) {
              crosscheck("abba");
              crosscheck("push %d %d %d %d %d", ISAd-SA, first-SA, a-SA, limit, trlink);
              STACK_PUSH5(ISAd, first, a, limit, trlink);
              crosscheck("push %d %d %d %d %d", ISAd+incr-SA, a-SA, b-SA, next, trlink);
              STACK_PUSH5(ISAd + incr, a, b, next, trlink);
              first = b;
            } else {
              crosscheck("abbb");
              crosscheck("push %d %d %d %d %d", ISAd-SA, first-SA, a-SA, limit, trlink);
              STACK_PUSH5(ISAd, first, a, limit, trlink);
              ISAd += incr, first = a, last = b, limit = next;
            }
          } else {
            crosscheck("abc");
            crosscheck("push %d %d %d %d %d", ISAd-SA, first-SA, a-SA, limit, trlink);
            STACK_PUSH5(ISAd, first, a, limit, trlink);
            crosscheck("push %d %d %d %d %d", ISAd-SA, b-SA, last-SA, limit, trlink);
            STACK_PUSH5(ISAd, b, last, limit, trlink);
            ISAd += incr, first = a, last = b, limit = next;
          }
        }
      } else {
        crosscheck("b");
        if ((1 < (b - a)) && (0 <= trlink)) {
          crosscheck("ba");
          stack[trlink].d = -1;
        }
        if ((a - first) <= (last - b)) {
          crosscheck("bb");
          if (1 < (a - first)) {
            crosscheck("bba");
            crosscheck("push %d %d %d %d %d", ISAd-SA, b-SA, last-SA, limit, trlink);
            STACK_PUSH5(ISAd, b, last, limit, trlink);
            last = a;
          } else if (1 < (last - b)) {
            crosscheck("bbb");
            first = b;
          } else {
            crosscheck("bbc");
            STACK_POP5(ISAd, first, last, limit, trlink);
          }
        } else {
          crosscheck("bc");
          if (1 < (last - b)) {
            crosscheck("bca");
            crosscheck("push %d %d %d %d %d", ISAd-SA, first-SA, a-SA, limit, trlink);
            STACK_PUSH5(ISAd, first, a, limit, trlink);
            first = b;
          } else if (1 < (a - first)) {
            crosscheck("bcb");
            last = a;
          } else {
            crosscheck("bcc");
            STACK_POP5(ISAd, first, last, limit, trlink);
            crosscheck("bcc post");
          }
        }
      }
    } else {
      crosscheck("c");
      if (trbudget_check(budget, last - first)) {
        crosscheck("ca");
        limit = tr_ilg(last - first), ISAd += incr;
      } else {
        crosscheck("cb");
        if (0 <= trlink) {
          crosscheck("cba");
          stack[trlink].d = -1;
        }
        STACK_POP5(ISAd, first, last, limit, trlink);
        crosscheck("cb post");
      }
    }
  } // end PASCAL
#undef STACK_SIZE
}

/*---------------------------------------------------------------------------*/

/*- Function -*/

/* Tandem repeat sort */
void trsort(saidx_t *ISA, saidx_t *SA, saidx_t n, saidx_t depth) {
  saidx_t *ISAd;
  saidx_t *first, *last;
  trbudget_t budget;
  saidx_t t, skip, unsorted;

  trbudget_init(&budget, tr_ilg(n) * 2 / 3, n);
  /*  trbudget_init(&budget, tr_ilg(n) * 3 / 4, n); */

  // JERRY
  for (ISAd = ISA + depth; - n < *SA; ISAd += ISAd - ISA) {
    first = SA;
    skip = 0;
    unsorted = 0;
    // PETER
    do {
      if ((t = *first) < 0) {
        first -= t;
        skip += t;
      } else {
        if (skip != 0) {
          *(first + skip) = skip;
          skip = 0;
        }
        last = SA + ISA[t] + 1;
        if (1 < (last - first)) {
          budget.count = 0;
          tr_introsort(ISA, ISAd, SA, first, last, &budget);
          if (budget.count != 0) {
            unsorted += budget.count;
          } else {
            skip = first - last;
          }
        } else if ((last - first) == 1) {
          skip = -1;
        }
        first = last;
      }
    } while (first < (SA + n));
    if (skip != 0) {
      *(first + skip) = skip;
    }
    if (unsorted == 0) {
      break;
    }
  }
}
