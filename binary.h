
#ifndef _BINARY_H_
#define _BINARY_H_

#include <windows.h>
#include <stdio.h>




//typedef int boolean;

//typedef unsigned char byte;
typedef unsigned short word;

typedef unsigned char uint8_t;	/* 1-byte  (8-bits)  */
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;	/* 8-bytes (64-bits) */
typedef unsigned short int uint16_t;

typedef unsigned int uint16;	/* at LEAST 16 bits, maybe more */
typedef unsigned int word16;

typedef unsigned char uint8;
typedef unsigned int  uint32;
typedef unsigned long long uint64;

/* typedef a 32 bit type */
typedef unsigned long int UINT4;



extern const unsigned char Reflect8[256];

extern uint16_t Reflect16(uint16_t Value16);
extern uint32_t Reflect24(uint32_t Value24);
extern uint32_t Reflect32(uint32_t Value32);
extern uint64_t Reflect40(uint64_t Value40);
extern uint64_t Reflect64(uint64_t Value64);

extern uint8_t u16_hi(uint16_t value);
extern uint8_t u16_lo(uint16_t value);

extern uint8_t u32_b3(uint32_t value);
extern uint8_t u32_b2(uint32_t value);
extern uint8_t u32_b1(uint32_t value);
extern uint8_t u32_b0(uint32_t value);

extern uint8_t u64_b7(uint64_t value);
extern uint8_t u64_b6(uint64_t value);
extern uint8_t u64_b5(uint64_t value);
extern uint8_t u64_b4(uint64_t value);
extern uint8_t u64_b3(uint64_t value);
extern uint8_t u64_b2(uint64_t value);
extern uint8_t u64_b1(uint64_t value);
extern uint8_t u64_b0(uint64_t value);

extern uint8_t nibble2ascii(uint8_t value);
extern int cs_isdecdigit(char c);
extern unsigned char tohex(unsigned char c);
extern unsigned char todecimal(unsigned char c);

#endif /* _BINARY_H_ */




