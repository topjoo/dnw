//-------------------------
// checksum
#define STRICT
#define WIN32_LEAN_AND_MEAN

#include "resource.h"

#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <memory.h> 

#include "md5.h"
#include "dnw.h"



void EB_Printf(const TCHAR *fmt,...); //engine.h


#ifdef __cplusplus
extern "C"
{
#endif



#define MD5_CHECKSUM_ENCIPHER 		1
#define POLARSSL_SHA1_ENCIPHER 		1
#define SHA2_256_384_512 			1
#define MD6_CHECKSUM_ENCIPHER 		1
#define SHA2_224_CHECKSUM 			1





#if MD6_CHECKSUM_ENCIPHER

/* GCC */
/// #include <stdint.h>
#if 0
inline uint64_t ticks() 
{
	/* read timestamp counter */
	uint32_t lo, hi;
	asm volatile (
		"xorl %%eax,%%eax \n        cpuid"
		::: "%rax", "%rbx", "%rcx", "%rdx");
	asm volatile ("rdtsc" : "=a" (lo), "=d" (hi));
	return (uint64_t)hi << 32 | lo;
} 
#endif

#if 0
#define   md6_w    64

/* Define "md6_word" appropriately for given value of md6_w.
** Also define PR_MD6_WORD to be the appropriate hex format string,
** using the format strings from inttypes.h .
** The term `word' in comments means an `md6_word'.
*/

#if (md6_w==64)                    /* standard md6 */
typedef uint64_t md6_word;
#define PR_MD6_WORD "%.16" PRIx64
#endif


/* MD6 compression function constants  */
#define md6_n      89    /* size of compression input block, in words  */
#define md6_c      16    /* size of compression output, in words       */
                         /* a c-word block is also called a "chunk"    */
#define md6_max_r 255    /* max allowable value for number r of rounds */

/* Compression function routines                                
** These are ``internal'' routines that need not be called for  
** ordinary md6 usage.
*/


typedef uint64_t md6_control_word;                      /* (r,L,z,p,d) */
typedef uint64_t md6_nodeID; /* (ell,i) */


/* MD6 mode of operation.
** MD6 mode of operation is defined in file md6_mode.c 
*/

/* MD6 constants related to standard mode of operation                 */
/* These five values give lengths of the components of compression     */
/* input block; they should sum to md6_n.                              */

#define md6_q 15         /* # Q words in compression block (>=0)       */
#define md6_k  8         /* # key words per compression block (>=0)    */
#define md6_u (64/md6_w) /* # words for unique node ID (0 or 64/w)     */
#define md6_v (64/md6_w) /* # words for control word (0 or 64/w)       */
#define md6_b 64         /* # data words per compression block (>0)    */

#define md6_default_L 64    /* large so that MD6 is fully hierarchical */

#define md6_max_stack_height 29
    /* max_stack_height determines the maximum number of bits that
    ** can be processed by this implementation (with default L) to be:
    **    (b*w) * ((b/c) ** (max_stack_height-3)
    **    = 2 ** 64  for b = 64, w = 64, c = 16, and  max_stack_height = 29
    ** (We lose three off the height since level 0 is unused,
    ** level 1 contains the input data, and C has 0-origin indexing.)
    ** The smallest workable value for md6_max_stack_height is 3.
    ** (To avoid stack overflow for non-default L values, 
    ** we should have max_stack_height >= L + 2.)
    ** (One level of storage could be saved by letting st->N[] use
    ** 1-origin indexing, since st->N[0] is now unused.)
    */

/* MD6 state.
** 
** md6_state is the main data structure for the MD6 hash function.
*/

typedef struct {

  int d;           /* desired hash bit length. 1 <= d <= 512.      */
  int hashbitlen;  /* hashbitlen is the same as d; for NIST API    */

  unsigned char hashval[ md6_c*(md6_w/8) ];
      /* e.g. unsigned char hashval[128]                           */
      /* contains hashval after call to md6_final                  */
      /* hashval appears in first floor(d/8) bytes, with           */
      /* remaining (d mod 8) bits (if any) appearing in            */
      /* high-order bit positions of hashval[1+floor(d/8)].        */

  unsigned char hexhashval[(md6_c*(md6_w/8))+1];
      /* e.g. unsigned char hexhashval[129];                       */
      /* zero-terminated string representing hex value of hashval  */

  int initialized;         /* zero, then one after md6_init called */
  uint64_t bits_processed;                /* bits processed so far */
  uint64_t compression_calls;    /* compression function calls made*/
  int finalized;          /* zero, then one after md6_final called */

  md6_word K[ md6_k ];  
      /* k-word (8 word) key (aka "salt") for this instance of md6 */
  int keylen;
      /* number of bytes in key K. 0<=keylen<=k*(w/8)              */

  int L;
      /* md6 mode specification parameter. 0 <= L <= 255           */
      /* L == 0 means purely sequential (Merkle-Damgaard)          */
      /* L >= 29 means purely tree-based                           */
      /* Default is md6_default_L = 64 (hierarchical)              */

  int r;
      /* Number of rounds. 0 <= r <= 255                           */

  int top;
      /* index of block corresponding to top of stack              */

  md6_word B[ md6_max_stack_height ][ md6_b ];
      /* md6_word B[29][64]                                        */
      /* stack of 29 64-word partial blocks waiting to be          */
      /* completed and compressed.                                 */
      /* B[1] is for compressing text data (input);                */
      /* B[ell] corresponds to node at level ell in the tree.      */

  unsigned int bits[ md6_max_stack_height ];    
      /* bits[ell] =                                               */
      /*    number of bits already placed in B[ell]                */
      /*    for 1 <= ell < max_stack_height                        */
      /* 0 <= bits[ell] <= b*w                                     */

  uint64_t i_for_level[ md6_max_stack_height ];
      /* i_for_level[ell] =                                        */
      /*    index of the node B[ ell ] on this level (0,1,...)     */
      /* when it is output   */

} md6_state;
#endif


/* MD6 main interface routines
** These routines are defined in md6_mode.c
*/



/* MD6 return codes.
**
** The interface routines defined in md6_mode.c always return a
** "return code": an integer giving the status of the call.
** The codes
*/

/* SUCCESS:  */
#define MD6_SUCCESS 		0

/* ERROR CODES: */
#define MD6_FAIL 1           /* some other problem                     */
#define MD6_BADHASHLEN 2     /* hashbitlen<1 or >512 bits              */
#define MD6_NULLSTATE 3      /* null state passed to MD6               */
#define MD6_BADKEYLEN 4      /* key length is <0 or >512 bits          */
#define MD6_STATENOTINIT 5   /* state was never initialized            */
#define MD6_STACKUNDERFLOW 6 /* MD6 stack underflows (shouldn't happen)*/
#define MD6_STACKOVERFLOW 7  /* MD6 stack overflow (message too long)  */
#define MD6_NULLDATA 8       /* null data pointer                      */
#define MD6_NULL_N 9         /* compress: N is null                    */
#define MD6_NULL_B 10        /* standard compress: null B pointer      */
#define MD6_BAD_ELL 11       /* standard compress: ell not in {0,255}  */
#define MD6_BAD_p 12         /* standard compress: p<0 or p>b*w        */
#define MD6_NULL_K 13        /* standard compress: K is null           */
#define MD6_NULL_Q 14        /* standard compress: Q is null           */
#define MD6_NULL_C 15        /* standard compress: C is null           */
#define MD6_BAD_L 16         /* standard compress: L <0 or > 255       */ 
                             /* md6_init: L<0 or L>255                 */
#define MD6_BAD_r 17         /* compress: r<0 or r>255                 */
                             /* md6_init: r<0 or r>255                 */
#define MD6_OUT_OF_MEMORY 18 /* compress: storage allocation failed    */


/* compression hook, if defined, points to a function that is 
** called after each compression operation.                             
**
** compression hook must be set *after* md6_init or md6_full_init 
** is called.
*/

void (* compression_hook)(md6_word *C,
			  const md6_word *Q,
			  md6_word *K,
			  int ell,
			  int i,
			  int r,
			  int L,
			  int z,
			  int p,
			  int keylen,
			  int d,
			  md6_word *N
			  );


/// md6_nist.h
typedef unsigned char BitSequence;
typedef unsigned long long DataLength;
typedef enum { SUCCESS = 0, FAIL = 1, BAD_HASHLEN = 2 } HashReturn;
typedef md6_state hashState;
HashReturn Init( hashState *state, int hashbitlen);
HashReturn Update( hashState *state, const BitSequence *data, DataLength databitlen);
HashReturn Final( hashState *state, BitSequence *hashval );
HashReturn Hash( int hashbitlen, const BitSequence *data, DataLength databitlen, BitSequence *hashval );




/* Variables defining lengths of various values */
#define   wwww   md6_w  /* # bits in a word (64) */
#define   nnnn   md6_n  /* # words in compression input (89) */
#define   cccc   md6_c  /* # words in compression output (16) */
#define   bbbb   md6_b  /* # message words per compression input block (64) */
#define   vvvv   md6_v  /* # words in control word (1) */
#define   uuuu   md6_u  /* # words in unique nodeID (1) */
#define   kkkk   md6_k  /* # key words per compression input block (8) */
#define   qqqq   md6_q  /* # Q words per compression input block (15) */


/* "Tap positions" for feedback shift-register */

#if (nnnn==89)
#define  t0   17     /* index for linear feedback */
#define  t1   18     /* index for first input to first and */
#define  t2   21     /* index for second input to first and */
#define  t3   31     /* index for first input to second and */
#define  t4   67     /* index for second input to second and */
#define  t5   89     /* last tap */
#endif

/* Loop-unrolling setup.
**
** Define macros for loop-unrolling within compression function 
** These expand to:     loop_body(right-shift,left-shift,step)      
** These are given for word sizes 64, 32, 16, and 8, although   
** only w=64 is needed for the standard MD6 definition.         
**                                                              
** Also given for each word size are the constants S0 and Smask 
** needed in the recurrence for round constants.                
*/

#if (wwww==64)                        /* for standard word size */
#define RL00 loop_body(10,11, 0)
#define RL01 loop_body( 5,24, 1)
#define RL02 loop_body(13, 9, 2)
#define RL03 loop_body(10,16, 3)
#define RL04 loop_body(11,15, 4)
#define RL05 loop_body(12, 9, 5)
#define RL06 loop_body( 2,27, 6)
#define RL07 loop_body( 7,15, 7)
#define RL08 loop_body(14, 6, 8)
#define RL09 loop_body(15, 2, 9)
#define RL10 loop_body( 7,29,10)
#define RL11 loop_body(13, 8,11)
#define RL12 loop_body(11,15,12)
#define RL13 loop_body( 7, 5,13)
#define RL14 loop_body( 6,31,14)
#define RL15 loop_body(12, 9,15)

const md6_word S0 = (md6_word)0x0123456789abcdefULL;
const md6_word Smask = (md6_word)0x7311c2812425cfa0ULL;
#endif



/* Main compression loop. */
/*
** Perform the md6 "main compression loop" on the array A.
** This is where most of the computation occurs; it is the "heart"
** of the md6 compression algorithm.
** Input:
**     A                  input array of length t+n already set up
**                        with input in the first n words.
**     r                  number of rounds to run (178); each is c steps
** Modifies:
**     A                  A[n..r*c+n-1] filled in.
*/
void md6_main_compression_loop( md6_word* A , int r )
{ md6_word x, S;
  int i,j;

  /*
  ** main computation loop for md6 compression
  */
  S = S0;
  for (j = 0, i = nnnn; j<r*cccc; j+=cccc)
  {

/* ***************************************************************** */
#define loop_body(rs,ls,step)                                       \
      x = S;                                /* feedback constant     */ \
      x ^= A[i+step-t5];                    /* end-around feedback   */ \
      x ^= A[i+step-t0];                    /* linear feedback       */ \
      x ^= ( A[i+step-t1] & A[i+step-t2] ); /* first quadratic term  */ \
      x ^= ( A[i+step-t3] & A[i+step-t4] ); /* second quadratic term */ \
      x ^= (x >> rs);                       /* right-shift           */ \
      A[i+step] = x ^ (x << ls);            /* left-shift            */   
/* ***************************************************************** */

      /*
      ** Unroll loop c=16 times. (One "round" of computation.)
      ** Shift amounts are embedded in macros RLnn.
      */
      RL00 RL01 RL02 RL03 RL04 RL05 RL06 RL07
      RL08 RL09 RL10 RL11 RL12 RL13 RL14 RL15

      /* Advance round constant S to the next round constant. */
      S = (S << 1) ^ (S >> (wwww-1)) ^ (S & Smask);
      i += 16;
  }
}


/* ``Bare'' compression routine.
**
** Compresses n-word input to c-word output.
*/
/* Assumes n-word input array N has been fully set up.
** Input:
**	 N				 input array of n w-bit words (n=89)
**	 A				 working array of a = rc+n w-bit words
**					 A is OPTIONAL, may be given as NULL 
**					 (then md6_compress allocates and uses its own A).
**	 r				 number of rounds			 
** Modifies:
**	 C				 output array of c w-bit words (c=16)
** Returns one of the following:
**	 MD6_SUCCESS (0)	
**	 MD6_NULL_N 		
**	 MD6_NULL_C 		
**	 MD6_BAD_r			
**	 MD6_OUT_OF_MEMORY	
*/

int md6_compress( md6_word *C,
		  md6_word *N,
		  int r,
		  md6_word *A
		 )
{ md6_word* A_as_given = A;

  /* check that input is sensible */
  if ( N == NULL) return MD6_NULL_N;
  if ( C == NULL) return MD6_NULL_C;
  if ( r<0 || r > md6_max_r) return MD6_BAD_r;

  if ( A == NULL) A = calloc(r*cccc+nnnn,sizeof(md6_word));
  if ( A == NULL) return MD6_OUT_OF_MEMORY;

  memcpy( A, N, nnnn*sizeof(md6_word) );    /* copy N to front of A */

  md6_main_compression_loop( A, r );          /* do all the work */

  memcpy( C, A+(r-1)*cccc+nnnn, cccc*sizeof(md6_word) ); /* output into C */

  if ( A_as_given == NULL )           /* zero and free A if nec. */
    { memset(A,0,(r*cccc+nnnn)*sizeof(md6_word)); /* contains key info */
      free(A);           
    }

  return MD6_SUCCESS;
}


/* Control words.*/
/* Construct control word V for given inputs.
** Input:
**   r = number of rounds
**   L = mode parameter (maximum tree height)
**   z = 1 iff this is final compression operation
**   p = number of pad bits in a block to be compressed
**   keylen = number of bytes in key
**   d = desired hash output length
**   Does not check inputs for validity.
** Returns:
**   V = constructed control word
*/
md6_control_word md6_make_control_word(	int r, 
					int L, 
					int z, 
					int p, 
					int keylen, 
					int d 
					)
{ md6_control_word V;
  V = ( (((md6_control_word) 0) << 60) | /* reserved, width  4 bits */
	(((md6_control_word) r) << 48) |           /* width 12 bits */
	(((md6_control_word) L) << 40) |           /* width  8 bits */
	(((md6_control_word) z) << 36) |           /* width  4 bits */
	(((md6_control_word) p) << 20) |           /* width 16 bits */
	(((md6_control_word) keylen) << 12 ) |     /* width  8 bits */
        (((md6_control_word) d)) );                /* width 12 bits */
  return V;
}


/* Node ID's. */
/* Make "unique nodeID" U based on level ell and position i 
** within level; place it at specified destination.
** Inputs:
**    dest = address of where nodeID U should be placed
**    ell = integer level number, 1 <= ell <= ...
**    i = index within level, i = 0, 1, 2,...
** Returns
**    U = constructed nodeID
*/
md6_nodeID md6_make_nodeID( int ell,                     /* level number */
			      int i    /* index (0,1,2,...) within level */
			    )
{ md6_nodeID U;
  U = ( (((md6_nodeID) ell) << 56) | 
	((md6_nodeID) i) );
  return U;
}


/* Assembling components of compression input. */
/* Pack data before compression into n-word array N. */
void md6_pack( md6_word*N,
	       const md6_word* Q,
	       md6_word* K,
	       int ell, int i,
	       int r, int L, int z, int p, int keylen, int d,
	       md6_word* B )
{ int j;
  int ni;
  md6_nodeID U;
  md6_control_word V;    

  ni = 0;

  for (j=0;j<qqqq;j++) N[ni++] = Q[j];       /* Q: Q in words     0--14 */

  for (j=0;j<kkkk;j++) N[ni++] = K[j];       /* K: key in words  15--22 */

  U = md6_make_nodeID(ell,i);             /* U: unique node ID in 23 */
  /* The following also works for variants 
  ** in which u=0.
  */
  memcpy((unsigned char *)&N[ni],
	 &U,
	 min(uuuu*(wwww/8),sizeof(md6_nodeID)));
  ni += uuuu;

  V = md6_make_control_word(
			r,L,z,p,keylen,d);/* V: control word in   24 */
  /* The following also works for variants
  ** in which v=0.
  */
  memcpy((unsigned char *)&N[ni],
	 &V,
	 min(vvvv*(wwww/8),sizeof(md6_control_word)));
  ni += vvvv;

  memcpy(N+ni,B,bbbb*sizeof(md6_word));      /* B: data words    25--88 */
}
	       

/* Standard compress: assemble components and then compress*/
/* Perform md6 block compression using all the "standard" inputs.
** Input:
**     Q              q-word (q=15) approximation to (sqrt(6)-2)
**     K              k-word key input (k=8)
**     ell            level number
**     i              index within level
**     r              number of rounds in this compression operation
**     L              mode parameter (max tree height)
**     z              1 iff this is the very last compression
**     p              number of padding bits of input in payload B
**     keylen         number of bytes in key
**     d              desired output hash bit length
**     B              b-word (64-word) data input block (with zero padding)
** Modifies:
**     C              c-word output array (c=16)
** Returns one of the following:
**   MD6_SUCCESS (0)   MD6_BAD_p
**   MD6_NULL_B        MD6_BAD_HASHLEN
**   MD6_NULL_C        MD6_NULL_K
**   MD6_BAD_r         MD6_NULL_Q
**   MD6_BAD_ELL       MD6_OUT_OF_MEMORY
*/
int md6_standard_compress( md6_word* C,
			   const md6_word* Q,
			   md6_word* K,
			   int ell, int i,
			   int r, int L, int z, int p, int keylen, int d,
			   md6_word* B 
			   )
{ md6_word N[md6_n];
  md6_word A[5000];       /* MS VS can't handle variable size here */

  /* check that input values are sensible */
  if ( (C == NULL) ) return MD6_NULL_C;
  if ( (B == NULL) ) return MD6_NULL_B;
  if ( (r<0) | (r>md6_max_r) ) return MD6_BAD_r;
  if ( (L<0) | (L>255) ) return MD6_BAD_L;
  if ( (ell < 0) || (ell > 255) ) return MD6_BAD_ELL;
  if ( (p < 0) || (p > bbbb*wwww ) ) return MD6_BAD_p;
  if ( (d <= 0) || (d > cccc*wwww/2) ) return MD6_BADHASHLEN;
  if ( (K == NULL) ) return MD6_NULL_K;
  if ( (Q == NULL) ) return MD6_NULL_Q;

  /* pack components into N for compression */
  md6_pack(N,Q,K,ell,i,r,L,z,p,keylen,d,B);

  /* call compression hook if it is defined. */
  /* -- for testing and debugging.           */
  if (compression_hook != NULL)
    compression_hook(C,Q,K,ell,i,r,L,z,p,keylen,d,B);

  return md6_compress(C,N,r,A);
}
//// ------------------------------------------------





/* Default number of rounds                                    */
/* (as a function of digest size d and keylen                  */
int md6_default_r( int d , int keylen )
{ 
  int r;
  /* Default number of rounds is forty plus floor(d/4) */
  r = 40 + (d/4);
  /* unless keylen > 0, in which case it must be >= 80 as well */
  if (keylen>0)
    r = max(80,r);
  return r;
}


/* MD6 Constant Vector Q
** Q = initial 960 bits of fractional part of sqrt(6)
**
** Given here for w = 64, 32, 16, and 8, although only
** w = 64 is needed for the standard version of MD6.
*/

#if (wwww==64) /* for standard version */
/* 15 64-bit words */
static const md6_word Q[15] =
{
    0x7311c2812425cfa0ULL,
    0x6432286434aac8e7ULL, 
    0xb60450e9ef68b7c1ULL, 
    0xe8fb23908d9f06f1ULL, 
    0xdd2e76cba691e5bfULL, 
    0x0cd0d63b2c30bc41ULL, 
    0x1f8ccf6823058f8aULL, 
    0x54e5ed5b88e3775dULL, 
    0x4ad12aae0a6d6031ULL, 
    0x3e7f16bb88222e0dULL, 
    0x8af8671d3fb50c2cULL, 
    0x995ad1178bd25c31ULL, 
    0xc878c1dd04c4b633ULL, 
    0x3b72066c7a1552acULL, 
    0x0d6f3522631effcbULL, 
};
#endif



/* Endianness. */

/* routines for dealing with byte ordering */

int md6_byte_order = 0;    
/* md6_byte_order describes the endianness of the 
** underlying machine:
** 0 = unknown
** 1 = little-endian
** 2 = big-endian
*/

/* Macros to detect machine byte order; these
** presume that md6_byte_order has been setup by
** md6_detect_byte_order()
*/
#define MD6_LITTLE_ENDIAN (md6_byte_order == 1)
#define MD6_BIG_ENDIAN    (md6_byte_order == 2)
 

/* determine if underlying machine is little-endian or big-endian
** set global variable md6_byte_order to reflect result
** Written to work for any w.
*/
void md6_detect_byte_order( void )
{ md6_word x = 1 | (((md6_word)2)<<(wwww-8));
  unsigned char *cp = (unsigned char *)&x;
  if ( *cp == 1 )        md6_byte_order = 1;      /* little-endian */
  else if ( *cp == 2 )   md6_byte_order = 2;      /* big-endian    */
  else                   md6_byte_order = 0;      /* unknown       */
}



/* return byte-reversal of md6_word x.
** Written to work for any w, w=8,16,32,64.
*/
md6_word md6_byte_reverse( md6_word x )
{ 
#define mask8  ((md6_word)0x00ff00ff00ff00ffULL)
#define mask16 ((md6_word)0x0000ffff0000ffffULL)

#if (wwww==64)
  x = (x << 32) | (x >> 32);
#endif

#if (wwww >= 32)
  x = ((x & mask16) << 16) | ((x & ~mask16) >> 16);
#endif

#if (wwww >= 16)
  x = ((x & mask8) << 8) | ((x & ~mask8) >> 8);
#endif

  return x;
}



/* Byte-reverse words x[0...count-1] if machine is little_endian */
void md6_reverse_little_endian( md6_word *x, int count )
{
  int i;
  if (MD6_LITTLE_ENDIAN)
    for (i=0;i<count;i++)
      x[i] = md6_byte_reverse(x[i]);
}


/* Appending one bit string onto another.
*/
/* Append bit string src to the end of bit string dest
** Input:
**	   dest 		a bit string of destlen bits, starting in dest[0]
**					if destlen is not a multiple of 8, the high-order
**					bits are used first
**	   src			a bit string of srclen bits, starting in src[0]
**					if srclen is not a multiple of 8, the high-order
**					bits are used first
** Modifies:
**	   dest 		when append_bits returns, dest will be modified to
**					be a bit-string of length (destlen+srclen).
**					zeros will fill any unused bit positions in the 
**					last byte.
*/
void append_bits( unsigned char *dest, unsigned int destlen, unsigned char *src,  unsigned int srclen )
{ int i, di, accumlen;
  uint16_t accum;
  int srcbytes;

  if (srclen == 0) return;

  /* Initialize accum, accumlen, and di */
  accum = 0;    /* accumulates bits waiting to be moved, right-justified */
  accumlen = 0; /* number of bits in accumulator */
  if (destlen%8 != 0)
    { accumlen = destlen%8;
      accum = dest[destlen/8];        /* grab partial byte from dest     */
      accum = accum >> (8-accumlen);  /* right-justify it in accumulator */
    }
  di = destlen/8;        /* index of where next byte will go within dest */
  
  /* Now process each byte of src */
  srcbytes = (srclen+7)/8;   /* number of bytes (full or partial) in src */
  for (i=0;i<srcbytes;i++)
    { /* shift good bits from src[i] into accum */
      if (i != srcbytes-1) /* not last byte */
	{ accum = (accum << 8) ^ src[i];  
	  accumlen += 8;
	}
      else /* last byte */
	{ int newbits = ((srclen%8 == 0) ? 8 : (srclen%8));
	  accum = (accum << newbits) | (src[i] >> (8-newbits));
	  accumlen += newbits;
	}
      /* do as many high-order bits of accum as you can (or need to) */
      while ( ( (i != srcbytes-1) & (accumlen >= 8) ) ||
	      ( (i == srcbytes-1) & (accumlen > 0) ) )
	{ int numbits = min(8,accumlen);
	  unsigned char bits;
	  bits = accum >> (accumlen - numbits);    /* right justified */
	  bits = bits << (8-numbits);              /* left justified  */
	  bits &= (0xff00 >> numbits);             /* mask            */
	  dest[di++] = bits;                       /* save            */
	  accumlen -= numbits; 
	}
    }
}


/* State initialization. (md6_full_init, with all parameters specified)
*/
/* Initialize md6_state
** Input:
**	   st		  md6_state to be initialized
**	   d		  desired hash bit length 1 <= d <= w*(c/2)    (<=512 bits)
**	   key		  key (aka salt) for this hash computation	   (byte array)
**				  defaults to all-zero key if key==NULL or keylen==0
**	   keylen	  length of key in bytes; 0 <= keylen <= (k*8) (<=64 bytes)
**	   L		  md6 mode parameter; 0 <= L <= 255
**				  md6.h defines md6_default_L for when you want default
**	   r		  number of rounds; 0 <= r <= 255
** Output:
**	   updates components of state
**	   returns one of the following:
**		 MD6_SUCCESS
**		 MD6_NULLSTATE
**		 MD6_BADKEYLEN
**		 MD6_BADHASHLEN
*/
int md6_full_init( md6_state *st,       /* uninitialized state to use */
		   int d,                          /* hash bit length */
		   unsigned char *key,        /* key; OK to give NULL */
		   int keylen,     /* keylength (bytes); OK to give 0 */
		   int L,           /* mode; OK to give md6_default_L */
		   int r                          /* number of rounds */
		   )
{ /* check that md6_full_init input parameters make some sense */
  if (st == NULL) return MD6_NULLSTATE;
  if ( (key != NULL) && ((keylen < 0) || (keylen > kkkk*(wwww/8))) )
    return MD6_BADKEYLEN;
  if ( d < 1 || d > 512 || d > wwww*cccc/2 ) return MD6_BADHASHLEN;

  md6_detect_byte_order();
  memset(st,0,sizeof(md6_state));  /* clear state to zero */
  st->d = d;                       /* save hashbitlen */
  if (key != NULL && keylen > 0)   /* if no key given, use memset zeros*/
    { memcpy(st->K,key,keylen);    /* else save key (with zeros added) */
      st->keylen = keylen;
      /* handle endian-ness */       /* first byte went into high end */
      md6_reverse_little_endian(st->K,kkkk);
    }
  else
    st->keylen = 0;
  if ( (L<0) | (L>255) ) return MD6_BAD_L;
  st->L = L;
  if ( (r<0) | (r>255) ) return MD6_BAD_r;
  st->r = r;
  st->initialized = 1;  
  st->top = 1;
  /* if SEQ mode for level 1; use IV=0  */
  /* zero bits already there by memset; */
  /* we just need to set st->bits[1]    */
  if (L==0) st->bits[1] = cccc*wwww;     
  compression_hook = NULL;     /* just to be sure default is "not set" */
  return MD6_SUCCESS;
}


/* State initialization. (md6_init, which defaults most parameters.)
*/
/* Same as md6_full_init, but with default key, L, and r */
int md6_init( md6_state *st, int d)
{ return md6_full_init(st,
		       d,
		       NULL,
		       0,
		       md6_default_L,
		       md6_default_r(d,0)
		       );
}


/* Data structure notes.
*/

/*
Here are some notes on the data structures used (inside state).

* The variable B[] is a stack of length-b (b-64) word records,
  each corresponding to a node in the tree.  B[ell] corresponds
  to a node at level ell.  Specifically, it represents the record which,
  when compressed, will yield the value at that level. (It only
  contains the data payload, not the auxiliary information.)
  Note that B[i] is used to store the *inputs* to the computation at
  level i, not the output for the node at that level.  
  Thus, for example, the message input is stored in B[1], not B[0].

* Level 0 is not used.  The message bytes are placed into B[1].

* top is the largest ell for which B[ell] has received data,
  or is equal to 1 in case no data has been received yet at all.

* top is never greater than L+1.  If B[L+1] is
  compressed, the result is put back into B[L+1]  (this is SEQ).

* bits[ell] says how many bits have been placed into
  B[ell].  An invariant maintained is that of the bits in B[ell], 
  only the first bits[ell] may be nonzero; the following bits must be zero.

* The B nodes may have somewhat different formats, depending on the level:
  -- Level 1 node contains a variable-length bit-string, and so
     0 <= bits[1] <= b*w     is all we can say.
  -- Levels 2...top always receive data in c-word chunks (from
     children), so for them bits[ell] is between 0 and b*w,
     inclusive, but is also a multiple of cw.  We can think of these
     nodes as have (b/c) (i.e. 4) "slots" for chunks.
  -- Level L+1 is special, in that the first c words of B are dedicated
     to the "chaining variable" (or IV, for the first node on the level).

* When the hashing is over, B[top] will contain the 
  final hash value, in the first or second (if top = L+1) slot.

*/

/* Compress one block -- compress data at a node (md6_compress_block).
*/
/* compress block at level ell, and put c-word result into C.
** Input:
**	   st		  current md6 computation state
**	   ell		  0 <= ell < max_stack_height-1
**	   z		  z = 1 if this is very last compression; else 0
** Output:
**	   C		  c-word array to put result in
** Modifies:
**	   st->bits[ell]  (zeroed)
**	   st->i_for_level[ell] (incremented)  
**	   st->B[ell] (zeroed)
**	   st->compression_calls (incremented)
** Returns one of the following:
**	   MD6_SUCCESS
**	   MD6_NULLSTATE
**	   MD6_STATENOTINIT
**	   MD6_STACKUNDERFLOW
**	   MD6_STACKOVERFLOW
*/

int md6_compress_block( md6_word *C, md6_state *st, int ell, int z )
{ int p, err;

  /* check that input values are sensible */
  if ( st == NULL) return MD6_NULLSTATE;
  if ( st->initialized == 0 ) return MD6_STATENOTINIT;
  if ( ell < 0 ) return MD6_STACKUNDERFLOW;
  if ( ell >= md6_max_stack_height-1 ) return MD6_STACKOVERFLOW;

  st->compression_calls++;

  if (ell==1) /* leaf; hashing data; reverse bytes if nec. */
    { if (ell<(st->L + 1)) /* PAR (tree) node */
	md6_reverse_little_endian(&(st->B[ell][0]),bbbb);
      else /* SEQ (sequential) node; don't reverse chaining vars */
	md6_reverse_little_endian(&(st->B[ell][cccc]),bbbb-cccc);
    }

  p = bbbb*wwww - st->bits[ell];          /* number of pad bits */

  err = 
    md6_standard_compress( 
      C,                                      /* C    */
      Q,                                      /* Q    */
      st->K,                                  /* K    */
      ell, (int)(st->i_for_level[ell]),              /* -> U */
      st->r, st->L, z, p, st->keylen, st->d,  /* -> V */
      st->B[ell]                              /* B    */
			   );                         
  if (err) return err; 

  st->bits[ell] = 0; /* clear bits used count this level */
  st->i_for_level[ell]++;

  memset(&(st->B[ell][0]),0,bbbb*sizeof(md6_word));     /* clear B[ell] */
  return MD6_SUCCESS;
}


/* Process (compress) a node and its compressible ancestors.
*/
int md6_process( md6_state *st, int ell, int final )
/*
** Do processing of level ell (and higher, if necessary) blocks.
** 
** Input:
**     st         md6 state that has been accumulating message bits
**                and/or intermediate results
**     ell        level number of block to process
**     final      true if this routine called from md6_final 
**                     (no more input will come)
**                false if more input will be coming
**                (This is not same notion as "final bit" (i.e. z)
**                 indicating the last compression operation.)
** Output (by side effect on state):
**     Sets st->hashval to final chaining value on final compression.
** Returns one of the following:
**     MD6_SUCCESS
**     MD6_NULLSTATE
**     MD6_STATENOTINIT
*/
{ int err, z, next_level;
  md6_word C[cccc];

  /* check that input values are sensible */
  if ( st == NULL) return MD6_NULLSTATE;
  if ( st->initialized == 0 ) return MD6_STATENOTINIT;

  if (!final) /* not final -- more input will be coming */
    { /* if not final and block on this level not full, nothing to do */
      if ( st->bits[ell] < bbbb*wwww ) 
	return MD6_SUCCESS;
      /* else fall through to compress this full block, 
      **       since more input will be coming 
      */
    }
  else /* final -- no more input will be coming */
    { if ( ell == st->top )
	{ if (ell == (st->L + 1)) /* SEQ node */
	    { if ( st->bits[ell]==cccc*wwww && st->i_for_level[ell]>0 )
		return MD6_SUCCESS;
	      /* else (bits>cw or i==0, so fall thru to compress */
	    }
           else /* st->top == ell <= st->L so we are at top tree node */
	     { if ( ell>1 && st->bits[ell]==cccc*wwww)
		 return MD6_SUCCESS;
	       /* else (ell==1 or bits>cw, so fall thru to compress */
	     }
	}
      /* else (here ell < st->top so fall through to compress */
    }

  /* compress block at this level; result goes into C */
  /* first set z to 1 iff this is the very last compression */
  z = 0; if (final && (ell == st->top)) z = 1; 
  if ((err = md6_compress_block(C,st,ell,z))) 
      return err;
  if (z==1) /* save final chaining value in st->hashval */
    { memcpy( st->hashval, C, md6_c*(wwww/8) );
      return MD6_SUCCESS;
    }
  
  /* where should result go? To "next level" */
  next_level = min(ell+1,st->L+1);
  /* Start sequential mode with IV=0 at that level if necessary 
  ** (All that is needed is to set bits[next_level] to c*w, 
  ** since the bits themselves are already zeroed, either
  ** initially, or at the end of md6_compress_block.)
  */
  if (next_level == st->L + 1 
      && st->i_for_level[next_level]==0
      && st->bits[next_level]==0 )
    st->bits[next_level] = cccc*wwww;   
  /* now copy C onto next level */
  memcpy((char *)st->B[next_level] + st->bits[next_level]/8,
	 C,
	 cccc*(wwww/8));
  st->bits[next_level] += cccc*wwww;   
  if (next_level > st->top) st->top = next_level;

  return md6_process(st,next_level,final);
}


/* Update -- incorporate data string into hash computation.
*/
/* Process input byte string data, updating state to reflect result
** Input:
**	   st				already initialized state to be updated
**	   data 			byte string of length databitlen bits 
**						to be processed (aka "M")
**	   databitlen		number of bits in string data (aka "m")
** Modifies:
**	   st				updated to reflect input of data
*/
int md6_update( md6_state *st, unsigned char *data, uint64_t databitlen )
{ 
  unsigned int j, portion_size;
  int err;

  /* check that input values are sensible */
  if ( st == NULL ) return MD6_NULLSTATE;
  if ( st->initialized == 0 ) return MD6_STATENOTINIT;
  if ( data == NULL ) return MD6_NULLDATA;
  
  j = 0; /* j = number of bits processed so far with this update */
  while (j<databitlen)
    { /* handle input string in portions (portion_size in bits)
      ** portion_size may be zero (level 1 data block might be full, 
      ** having size b*w bits) */
      portion_size = (unsigned int)min(databitlen-j,
			 (unsigned int)(bbbb*wwww-(st->bits[1]))); 

      if ((portion_size % 8 == 0) && 
	  (st->bits[1] % 8 == 0) &&
	  (j % 8 == 0))
	{ /* use mempy to handle easy, but most common, case */
	  memcpy((char *)st->B[1] + st->bits[1]/8,
		 &(data[j/8]),                                 
		 portion_size/8);
	}
      else /* handle messy case where shifting is needed */
	{ append_bits((unsigned char *)st->B[1], /* dest */
		      st->bits[1],   /* dest current bit size */
		      &(data[j/8]),  /* src */
		      portion_size); /* src size in bits  */
	}
      j += portion_size;
      st->bits[1] += portion_size;
      st->bits_processed += portion_size;

      /* compress level-1 block if it is now full 
	 but we're not done yet */
      if (st->bits[1] == bbbb*wwww && j<databitlen)
	{ if ((err=md6_process(st,
			       1,    /* ell */
			       0     /* final */
			       ))) 
	    return err; 
	}
    } /* end of loop body handling input portion */
  return MD6_SUCCESS;
}


/* Convert hash value to hexadecimal, and store it in state.
*/
/*
** Convert hashval in st->hashval into hexadecimal, and
** save result in st->hexhashval
** This will be a zero-terminated string of length ceil(d/4).
** Assumes that hashval has already been "trimmed" to correct 
** length.
** 
** Returns one of the following:
**	  MD6_SUCCESS
**	  MD6_NULLSTATE 					 (if input state pointer was NULL)
*/
int md6_compute_hex_hashval( md6_state *st )
{ int i;
  static unsigned char hex_digits[] = "0123456789abcdef";

  /* check that input is sensible */
  if ( st == NULL ) return MD6_NULLSTATE;
  
  for (i=0;i<((st->d+7)/8);i++)
    { st->hexhashval[2*i]   
	= hex_digits[ ((st->hashval[i])>>4) & 0xf ];
      st->hexhashval[2*i+1] 
	= hex_digits[ (st->hashval[i]) & 0xf ];
    }
  
  /* insert zero string termination byte at position ceil(d/4) */
  st->hexhashval[(st->d+3)/4] = 0;
  return MD6_SUCCESS;
}


/* Extract last d bits of chaining variable as hash value.*/

void trim_hashval(md6_state *st)
{ /* trim hashval to desired length d bits by taking only last d bits */
  /* note that high-order bit of a byte is considered its *first* bit */
  int full_or_partial_bytes = (st->d+7)/8;
  int bits = st->d % 8;                 /* bits in partial byte */
  int i;

  /* move relevant bytes to the front */
  for ( i=0; i<full_or_partial_bytes; i++ )
    st->hashval[i] = st->hashval[cccc*(wwww/8)-full_or_partial_bytes+i];

  /* zero out following bytes */
  for ( i=full_or_partial_bytes; i<cccc*(wwww/8); i++ )
    st->hashval[i] = 0;

  /* shift result left by (8-bits) bit positions, per byte, if needed */
  if (bits>0)
    { for ( i=0; i<full_or_partial_bytes; i++ )
	{ st->hashval[i] = (st->hashval[i] << (8-bits));
	  if ( (i+1) < cccc*(wwww/8) )
	    st->hashval[i] |= (st->hashval[i+1] >> bits);
	}
    }
}

/* Final -- no more data; finish up and produce hash value.*/
/* Do final processing to produce md6 hash value
** Input:
**     st              md6 state that has been accumulating message bits
**                     and/or intermediate results
** Output (by side effect on state):
**     hashval         If this is non-NULL, final hash value copied here.
**                     (NULL means don't copy.)  In any case, the hash
**                     value remains in st->hashval.
**     st->hashval     this is a 64-byte array; the first st->d
**                     bits of which will be the desired hash value
**                     (with high-order bits of a byte used first), and
**                     remaining bits set to zero (same as hashval)
**     st->hexhashval  this is a 129-byte array which contains the
**                     zero-terminated hexadecimal version of the hash
** Returns one of the following:
**     MD6_SUCCESS
**     MD6_NULLSTATE
**     MD6_STATENOTINIT
*/
int md6_final( md6_state *st , unsigned char *hashval)
{ int ell, err;

  /* check that input values are sensible */
  if ( st == NULL) return MD6_NULLSTATE;
  if ( st->initialized == 0 ) return MD6_STATENOTINIT;

  /* md6_final was previously called */
  if ( st->finalized == 1 ) return MD6_SUCCESS;

  /* force any processing that needs doing */
  if (st->top == 1) ell = 1;
  else for (ell=1; ell<=st->top; ell++)
	 if (st->bits[ell]>0) break;
  /* process starting at level ell, up to root */
  err = md6_process(st,ell,1);
  if (err) return err;

  /* md6_process has saved final chaining value in st->hashval */

  md6_reverse_little_endian( (md6_word*)st->hashval, cccc );

  /* 4/15/09: Following two lines were previously out of order, which 
  **          caused errors depending on whether caller took hash output 
  **          from  st->hashval (which was correct) or from 
  **                hashval parameter (which was incorrect, since it 
  **                                   missed getting "trimmed".)
  */
  trim_hashval( st );
  if (hashval != NULL) memcpy( hashval, st->hashval, (st->d+7)/8 );

  md6_compute_hex_hashval( st );

  st->finalized = 1;
  return MD6_SUCCESS;
}

/* Routines for hashing message given "all at once".*/
int md6_full_hash( int d,                    /* hash bit length */
		   unsigned char *data,/* complete data to hash */
		   uint64_t databitlen,   /* its length in bits */
		   unsigned char *key,       /* OK to give NULL */
		   int keylen,       /* (in bytes) OK to give 0 */
		   int L,     /* mode; OK to give md6_default_L */
		   int r,                   /* number of rounds */
		   unsigned char *hashval             /* output */
		   )
{ md6_state st;
  int err;

  err = md6_full_init(&st,d,key,keylen,L,r);
  if (err) return err;
  err = md6_update(&st,data,databitlen);
  if (err) return err;
  md6_final(&st,hashval);
  if (err) return err;
  return MD6_SUCCESS;
}


/// int d: hash bit length
/// byte *data : complete data to hash
/// uint64_t databitlen : its length in bits
/// unsigned char *hashval : output
int md6_hash( int d, unsigned char *data, uint64_t databitlen, unsigned char *hashval )
{ 
  int err;

  err = md6_full_hash(d,data,databitlen,
		      NULL,0,md6_default_L,md6_default_r(d,0),hashval);
  if (err) return err;
  return MD6_SUCCESS;
}

//// end of md6_mode.c -------------------------------



HashReturn Init( hashState *state, int hashbitlen)
{ int err;
  if ((err = md6_init( (md6_state *) state, 
		       hashbitlen
		       )))
    return err;
  state->hashbitlen = hashbitlen;
  return SUCCESS;
}

HashReturn Update( hashState *state, const BitSequence *data, DataLength databitlen )
{ 
	return md6_update( (md6_state *) state, 
		     (unsigned char *)data, 
		     (uint64_t) databitlen );
}

HashReturn Final( hashState *state, BitSequence *hashval )
{ 
	return md6_final( (md6_state *) state, (unsigned char *) hashval );
}

HashReturn Hash( int hashbitlen, const BitSequence *data, DataLength databitlen, BitSequence *hashval )
{ int err;
  md6_state state;
  if ((err = Init( &state, hashbitlen ))) 
    return err;
  if ((err = Update( &state, data, databitlen ))) 
    return err;
  return Final( &state, hashval );
}


/* ------------ MD6 parameters --------------------- */
int md6_dgtLen = 256;         /* digest length */
int md6_modPar = 64;          /* mode parameter */
int md6_roundN;               /* number of rounds */
int md6_use_default_r = 1;    /* 1 if r should be set to the default, 0 if r is explicitly provided */
unsigned char md6_K[100];     /* key */
int md6_keylen = 0;           /* key length in bytes (at most 64) */
md6_state md6_st;             /* md6 computation state */

char md6_msg[5000];           /* message to be hashed (if given with -M) */
int msglenbytes;              /* message length in bytes */


/* return integer starting at s (input presumed to end with '\n')
** (It may be expressed in exponential format e.g. 1e9.)
*/
uint64_t get_int(char *s)
{ 
	long double g;
	sscanf(s,"%Lg",&g);
	return (uint64_t)g;
}

/* routines to escape/unescape filenames, in case they
** contain backslash or \n 's.
*/
/* input t, output s -- recode t so it all newlines and 
** backslashes are escaped as \n and \\ respectively.
** Also, a leading '-' is escaped to \-.
*/
void encode(char *s, char *t)
{ 
	if (*t && *t=='-')
	{ *s++ = '\\'; *s++ = '-'; t++; }
	while (*t)
    { if (*t=='\\')      { *s++ = '\\'; *s++ = '\\'; }     
      else if (*t=='\n') { *s++ = '\\'; *s++ = 'n';  }
      else               *s++ = *t;
      t++;
    }
	*s = 0;
	return;
}


/* inverse of encode -- s is unescaped version of t. */
void decode(char *s, char *t)
{ while (*t)
    { if (*t == '\\')
	{ if (*(t+1)=='\\')     { *s++ = '\\'; t+=1; }
	  else if (*(t+1)=='n') { *s++ = '\n'; t+=1; } 
	  else if (*(t+1)=='-') { *s++ = '-'; t+=1; }
	  else if (*(t+1)==0)   { *s++ = '\\'; }
	  else                  { *s++ = *t; }
	}
      else *s++ = *t;
      t++;
    }
  *s = 0;
  return;
}

#if 0
/* timing variables and routines */
double start_time;
double end_time;
uint64_t start_ticks;
uint64_t end_ticks;

void start_timer()
{
  start_time = ((double)clock())/CLOCKS_PER_SEC;
  start_ticks = ticks();
}

void end_timer()
{
  end_time = ((double)clock())/CLOCKS_PER_SEC;
  end_ticks = ticks();
}
#endif

int print_times = 0;

void print_time()
{ 
#ifdef DEBUG_PRINT

  double elapsed_time = end_time - start_time;
  uint64_t elapsed_ticks = end_ticks - start_ticks;
  uint64_t bytes = md6_st.bits_processed/8;
  int bits = md6_st.bits_processed % 8;
  if (!print_times) return;
  printf("-- Length = ");
  if (md6_st.bits_processed==0) printf("0");
  if (bytes>0) printf("%g byte",(double)bytes);
  if (bytes>1) printf("s");
  if (bytes>0 && bits>0) printf(" + ");
  if (bits>0) printf("%d bit",bits);
  if (bits>1) printf("s");
  printf("\n");
  printf("-- Compression calls made = %g\n",(double)md6_st.compression_calls);
  if (elapsed_time == 0.0)
    printf("-- Elapsed time too short to measure...\n");
  else
    { printf("-- Elapsed time = %.3f seconds.\n", elapsed_time);
      printf("-- Megabytes per second = %g.\n",
	     (bytes/elapsed_time)/1000000.0);
      printf("-- Microseconds per compression function = %g.\n",
	     (elapsed_time*1.0e6 / md6_st.compression_calls ));
    }
  printf("-- Total clock ticks = %lld\n",
	 (long long int)elapsed_ticks);
  if (bytes>0)
    printf("-- Clock ticks / byte = %lld\n",
	   (long long int)(elapsed_ticks/bytes));
  printf("-- Clock ticks / compression function call = %lld\n",
	 (long long int)(elapsed_ticks/md6_st.compression_calls));

#endif
}



/* testing and debugging */

/* Global variables used by compression_hook_1 */
/// FILE *outFile = NULL;
int  print_input_output = 0;
int  print_intermediate = 0;

void compression_hook_1(md6_word *C,
			const md6_word *Q,
			md6_word *K,
			int ell,
			int ii,
			int r,
			int L,
			int z,
			int p,
			int keylen,
			int d,
			md6_word *B
)
{ 
	// -- int i;
	md6_word A[5000];
	// --time_t now;

	md6_pack(A,Q,K,ell,ii,r,L,z,p,keylen,d,B);

	md6_main_compression_loop( A, r);





#ifdef DEBUG_PRINT

	if (ell==1 && ii==0)
	{ 
		time(&now);
		printf("-- d = %6d (digest length in bits)\n",d);
		printf("-- L = %6d (number of parallel passes)\n",L);
		printf("-- r = %6d (number of rounds)\n",r);

		/* print key out as chars, since for md6sum it is essentially
		** impossible to enter non-char keys...
		*/
		printf("-- K = '");
		for (i=0;i<keylen;i++) 
			printf("%c",(int)(K[i/(wwww/8)]>>8*(7-(i%(wwww/8))))&0xff);
		printf("' (key)\n");
		printf("-- k = %6d (key length in bytes)\n",keylen);
		printf("\n");
	}

  printf("MD6 compression function computation ");
  printf("(level %d, index %d):\n",ell,ii);
  printf("Input (%d words):\n",nnnn);


  for (i=0;i<r*cccc+nnnn;i++)
  {
	if ((i<qqqq))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" Q[%d]\n",i);
	}
	else if ((i>=qqqq)&&(i<qqqq+kkkk))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" key K[%d]\n",i-qqqq);
	}
	else if ((uuuu>0)&&(i==qqqq+kkkk+uuuu-1))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" nodeID U = (ell,i) = (%d,%d)\n",ell,ii);
	}
	else if ((vvvv>0)&&(i==qqqq+kkkk+uuuu+vvvv-1))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" control word V = "
				  "(r,L,z,p,keylen,d) = "
		  "(%d,%d,%d,%d,%d,%d)\n",r,L,z,p,keylen,d);
	}
	else if ((i>=qqqq+kkkk+uuuu+vvvv)&&(i<nnnn))
	{ 
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" data B[%2d] ",i-qqqq-kkkk-uuuu-vvvv);
	  if (ell < L+1) /* PAR node */
		{ if (ell == 1)
		{ if ( (i+(p/wwww))<nnnn )
			printf("input message word %4d",
				ii*bbbb+(i-(qqqq+kkkk+uuuu+vvvv)));
		  else
			printf("padding");
		}
		  else
		if ( (i+(p/wwww))< nnnn )
		  printf(
			  "chaining from (%d,%d)",
			  ell-1,
			  4*ii+(i-(qqqq+kkkk+uuuu+vvvv))/cccc);
		else 
		  printf("padding");
		}
	  else /* SEQ node: ell == L+1 */
		{ if (i-(qqqq+kkkk+uuuu+vvvv)<cccc) /* initial portion: IV or chaining */
		{ if (ii == 0)
			printf("IV");
		  else
			printf("chaining from (%d,%d)",ell,ii-1);
		}
		  else /* data, chaining from below, or padding */
		{ if (i+(p/wwww)>=nnnn)
			printf("padding");
		  else if (ell == 1)
			printf("input message word %4d",
				ii*(bbbb-cccc)+(i-(qqqq+kkkk+uuuu+vvvv+cccc)));
		  else 
			printf(
				"chaining from (%d,%d)",
				ell-1,
				3*ii+(i-(qqqq+kkkk+uuuu+vvvv+cccc))/cccc);
		}
		}
	  printf("\n");
	}
	else if ((i>=r*cccc+nnnn-cccc))
	{ if ((i==r*cccc+nnnn-cccc))
		printf("Output (%d words of chaining values):\n",cccc);
	  printf("A[%4d] = " PR_MD6_WORD,i,A[i]);
	  printf(" output chaining value C[%d]\n",i-(r*cccc+nnnn-cccc));
	}
	  else 
	{ if (i==nnnn)
		{ if (print_intermediate)
		printf("Intermediate values:\n");
		  else
		printf(
			"Intermediate values A[%d..%d] omitted... "
			"\n",nnnn,r*cccc+nnnn-cccc-1);
		}
	  if (print_intermediate)
		printf("A[%4d] = " PR_MD6_WORD "\n",i,A[i]);
	}
  }
  printf("\n");

#endif

}


/* interface to hash routines
*/

void hash_init()
{ int err;
//  start_timer();

  if ((err=md6_full_init(&md6_st, md6_dgtLen, md6_K,md6_keylen, md6_modPar, md6_roundN)))
    { printf("Bad MD6 parameters; can't initialize md6. "
	     "errcode = %d\n",err);
      return;
    }
  if (print_input_output)
    compression_hook = compression_hook_1;
}

void hash_update(char* data, 
		 uint64_t databitlen)
{ int err;
  if ((err=md6_update(&md6_st, 
		      (unsigned char *)data, 
		      databitlen)))
    { printf("MD6 update error. error code: %d\n",err);
      return;
    }
}

void hash_final()
{ int err;
  if ((err=md6_final(&md6_st,NULL)))
    { printf("MD6 finalization error. error code: %d\n",err);
      return;
    }
//  end_timer();
}

void hash_filep(FILE *inFile)
{ uint64_t bytes;
  char data[1024];
  if (inFile==NULL)
    { printf("hash_filep has NULL input file pointer.\n");
      return;
    }
  hash_init();
  while ((bytes = fread (data, 1, 1024, inFile)) != 0)
    hash_update(data,bytes*8);
  hash_final();
}

void hash_stdin()
{ hash_filep(stdin);
}

void hash_file( char *filename )
{ FILE *inFile = fopen (filename, "rb");
  if ( inFile == NULL ) 
    { printf("%s can't be opened.\n", filename);
      return;
    }
  hash_filep(inFile);
  fclose(inFile);
}

void hash_b(uint64_t bitlen)
/* Hash dummy input file of length bitlen bits.
** File (hex) repeats with period 7:
**   11 22 33 44 55 66 77 11 22 33 44 55 66 77 11 22 33 ...
*/
{ int i;
  char data[700];  /* nice if length is multiple of 7 for periodicity */
  for (i=0;i<700;i++)
    data[i] =  0x11 + (char)((i % 7)*(0x11));
  hash_init();
  while (bitlen>0)
    { uint64_t part_len = min(700*8,bitlen);
      hash_update(data,part_len);
      bitlen = bitlen - part_len;
    }
  hash_final();
}


#if 0
void hash_B(uint64_t B)
/* Hash dummy input file of length B bytes.
*/
{ hash_b(B*8);
}

/* Routines to handle command-line options
*/

void optd(char *optstr)
{ /* set MD6 digest length 
  ** -dnn sets digest length to nn, 1 <= nn <= 512 
  */ 
  int dopt = get_int(optstr+2);
  if (dopt<1 || dopt>512)
    printf("Illegal digest size option %s ignored.\n",optstr);
  else
    md6_dgtLen = dopt;
  if (md6_use_default_r) {
    md6_roundN = md6_default_r(md6_dgtLen,md6_keylen);
  }
}

#if 0
void opth()
{ 
  /* print md6sum help string */
  printf(help_string);
}
#endif


void optK(char *optstr)
{ /* set MD6 key */
  optstr += 2;
  md6_keylen = 0;
  while (*optstr && md6_keylen<64) md6_K[md6_keylen++] = *optstr++;
  md6_K[md6_keylen] = 0;
  if (md6_use_default_r) {
    md6_roundN = md6_default_r(md6_dgtLen,md6_keylen);
  }
}

void optL(char *optstr)
{ /* set MD6 mode parameter
  ** -Lnn where 0<=n<=64
  ** nn = 0 means fully sequential
  ** nn = 64 means fully hierarchical
  ** intermediate values give a blended approach
  */
  int Lopt = get_int(optstr+2);
  if (Lopt<0 || Lopt>64)
    printf("Illegal L options %s ignored.\n",optstr);
  else
    md6_modPar = Lopt;
}

void optr(char *optstr)
{ /* set MD6 number of rounds
  ** -rnn where 0<=r<=255
  */
  int ropt = get_int(optstr+2);
  /* Following line was changed 4/15/09 to replace two
  ** occurrences of variable "r" with variable "ropt".
  ** (Thanks to R.L. Vaughn for discovering this error.)
  */     
  if (ropt<0 || ropt>255)
    printf("Illegal r options %s ignored.\n",optstr);
  else {
    md6_roundN = ropt;
    md6_use_default_r = 0;
  }
    
}

void optM(char *optstr)
{ /* hash a message given as a command-line argument */
  char *p = optstr + 2;
  msglenbytes = 0;
  while (*p && msglenbytes<4990) md6_msg[msglenbytes++] = *p++;
  md6_msg[msglenbytes] = 0;
  hash_init();
  hash_update(md6_msg,msglenbytes*8);
  hash_final();
}

void optb(char *optstr)
/* -bnnnn hash dummy file of length nnnn bits */
{ 
  uint64_t bitlen = get_int(optstr+2);
  hash_b(bitlen);
}

void optB(char *optstr)
/* -Bnnnn hash dummy file of length nnnn bytes */
{ uint64_t B = get_int(optstr+2);
  hash_b(B*8);
}

void check_line(char *line)
/* print filename if its hash doesn't agree with what's given in line 
*/
{ char *x;
  char hexhashval[1000];
  int hexhashlen;
  char filename[1000];
  char decfilename[1000];
  int filenamelen;
  /* collect hash value */
  x = line;
  hexhashlen = 0;
  while (*x && *x!=' ' && hexhashlen<900) 
    hexhashval[hexhashlen++] = *x++;
  hexhashval[hexhashlen] = 0;
  if (*x != ' ')
    { printf("Badly formed hash check file line: %s\n",line);
      return;
    }
  x++;
  /* collect filename and decode it */
  filenamelen = 0;
  while (*x && *x != '\n' && filenamelen<900) 
    filename[filenamelen++] = *x++;
  filename[filenamelen] = 0;
  decode(decfilename,filename);
  if (filename[0]=='-')
    { /* handle "filenames" starting with '-' specially,
      ** even though legitimate filenames may start with '-'.
      */
      if (filenamelen==1) 
	return; 	/* skip standard input */
      switch( filename[1] )
	{ 
	case 'M': optM(decfilename); break;
	case 'b': optb(decfilename); break;
	case 'B': optB(decfilename); break;
	default: hash_file(decfilename); break;
	}
    }
  else
    { /* now compute hash of file */
      hash_file(decfilename);
    }
  if (strcmp(hexhashval,(char *)md6_st.hexhashval)!=0)
    printf("%s\n",decfilename);
}



void optt()
/* turn on timing printout */
{
  print_times = 1;
}

/* Routine to print hashvalue filename line.
** Prints time_of_day first if it hasn't been printed already.
*/

int tod_printed = 0;

void print_tod()
{ /* print time-of-day if it hasn't been printed yet. */
  time_t now;
  if (!tod_printed)
    { time(&now);
      printf("-- %s",ctime(&now));
      tod_printed = 1;
    }
}
#endif

#if 0
void opti()
/* turn on printing of input/output values for compression function calls */
{ print_tod();
  print_input_output = 1;
  outFile = stdout;
}

void optI()
/* turn on printing of input/output values AND intermediate values */
{ print_tod();
  print_input_output = 1;
  print_intermediate = 1;
  outFile = stdout;
}

void opts(char *optstr)
{ uint64_t trials = get_int(optstr+2);
  uint64_t i;
  int err;
  double elapsed_time;
  uint64_t elapsed_ticks;
  if (trials == 0) trials = 1;
  start_timer();
  for (i=0;i<trials;i++)
    { md6_st.initialized = 0;
      if ((err = md6_full_init(&md6_st,md6_dgtLen,md6_K,md6_keylen,md6_modPar,md6_roundN)))
	printf("MD6 initialization error %d for option -s.\n",err);
    }
  end_timer();
  elapsed_time = end_time - start_time;
  printf("-- Setup trials = %lld\n",(long long int)trials);
  printf("-- Elapsed time = %.3f seconds\n",elapsed_time);
  elapsed_ticks = end_ticks - start_ticks;
  printf("-- Total clock ticks = %lld\n",(long long int)elapsed_ticks);
  printf("-- Clock ticks / setup = %lld\n",(long long int)(elapsed_ticks/trials));
}
#endif

void print_hash(char *filename)
{ 
#ifdef DEBUG_PRINT
  print_tod();
  if (print_input_output == 0)
	printf("%s %s\n", md6_st.hexhashval,filename);
  else
	printf("Final hash value = %s\n", md6_st.hexhashval);
  print_time(); /* running time */
#endif
}


#if 0
void hash_filep(FILE *inFile)
{ 
	uint64_t bytes;
	char data[1024];
	if (inFile==NULL)
	{ 
		printf("hash_filep has NULL input file pointer.\n");
		return;
	}
	hash_init();
	while ((bytes = fread (data, 1, 1024, inFile)) != 0)
		hash_update(data,bytes*8);
	hash_final();

	/// encode(encfilename,argv[i]);
	print_hash(encfilename);

}
#endif
#endif /// MD6_CHECKSUM_ENCIPHER





#ifdef __cplusplus
}
#endif







