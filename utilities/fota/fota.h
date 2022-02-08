#ifndef _FOTA_H_
#define _FOTA_H_

#ifdef __CC_ARM                         /* ARM Compiler */
    #define FOTA_WEAK                __weak
#elif defined (__IAR_SYSTEMS_ICC__)     /* for IAR Compiler */
    #define FOTA_WEAK                __weak
#elif defined (__GNUC__)                /* GNU GCC Compiler */
    #define FOTA_WEAK                __attribute__((weak))
#endif /* __CC_ARM */

/* Tinycrypt package */
// #define TINY_CRYPT_AES

/**
 * FOTA firmware encryption algorithm and compression algorithm
 */
enum fota_algo
{
    FOTA_CRYPT_ALGO_NONE    = 0x0L,               /**< no encryption algorithm and no compression algorithm */
    FOTA_CRYPT_ALGO_XOR     = 0x1L,               /**< XOR encryption */
    FOTA_CRYPT_ALGO_AES256  = 0x2L,               /**< AES256 encryption */
    FOTA_CMPRS_ALGO_GZIP    = 0x1L << 8,          /**< Gzip: zh.wikipedia.org/wiki/Gzip */
    FOTA_CMPRS_ALGO_QUICKLZ = 0x2L << 8,          /**< QuickLZ: www.quicklz.com */
    FOTA_CMPRS_ALGO_FASTLZ  = 0x3L << 8,          /**< FastLZ: fastlz.org/ */

    FOTA_CRYPT_STAT_MASK    = 0xFL,
    FOTA_CMPRS_STAT_MASK    = 0xFL << 8,
};
typedef enum fota_algo fota_algo_t;

/* FOTA error code */
typedef enum {
    FOTA_NO_ERR             =  0,
    FOTA_GENERAL_ERR        = -1,    /* general error */
    FOTA_CHECK_FAILED       = -2,    /* check failed */
    FOTA_ALGO_NOT_SUPPORTED = -3,    /* firmware algorithm not supported */
    FOTA_COPY_FAILED        = -4,    /* copy firmware to destination partition failed */
    FOTA_FW_VERIFY_FAILED   = -5,    /* firmware verify failed */
    FOTA_NO_MEM_ERR         = -6,    /* no memory */
    FOTA_PART_READ_ERR      = -7,    /* partition read error */
    FOTA_PART_WRITE_ERR     = -8,    /* partition write error */
    FOTA_PART_ERASE_ERR     = -9,    /* partition erase error */
} fota_err_t;

#endif /* _FOTA_H_ */