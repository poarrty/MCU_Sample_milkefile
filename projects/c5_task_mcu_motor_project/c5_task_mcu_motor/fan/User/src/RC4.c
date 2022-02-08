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
#include "RC4.h"

#include "stm32f10x.h" 
#include "core_cm3.h"

RC4_CTX rc4_st ;
const uint8_t rc4_key[8] ={0x57,0x48,0x45,0x45,0x4C,0x43,0x4a,0x4d};
//初始化
int8_t rc4_init(RC4_CTX *ctx,const uint8_t *key, uint16_t keyLen){
    uint16_t  i;
    uint16_t  j;
    uint8_t swp;
    memset(&rc4_st , 0, sizeof(RC4_CTX));
    for(i=0; i<SLen; i++){
        ctx->S[i] = i;
    }
    
    for(i=j=0; i<SLen; i++){
        j = (j + ctx->S[i] + key[i%keyLen]) % SLen;
        swp = ctx->S[i];
        ctx->S[i] = ctx->S[j];
        ctx->S[j] = swp;
    }
    
    ctx->i = 0;
    ctx->j = 0;
    return 0;
}

//加密或解密
uint16_t rc4_run(RC4_CTX *ctx, uint8_t *output, uint8_t *input, uint16_t length){

    uint16_t n;
    uint8_t sum;
   uint8_t swp;
	RC4_CTX RC_t;
    memcpy(&RC_t,ctx,sizeof(RC4_CTX));
    for(n=0; n<length; n++){
        //计算新的 i 和 j。
        RC_t.i = (RC_t.i + 1) % SLen;
        RC_t.j = (RC_t.j + RC_t.S[RC_t.i]) % SLen;
        
        //交换 S[i] 和 S[j]，并计算 S[i]+S[j] 的和。
        sum  = (swp = RC_t.S[RC_t.i]);
        sum += (RC_t.S[RC_t.i] = RC_t.S[RC_t.j]);
        RC_t.S[RC_t.j] = swp;
        
        //加解密一个字节。
        output[n] = input[n] ^ (RC_t.S[sum % SLen]);
    }
    return length;
}
