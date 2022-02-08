/***************************************************************************
* File Name         :  RC6.h
* Author            :  陈锦明
* Version           :  1.0 Copyright (c) 2006-2020 CVTE
* Date              :  2020/5/07 10:1:6
* Description       :  RC4加密算法-48Mhz计算 时耗时2us,r=2耗时1us
********************************************************************************
* History:
* <author>        <time>        <version >        <desc>
* 陈锦明            2020/5/07         1.0            创建
*
*
*
*
*
*
*
*
*
******************************************************************************/
#ifndef _RC4_H_
#define _RC4_H_
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define SLen   64
typedef struct rc4Ctx {
    unsigned int  i;
    unsigned int  j;
    unsigned char S[SLen];
} RC4_CTX;
extern RC4_CTX rc4_st;
extern const uint8_t rc4_key[8];
int8_t rc4_init(RC4_CTX *ctx,const uint8_t *key, uint16_t keyLen);
uint16_t rc4_run(RC4_CTX *ctx, uint8_t *output, uint8_t *input, uint16_t length);

//int rc4_keyStream (RC4_CTX *ctx, unsigned char *keyStream, unsigned int length);

#endif
