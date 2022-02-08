#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fota.h>
#include <log.h>
#include <tinycrypt.h>
#include <fastlz.h>
#include <quicklz.h>
#include <rtt_fal.h>
#include <main.h>
#include <fota_port.h>
#include <malloc.h>

#define APP_HEADER_LEN			0x200

uint8_t is_first_enter = 1;
typedef struct {
	char type[4];				/* RBL 字符头 */
	uint16_t fota_algo;			/* 算法配置: 表示是否加密或者使用了压缩算法 */
	uint8_t fm_time[6];			/* 原始 bin 文件的时间戳, 6 位时间戳, 使用了 4 字节, 包含年月日信息 */
	char app_part_name[16];		/* app 执行分区名 */
	char download_version[24];	/* 固件代码版本号 */
	char current_version[24];	/* 这个域在 rbl 文件生成时都是一样的，我用于表示 app 分区当前运行固件的版本号，判断是否固件需要升级 */
	uint32_t code_crc;			/* 代码的 CRC32 校验值, 它是的打包后的校验值, 即 rbl 文件 96 字节后的数据 */
	uint32_t hash_val;			/* 原始代码本身的校验值 */
	uint32_t raw_size;			/* 原始代码的大小 */
	uint32_t com_size;			/* 打包代码的大小 */
	uint32_t head_crc;			/* rbl 文件头的 CRC32 校验值，即 rbl 文件的前 96 字节 */
} fota_part_head, *fota_part_head_t;

typedef void (*fota_app_func)(void);	
static fota_app_func app_func = NULL;

static fota_part_head rbl_fota_part_head;

static int fota_boot_verify(void)
{
	int fota_res = FOTA_NO_ERR;

	memset(&rbl_fota_part_head, 0x0, sizeof(fota_part_head));
	
	/* partition initial */
	fal_init(); 

	extern int fal_init_check(void);
	/* verify partition */
	if (fal_init_check() != 1)
    {
    	LOG_D("Partition initialized failed!");
		fota_res = FOTA_GENERAL_ERR;
		goto __exit_boot_verify;
    }

__exit_boot_verify:
	return fota_res;
}

#if defined(FOTA_ROLLBACK)
__weak int fota_rollback_check(void)
{
	return 1;
}

static int fota_rollback_copy(const char *dest_part_name, const char *source_part_name)
{
#define THE_NOR_FLASH_GRANULARITY		4096

	int fota_res = FOTA_NO_ERR;
	const struct fal_partition *dest_part;
	const struct fal_partition *source_part;

    uint8_t *cache_buf = NULL;
	uint32_t write_size = 0;
	uint32_t write_offset = 0;

	dest_part = fal_partition_find(dest_part_name);
	if (dest_part == NULL)
	{
		LOG_D("Find partition[%s] not found.", dest_part_name);
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __exit_rollback_copy;
	}

	source_part = fal_partition_find(source_part_name);
	if (source_part == NULL)
	{
		LOG_D("Find partition[%s] not found.", source_part_name);
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __exit_rollback_copy;
	}

    cache_buf = malloc(THE_NOR_FLASH_GRANULARITY);
    if (cache_buf == NULL)
    {
        LOG_D("Not enough memory for rollback_copy.");
        fota_res = FOTA_NO_MEM_ERR;
        goto __exit_rollback_copy;
    }
	
	if (fal_partition_erase_all(dest_part) < 0)
    {
		LOG_D("Erase partition[%s] failed.", dest_part_name);
		fota_res = FOTA_PART_ERASE_ERR;
		goto __exit_rollback_copy;
    }

	/*start cpoy*/
	write_size = source_part->len < dest_part->len ? source_part->len : dest_part->len;
	
	while(write_offset < write_size)
	{
		uint32_t write_len;
		if ((write_offset + THE_NOR_FLASH_GRANULARITY) > write_size)
		{
			write_len = write_size - write_offset;
		}
		else
		{
			write_len = THE_NOR_FLASH_GRANULARITY;
		}
		
		if (fal_partition_read(source_part, write_offset, cache_buf, write_len) < 0)
		{
			LOG_I("Read partition[%s] failed.", source_part_name);
			fota_res = FOTA_PART_READ_ERR;
			goto __exit_rollback_copy;
		}

		if (fal_partition_write(dest_part, write_offset, (const uint8_t *)cache_buf, write_len) < 0)
		{
			LOG_I("Write partition[%s] failed.", dest_part_name);
			fota_res = FOTA_PART_WRITE_ERR;
			goto __exit_rollback_copy;
		}

		write_offset += THE_NOR_FLASH_GRANULARITY;
	}

__exit_rollback_copy:
	if (cache_buf)
		free(cache_buf);
	
	if (fota_res != FOTA_NO_ERR)
    {
        LOG_I("Copy %s to %s failed!",source_part_name, dest_part_name);
    }
	else
    {
        LOG_I("Copy %s to %s Success!",source_part_name, dest_part_name);
    }

	return fota_res;
}
#endif

#if defined(FOTA_YMODEM)
__weak int fota_ymodem_check(void)
{
	return 0;
}

static int fota_ymodem_ota(void)
{
	extern uint8_t temp_data;
	extern UART_HandleTypeDef FOTA_YMODEM_HUART;
	HAL_UART_Receive_IT(&FOTA_YMODEM_HUART, (uint8_t *) &temp_data, 1);
	extern uint16_t ymodem_receive(void);
	if (ymodem_receive() >> 15 & 1)
	{
		printf("\r\n------Ymodem Download Error, Boot Will Start After 5s ------\r\n");
		HAL_Delay(5000);
		__set_FAULTMASK(1);  // 关闭所有中断
		NVIC_SystemReset();  // 复位
	}

	return 0;	
}
#endif

int fota_part_fw_verify(const char *part_name)
{
#define FOTA_CRC_BUFF_SIZE		4096
#define FOTA_CRC_INIT_VAL		0xffffffff
#define POLYNOMIAL				0x04c11db7

	int fota_res = FOTA_NO_ERR;
	const struct fal_partition *part;
	fota_part_head part_head;
	uint8_t *body_buf = NULL;
	uint32_t body_crc = FOTA_CRC_INIT_VAL;
	uint32_t hdr_crc;

	if (part_name == NULL)
	{
		LOG_D("Invaild paramenter input!");
		fota_res = FOTA_GENERAL_ERR;
		goto __exit_partition_verify;
	}

	part = fal_partition_find(part_name);
	if (part == NULL)
	{		
		LOG_D("Partition[%s] not found.", part_name);
		fota_res = FOTA_GENERAL_ERR;
		goto __exit_partition_verify;
	}

	/* read the head of RBL files */
	if (fal_partition_read(part, 0, (uint8_t *)&part_head, sizeof(fota_part_head)) < 0)
	{
		LOG_D("Partition[%s] read error!", part->name);
		fota_res = FOTA_PART_READ_ERR;
		goto __exit_partition_verify;
	}

	extern void crc32_init(uint32_t poly);
	crc32_init(POLYNOMIAL);
	extern uint32_t fota_crc(uint8_t *buf, uint32_t len);
	hdr_crc = fota_crc((uint8_t *)&part_head, sizeof(fota_part_head) - 4);
	if (hdr_crc != part_head.head_crc)
	{
		LOG_D("Partition[%s] head CRC32 error!", part->name);
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __exit_partition_verify;
	}
	
	if (strcmp(part_head.type, "RBL") != 0)
	{
		LOG_D("Partition[%s] type[%s] not surport.", part->name, part_head.type);
		fota_res = FOTA_CHECK_FAILED;
		goto __exit_partition_verify;
	}

	if (fal_partition_find(part_head.app_part_name) == NULL)
	{
		LOG_D("Partition[%s] not found.", part_head.app_part_name);
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __exit_partition_verify;
	}

	body_buf = malloc(FOTA_CRC_BUFF_SIZE);
	if (body_buf == NULL)
	{
		LOG_D("Not enough memory for body CRC32 verify.");	
		fota_res = FOTA_NO_MEM_ERR;
		goto __exit_partition_verify;
	}

	for (int body_pos = 0; body_pos < part_head.com_size;)
	{	
		int body_read_len = fal_partition_read(part, sizeof(fota_part_head) + body_pos, body_buf, FOTA_CRC_BUFF_SIZE);      
		if (body_read_len > 0) 
		{
            if ((body_pos + body_read_len) > part_head.com_size)
            {
                body_read_len = part_head.com_size - body_pos;
            }
            
			extern uint32_t fota_step_crc(uint32_t crc, uint8_t *buf, uint32_t len);
			body_crc = fota_step_crc(body_crc, body_buf, body_read_len);	
			body_pos = body_pos + body_read_len;
		}
		else
		{
			LOG_D("Partition[%s] read error!", part->name);		
			fota_res = FOTA_PART_READ_ERR;
			goto __exit_partition_verify;
		}
	}
	body_crc = body_crc ^ FOTA_CRC_INIT_VAL;
	
	if (body_crc != part_head.code_crc)
	{
		LOG_D("Partition[%s] firmware integrity verify failed.", part->name);		
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __exit_partition_verify;
	}

__exit_partition_verify:
	if (fota_res == FOTA_NO_ERR)
	{
		// enter_critical();
		memcpy(&rbl_fota_part_head, &part_head, sizeof(fota_part_head));
		// exit_critical();

		LOG_D("partition[%s] verify success!", part->name);
	}
	else
	{
		// enter_critical();
		memset(&rbl_fota_part_head, 0x0, sizeof(fota_part_head));
		// exit_critical();

        if (part_name != NULL)
        {
            LOG_D("Partition[%s] verify failed!", part->name);
        }
	}

	if (body_buf)
		free(body_buf);
	
	return fota_res;
}

int fota_check_upgrade(void)
{
	int is_upgrade = 0;

	if (strcmp(rbl_fota_part_head.download_version, rbl_fota_part_head.current_version) != 0)
	{
		is_upgrade = 1;
		LOG_D("Application need upgrade.");
		goto __exit_check_upgrade;
	}

__exit_check_upgrade:
	return is_upgrade;
}

int fota_erase_app_part(void)
{
	int fota_res = FOTA_NO_ERR;
	const struct fal_partition *part;

	part = fal_partition_find(rbl_fota_part_head.app_part_name);
	if (part == NULL)
	{
		LOG_D("Erase partition[%s] not found.", rbl_fota_part_head.app_part_name);
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __exit_partition_erase;
	}
    
    LOG_I("Partition[%s] erase start:", part->name);
	if (fal_partition_erase_all(part) < 0)
	{
		LOG_D("Partition[%s] erase failed!", part->name);
		fota_res = FOTA_PART_ERASE_ERR;
		goto __exit_partition_erase;
	}

__exit_partition_erase:
	if (fota_res == FOTA_NO_ERR)
	{
		LOG_D("Partition[%s] erase %ld bytes success!", part->name, rbl_fota_part_head.raw_size);
	}
	return fota_res;
}

int fota_write_app_part(int fw_pos, uint8_t *fw_buf, int fw_len)
{
	int fota_res = FOTA_NO_ERR;
	const struct fal_partition *part;

	part = fal_partition_find(rbl_fota_part_head.app_part_name);
	if (part == NULL)
	{
		LOG_D("partition[%s] not found.", rbl_fota_part_head.app_part_name);
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __partition_write_exit;
	}

	if (fal_partition_write(part, fw_pos, fw_buf, fw_len) < 0)
	{
		LOG_D("Partition[%s] write failed!", part->name);
		fota_res = FOTA_PART_WRITE_ERR;
		goto __partition_write_exit;
	}
__partition_write_exit:
	if (fota_res == FOTA_NO_ERR)
	{
		LOG_D("Partition[%s] write %d bytes success!", part->name, fw_len);
	}
	return fota_res;
}

static int fota_read_part(const struct fal_partition *part, int read_pos, tiny_aes_context *aes_ctx, uint8_t *aes_iv, uint8_t *decrypt_buf, uint32_t decrypt_len)
{
	int fota_err = FOTA_NO_ERR;
	uint8_t *encrypt_buf = NULL;

	if ((part == NULL) || (decrypt_buf == NULL) 
		|| (decrypt_len % 16 != 0) || (decrypt_len > FOTA_ALGO_BUFF_SIZE))
	{
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_read_decrypt;
	}

	memset(decrypt_buf, 0x0, decrypt_len);

	/* Not use AES256 algorithm */
	if (aes_ctx == NULL || aes_iv == NULL)
	{
		fota_err = fal_partition_read(part, sizeof(fota_part_head) + read_pos, decrypt_buf, decrypt_len);
		if (fota_err <= 0)
		{
			fota_err = FOTA_PART_READ_ERR;
		}
		goto __exit_read_decrypt;
	}

	encrypt_buf = malloc(decrypt_len);
	if (encrypt_buf == NULL)
	{
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_read_decrypt;
	}
	memset(encrypt_buf, 0x0, decrypt_len);

	fota_err = fal_partition_read(part, sizeof(fota_part_head) + read_pos, encrypt_buf, decrypt_len);
	if (fota_err <= 0 || fota_err % 16 != 0)
	{
		fota_err = FOTA_PART_READ_ERR;
		goto __exit_read_decrypt;
	}

	tiny_aes_crypt_cbc(aes_ctx, AES_DECRYPT, fota_err, aes_iv, encrypt_buf, decrypt_buf);
__exit_read_decrypt:
	if (encrypt_buf)
		free(encrypt_buf);
	
	return fota_err;
}

/*uint32_t FNV_SEED = 2166136261u;
 *fota_calc_hash_code(data, 4096, 2166136261u)*/
static uint32_t fota_fnv1a_r(uint8_t onebyte, uint32_t hash)
{
    return (onebyte ^ hash) * 16777619;
}

static uint32_t fota_calc_hash_code(uint8_t *data, uint32_t len, uint32_t hash)
{
    for (int i = 0; i < len; i++)
    {
        hash = fota_fnv1a_r(data[i], hash);
    }
    return hash;
}

int fota_upgrade_verify(const char *part_name)
{
    int fota_err = FOTA_NO_ERR;

	const struct fal_partition *part;
	fota_part_head_t part_head = NULL;
	
	tiny_aes_context *aes_ctx = NULL;
	uint8_t *aes_iv = NULL;
	uint8_t *crypt_buf = NULL;
	
	int fw_raw_pos = 0;
	int fw_raw_len = 0;
	uint32_t total_copy_size = 0;
	uint32_t hash_code = 2166136261u;

	uint8_t block_hdr_buf[FOTA_BLOCK_HEADER_SIZE];	
	uint32_t block_hdr_pos = FOTA_ALGO_BUFF_SIZE;
	uint32_t block_size = 0;
	uint32_t dcprs_size = 0;
	
	qlz_state_decompress *dcprs_state = NULL;
	uint8_t *cmprs_buff = NULL;
	uint8_t *dcprs_buff = NULL;
	uint32_t padding_size = 0;

	if (part_name == NULL)
	{
		LOG_D("Invaild paramenter input!");
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_upgrade_verify;
	}

	part = fal_partition_find(part_name);
	if (part == NULL)
	{		
		LOG_D("Upgrade partition[%s] not found.", part_name);
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_upgrade_verify;
	}
	
	part_head = &rbl_fota_part_head;

	crypt_buf = malloc(FOTA_ALGO_BUFF_SIZE);
	if (crypt_buf == NULL)
	{
		LOG_D("Not enough memory for firmware buffer.");
		fota_err = FOTA_NO_MEM_ERR;
		goto __exit_upgrade_verify;
	}

	/* AES256 algorithm enable */
	if ((part_head->fota_algo & FOTA_CRYPT_STAT_MASK) == FOTA_CRYPT_ALGO_AES256)
	{
		aes_ctx = malloc(sizeof(tiny_aes_context));	
		aes_iv = malloc(strlen(FOTA_ALGO_AES_IV) + 1);		
		if (aes_ctx == NULL || aes_iv == NULL)
		{
			LOG_D("Not enough memory for firmware hash verify.");
			fota_err = FOTA_NO_MEM_ERR;
			goto __exit_upgrade_verify;
		}

		memset(aes_iv, 0x0, strlen(FOTA_ALGO_AES_IV) + 1);
		memcpy(aes_iv, FOTA_ALGO_AES_IV, strlen(FOTA_ALGO_AES_IV));
		tiny_aes_setkey_dec(aes_ctx, (uint8_t *)FOTA_ALGO_AES_KEY, 256);
	}
	else if ((part_head->fota_algo & FOTA_CRYPT_STAT_MASK) == FOTA_CRYPT_ALGO_XOR)
	{
		LOG_I("Not surpport XOR.");
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_upgrade_verify;
	}
	
	/* If enable fastlz compress function */	
	if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_FASTLZ) 
	{
		cmprs_buff = malloc(FOTA_CMPRS_BUFFER_SIZE + FOTA_FASTLZ_BUFFER_PADDING);
		dcprs_buff = malloc(FOTA_CMPRS_BUFFER_SIZE);	
		if (cmprs_buff == NULL || dcprs_buff == NULL)
		{
			LOG_D("Not enough memory for firmware hash verify.");
			fota_err = FOTA_NO_MEM_ERR;
			goto __exit_upgrade_verify;
		}

		padding_size = FOTA_FASTLZ_BUFFER_PADDING;
	}
	else if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_QUICKLZ) 
	{
		cmprs_buff = malloc(FOTA_CMPRS_BUFFER_SIZE + FOTA_QUICKLZ_BUFFER_PADDING);
		dcprs_buff = malloc(FOTA_CMPRS_BUFFER_SIZE);	
		dcprs_state = malloc(sizeof(qlz_state_decompress));
		if (cmprs_buff == NULL || dcprs_buff == NULL || dcprs_state == NULL)
		{
			LOG_D("Not enough memory for firmware hash verify.");
			fota_err = FOTA_NO_MEM_ERR;
			goto __exit_upgrade_verify;
		}

		padding_size = FOTA_QUICKLZ_BUFFER_PADDING;
		memset(dcprs_state, 0x0, sizeof(qlz_state_decompress));
	}
	else if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_GZIP) 
	{
		LOG_I("Not surpport GZIP.");
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_upgrade_verify;
	}

	LOG_I("Start check %s hash_val", part->name);

	while (fw_raw_pos < part_head->com_size)
	{
		if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) != FOTA_CRYPT_ALGO_NONE) 
		{
			if (block_hdr_pos >= FOTA_ALGO_BUFF_SIZE)
			{
				fw_raw_len = fota_read_part(part, fw_raw_pos, aes_ctx, aes_iv, crypt_buf, FOTA_ALGO_BUFF_SIZE);
				if (fw_raw_len < 0)
				{
					LOG_D("AES256 algorithm failed.");
					fota_err = FOTA_PART_READ_ERR;
					goto __exit_upgrade_verify;
				}
				fw_raw_pos += fw_raw_len;

				memcpy(block_hdr_buf, crypt_buf, FOTA_BLOCK_HEADER_SIZE);
				block_size = block_hdr_buf[0] * (1 << 24) + block_hdr_buf[1] * (1 << 16) + block_hdr_buf[2] * (1 << 8) + block_hdr_buf[3];
				if (block_size >= malloc_usable_size(cmprs_buff))
				{
					LOG_D("AES256 Get block_size[%ld] too large.", block_size);
					fota_err = FOTA_GENERAL_ERR;
					goto __exit_upgrade_verify;
				}
				memset(cmprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE + padding_size);
				memcpy(cmprs_buff, &crypt_buf[FOTA_BLOCK_HEADER_SIZE], block_size);

				block_hdr_pos = FOTA_BLOCK_HEADER_SIZE + block_size;
			}
			else
			{
				uint8_t hdr_tmp_pos = 0;
				while (block_hdr_pos < FOTA_ALGO_BUFF_SIZE)
				{
					if (hdr_tmp_pos < FOTA_BLOCK_HEADER_SIZE)
					{
						block_hdr_buf[hdr_tmp_pos++] = crypt_buf[block_hdr_pos++];
					}
					else
					{
						block_size = block_hdr_buf[0] * (1 << 24) + block_hdr_buf[1] * (1 << 16) + block_hdr_buf[2] * (1 << 8) + block_hdr_buf[3];
						if (block_size >= malloc_usable_size(cmprs_buff))
						{
							LOG_D("AES256 Get block_size[%ld] too large.", block_size);
							fota_err = FOTA_GENERAL_ERR;
							goto __exit_upgrade_verify;
						}
						memset(cmprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE + padding_size);
						if (block_size > (FOTA_ALGO_BUFF_SIZE - block_hdr_pos))
						{								
							memcpy(cmprs_buff, &crypt_buf[block_hdr_pos], (FOTA_ALGO_BUFF_SIZE - block_hdr_pos));
							fw_raw_len = fota_read_part(part, fw_raw_pos, aes_ctx, aes_iv, crypt_buf, FOTA_ALGO_BUFF_SIZE);
							if (fw_raw_len < 0)
							{
								LOG_D("AES256 algorithm failed.");
								fota_err = FOTA_PART_READ_ERR;
								goto __exit_upgrade_verify;
							}
							fw_raw_pos += fw_raw_len;

							memcpy(&cmprs_buff[FOTA_ALGO_BUFF_SIZE - block_hdr_pos], &crypt_buf[0], (block_size +  block_hdr_pos) - FOTA_ALGO_BUFF_SIZE);
							block_hdr_pos = (block_size +  block_hdr_pos) - FOTA_ALGO_BUFF_SIZE;
						}
						else
						{
							memcpy(cmprs_buff, &crypt_buf[block_hdr_pos], block_size);
							block_hdr_pos = block_hdr_pos + block_size;
						}						
						break;
					}
				}
				
				if (hdr_tmp_pos < FOTA_BLOCK_HEADER_SIZE)
				{				
					fw_raw_len = fota_read_part(part, fw_raw_pos, aes_ctx, aes_iv, crypt_buf, FOTA_ALGO_BUFF_SIZE);
					if (fw_raw_len < 0)
					{
						LOG_D("AES256 algorithm failed.");
						fota_err = FOTA_PART_READ_ERR;
						goto __exit_upgrade_verify;
					}
					fw_raw_pos += fw_raw_len;

					block_hdr_pos = 0;
					while (hdr_tmp_pos < FOTA_BLOCK_HEADER_SIZE)
					{
						block_hdr_buf[hdr_tmp_pos++] = crypt_buf[block_hdr_pos++];
					}
					block_size = block_hdr_buf[0] * (1 << 24) + block_hdr_buf[1] * (1 << 16) + block_hdr_buf[2] * (1 << 8) + block_hdr_buf[3];
					if (block_size >= malloc_usable_size(cmprs_buff))
					{
						LOG_D("AES256 Get block_size[%ld] too large.", block_size);
						fota_err = FOTA_GENERAL_ERR;
						goto __exit_upgrade_verify;
					}
					memset(cmprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE + padding_size);
					memcpy(cmprs_buff, &crypt_buf[block_hdr_pos], block_size);

					block_hdr_pos = (block_hdr_pos + block_size) % FOTA_ALGO_BUFF_SIZE;
				}
			}

			memset(dcprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE);		
			if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_FASTLZ) 
			{
				dcprs_size = fastlz_decompress((const void *)&cmprs_buff[0], block_size, &dcprs_buff[0], FOTA_CMPRS_BUFFER_SIZE);
			}
			else if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_QUICKLZ) 
			{
				dcprs_size = qlz_decompress((const char *)&cmprs_buff[0], &dcprs_buff[0], dcprs_state);
			}
			
			if (dcprs_size <= 0)
			{
				LOG_D("Decompress failed: %ld.", dcprs_size);
				fota_err = FOTA_GENERAL_ERR;
				goto __exit_upgrade_verify;
			}

			if (total_copy_size + dcprs_size > part_head->raw_size)
			{
				dcprs_size = part_head->raw_size - total_copy_size;
			}
            /*Hash code calculation */
            hash_code = fota_calc_hash_code(dcprs_buff, dcprs_size, hash_code);

			total_copy_size += dcprs_size;
		}
		/* no compress option */
		else
		{
			fw_raw_len = fota_read_part(part, fw_raw_pos, aes_ctx, aes_iv, crypt_buf, FOTA_ALGO_BUFF_SIZE);
			if (fw_raw_len < 0)
			{
				LOG_D("AES256 algorithm failed.");
				fota_err = FOTA_PART_READ_ERR;
				goto __exit_upgrade_verify;
			}		
			fw_raw_pos += fw_raw_len;

			if (total_copy_size + fw_raw_len > part_head->raw_size)
			{
				fw_raw_len = part_head->raw_size - total_copy_size;
			}
            /*Hash code calculation */
            hash_code = fota_calc_hash_code(crypt_buf, fw_raw_len, hash_code);

			total_copy_size += fw_raw_len;
			static uint8_t count = 0;
			for (size_t i = 0; i < count; i++)
			{
				printf("#");
			}
			count++;
			printf("#\r\n");
		}
	}

	/* it has compress option */
	if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) != FOTA_CRYPT_ALGO_NONE)
	{
        while (total_copy_size < part_head->raw_size)
        {
            if ((block_hdr_pos < fw_raw_len) && ((fw_raw_len - block_hdr_pos) > FOTA_BLOCK_HEADER_SIZE))
            {
                memcpy(block_hdr_buf, &crypt_buf[block_hdr_pos], FOTA_BLOCK_HEADER_SIZE);
                block_size = block_hdr_buf[0] * (1 << 24) + block_hdr_buf[1] * (1 << 16) + block_hdr_buf[2] * (1 << 8) + block_hdr_buf[3];
				if (block_size >= malloc_usable_size(cmprs_buff))
				{
					LOG_D("AES256 Get block_size[%ld] too large.", block_size);
					fota_err = FOTA_GENERAL_ERR;
					goto __exit_upgrade_verify;
				}
                if ((fw_raw_len - block_hdr_pos - FOTA_BLOCK_HEADER_SIZE) >= block_size)
                {
                    memset(cmprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE + padding_size);				
                    memcpy(cmprs_buff, &crypt_buf[block_hdr_pos + FOTA_BLOCK_HEADER_SIZE], block_size);
                    memset(dcprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE);
                    
                    block_hdr_pos += (block_size + FOTA_BLOCK_HEADER_SIZE);

                    if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_FASTLZ) 
                    {
                        dcprs_size = fastlz_decompress((const void *)&cmprs_buff[0], block_size, &dcprs_buff[0], FOTA_CMPRS_BUFFER_SIZE);
                    }
                    else if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_QUICKLZ) 
                    {
                        dcprs_size = qlz_decompress((const char *)&cmprs_buff[0], &dcprs_buff[0], dcprs_state);
                    }
                
                    if (dcprs_size <= 0)
                    {
                        LOG_D("Decompress failed: %ld.", dcprs_size);
                        fota_err = FOTA_GENERAL_ERR;
                        goto __exit_upgrade_verify;
                    }

                    if (total_copy_size + dcprs_size > part_head->raw_size)
					{
						dcprs_size = part_head->raw_size - total_copy_size;
					}
                    /*Hash code calculation */
                    hash_code = fota_calc_hash_code(dcprs_buff, dcprs_size, hash_code);

                    total_copy_size += dcprs_size;
                }
                else
                {
                    break;
                }                                
            }
            else
            {
                break;
            }
        }
	}

	/* 有可能两个值不相等,因为AES需要填充16字节整数,但最后的解密解压值的代码数量必须是大于等于raw_size */
	/* 比较好的方法是做一个校验,目前打包软件的HASH_CODE算法不知道 */
	if (total_copy_size < part_head->raw_size)
	{
		LOG_D("Decompress check failed.");
		fota_err = FOTA_GENERAL_ERR;
	}
	/*比较计算出的hash值与生成的hash val是否相等*/
    if (hash_code != part_head->hash_val)
    {
        LOG_I("hash val check failed.");
        fota_err = FOTA_GENERAL_ERR;
    }

__exit_upgrade_verify:
	if (aes_ctx)
		free(aes_ctx);

	if (aes_iv)
		free(aes_iv);

	if (crypt_buf)
		free(crypt_buf);

	if (cmprs_buff)
		free(cmprs_buff);

	if (dcprs_buff)
		free(dcprs_buff);

	if (dcprs_state)
		free(dcprs_state);

	if (fota_err == FOTA_NO_ERR)
	{
        LOG_I("verify success, total %ld bytes", total_copy_size);
    }
    else
    {
        LOG_I("verify falied");
    }
	return fota_err;
}

int fota_upgrade(const char *part_name)
{
	int fota_err = FOTA_NO_ERR;

	const struct fal_partition *part;
	fota_part_head_t part_head = NULL;
	
	tiny_aes_context *aes_ctx = NULL;
	uint8_t *aes_iv = NULL;
	uint8_t *crypt_buf = NULL;
	
	int fw_raw_pos = 0;
	int fw_raw_len = 0;
	uint32_t total_copy_size = 0;

	uint8_t block_hdr_buf[FOTA_BLOCK_HEADER_SIZE];	
	uint32_t block_hdr_pos = FOTA_ALGO_BUFF_SIZE;
	uint32_t block_size = 0;
	uint32_t dcprs_size = 0;
	
	qlz_state_decompress *dcprs_state = NULL;
	uint8_t *cmprs_buff = NULL;
	uint8_t *dcprs_buff = NULL;
	uint32_t padding_size = 0;

	if (part_name == NULL)
	{
		LOG_D("Invaild paramenter input!");
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_upgrade;
	}

	part = fal_partition_find(part_name);
	if (part == NULL)
	{		
		LOG_D("Upgrade partition[%s] not found.", part_name);
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_upgrade;
	}
	
	/* Application partition erase */
	fota_err = fota_erase_app_part();
	if (fota_err != FOTA_NO_ERR)
	{
		goto __exit_upgrade;
	}

	/* fota_erase_app_part() has check fota_part_head vaild already */
	part_head = &rbl_fota_part_head;

	crypt_buf = malloc(FOTA_ALGO_BUFF_SIZE);
	if (crypt_buf == NULL)
	{
		LOG_D("Not enough memory for firmware buffer.");
		fota_err = FOTA_NO_MEM_ERR;
		goto __exit_upgrade;
	}

	/* write head to app */
	if (fal_partition_read(part, 0, crypt_buf, sizeof(fota_part_head)) < 0)
	{
		LOG_I("Read partition[%s] failed.", part_name);
		fota_err = FOTA_PART_READ_ERR;
		goto __exit_upgrade;
	}

	if (fota_write_app_part(0, crypt_buf, sizeof(fota_part_head)) < 0)
	{
		LOG_I("Write partition[%s] %dByte failed.", part_name, sizeof(fota_part_head));
		fota_err = FOTA_PART_WRITE_ERR;
		goto __exit_upgrade;
	}

	/* AES256 algorithm enable */
	if ((part_head->fota_algo & FOTA_CRYPT_STAT_MASK) == FOTA_CRYPT_ALGO_AES256)
	{
		aes_ctx = malloc(sizeof(tiny_aes_context));	
		aes_iv = malloc(strlen(FOTA_ALGO_AES_IV) + 1);		
		if (aes_ctx == NULL || aes_iv == NULL)
		{
			LOG_D("Not enough memory for firmware hash verify.");
			fota_err = FOTA_NO_MEM_ERR;
			goto __exit_upgrade;
		}

		memset(aes_iv, 0x0, strlen(FOTA_ALGO_AES_IV) + 1);
		memcpy(aes_iv, FOTA_ALGO_AES_IV, strlen(FOTA_ALGO_AES_IV));
		tiny_aes_setkey_dec(aes_ctx, (uint8_t *)FOTA_ALGO_AES_KEY, 256);
	}
	else if ((part_head->fota_algo & FOTA_CRYPT_STAT_MASK) == FOTA_CRYPT_ALGO_XOR)
	{
		LOG_I("Not surpport XOR.");
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_upgrade;
	}
	
	/* If enable fastlz compress function */	
	if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_FASTLZ) 
	{
		cmprs_buff = malloc(FOTA_CMPRS_BUFFER_SIZE + FOTA_FASTLZ_BUFFER_PADDING);
		dcprs_buff = malloc(FOTA_CMPRS_BUFFER_SIZE);	
		if (cmprs_buff == NULL || dcprs_buff == NULL)
		{
			LOG_D("Not enough memory for firmware hash verify.");
			fota_err = FOTA_NO_MEM_ERR;
			goto __exit_upgrade;
		}

		padding_size = FOTA_FASTLZ_BUFFER_PADDING;
	}
	else if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_QUICKLZ) 
	{
		cmprs_buff = malloc(FOTA_CMPRS_BUFFER_SIZE + FOTA_QUICKLZ_BUFFER_PADDING);
		dcprs_buff = malloc(FOTA_CMPRS_BUFFER_SIZE);	
		dcprs_state = malloc(sizeof(qlz_state_decompress));
		if (cmprs_buff == NULL || dcprs_buff == NULL || dcprs_state == NULL)
		{
			LOG_D("Not enough memory for firmware hash verify.");
			fota_err = FOTA_NO_MEM_ERR;
			goto __exit_upgrade;
		}

		padding_size = FOTA_QUICKLZ_BUFFER_PADDING;
		memset(dcprs_state, 0x0, sizeof(qlz_state_decompress));
	}
	else if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_GZIP) 
	{
		LOG_I("Not surpport GZIP.");
		fota_err = FOTA_GENERAL_ERR;
		goto __exit_upgrade;
	}

	LOG_I("Start to copy firmware from %s to %s partition:", part->name, part_head->app_part_name);
	while (fw_raw_pos < part_head->com_size)
	{
		if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) != FOTA_CRYPT_ALGO_NONE) 
		{		
			if (block_hdr_pos >= FOTA_ALGO_BUFF_SIZE)
			{
				fw_raw_len = fota_read_part(part, fw_raw_pos, aes_ctx, aes_iv, crypt_buf, FOTA_ALGO_BUFF_SIZE);
				if (fw_raw_len < 0)
				{
					LOG_D("AES256 algorithm failed.");
					fota_err = FOTA_PART_READ_ERR;
					goto __exit_upgrade;
				}
				fw_raw_pos += fw_raw_len;

				memcpy(block_hdr_buf, crypt_buf, FOTA_BLOCK_HEADER_SIZE);
				block_size = block_hdr_buf[0] * (1 << 24) + block_hdr_buf[1] * (1 << 16) + block_hdr_buf[2] * (1 << 8) + block_hdr_buf[3];
				if (block_size >= malloc_usable_size(cmprs_buff))
				{
					LOG_D("AES256 Get block_size[%ld] too large.", block_size);
					fota_err = FOTA_GENERAL_ERR;
					goto __exit_upgrade;
				}
				memset(cmprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE + padding_size);
				memcpy(cmprs_buff, &crypt_buf[FOTA_BLOCK_HEADER_SIZE], block_size);

				block_hdr_pos = FOTA_BLOCK_HEADER_SIZE + block_size;
			}
			else
			{
				uint8_t hdr_tmp_pos = 0;
				while (block_hdr_pos < FOTA_ALGO_BUFF_SIZE)
				{
					if (hdr_tmp_pos < FOTA_BLOCK_HEADER_SIZE)
					{
						block_hdr_buf[hdr_tmp_pos++] = crypt_buf[block_hdr_pos++];
					}
					else
					{
						block_size = block_hdr_buf[0] * (1 << 24) + block_hdr_buf[1] * (1 << 16) + block_hdr_buf[2] * (1 << 8) + block_hdr_buf[3];
						if (block_size >= malloc_usable_size(cmprs_buff))
						{
							LOG_D("AES256 Get block_size[%ld] too large.", block_size);
							fota_err = FOTA_GENERAL_ERR;
							goto __exit_upgrade;
						}
						memset(cmprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE + padding_size);
						if (block_size > (FOTA_ALGO_BUFF_SIZE - block_hdr_pos))
						{								
							memcpy(cmprs_buff, &crypt_buf[block_hdr_pos], (FOTA_ALGO_BUFF_SIZE - block_hdr_pos));
							fw_raw_len = fota_read_part(part, fw_raw_pos, aes_ctx, aes_iv, crypt_buf, FOTA_ALGO_BUFF_SIZE);
							if (fw_raw_len < 0)
							{
								LOG_D("AES256 algorithm failed.");
								fota_err = FOTA_PART_READ_ERR;
								goto __exit_upgrade;
							}
							fw_raw_pos += fw_raw_len;

							memcpy(&cmprs_buff[FOTA_ALGO_BUFF_SIZE - block_hdr_pos], &crypt_buf[0], (block_size +  block_hdr_pos) - FOTA_ALGO_BUFF_SIZE);
							block_hdr_pos = (block_size +  block_hdr_pos) - FOTA_ALGO_BUFF_SIZE;
						}
						else
						{
							memcpy(cmprs_buff, &crypt_buf[block_hdr_pos], block_size);
							block_hdr_pos = block_hdr_pos + block_size;
						}						
						break;
					}
				}
				
				if (hdr_tmp_pos < FOTA_BLOCK_HEADER_SIZE)
				{				
					fw_raw_len = fota_read_part(part, fw_raw_pos, aes_ctx, aes_iv, crypt_buf, FOTA_ALGO_BUFF_SIZE);
					if (fw_raw_len < 0)
					{
						LOG_D("AES256 algorithm failed.");
						fota_err = FOTA_PART_READ_ERR;
						goto __exit_upgrade;
					}
					fw_raw_pos += fw_raw_len;

					block_hdr_pos = 0;
					while (hdr_tmp_pos < FOTA_BLOCK_HEADER_SIZE)
					{
						block_hdr_buf[hdr_tmp_pos++] = crypt_buf[block_hdr_pos++];
					}
					block_size = block_hdr_buf[0] * (1 << 24) + block_hdr_buf[1] * (1 << 16) + block_hdr_buf[2] * (1 << 8) + block_hdr_buf[3];
					if (block_size >= malloc_usable_size(cmprs_buff))
					{
						LOG_D("AES256 Get block_size[%ld] too large.", block_size);
						fota_err = FOTA_GENERAL_ERR;
						goto __exit_upgrade;
					}
					memset(cmprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE + padding_size);
					memcpy(cmprs_buff, &crypt_buf[block_hdr_pos], block_size);

					block_hdr_pos = (block_hdr_pos + block_size) % FOTA_ALGO_BUFF_SIZE;
				}
			}

			memset(dcprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE);		
			if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_FASTLZ) 
			{
				dcprs_size = fastlz_decompress((const void *)&cmprs_buff[0], block_size, &dcprs_buff[0], FOTA_CMPRS_BUFFER_SIZE);
			}
			else if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_QUICKLZ) 
			{
				dcprs_size = qlz_decompress((const char *)&cmprs_buff[0], &dcprs_buff[0], dcprs_state);
			}
			
			if (dcprs_size <= 0)
			{
				LOG_D("Decompress failed: %ld.", dcprs_size);
				fota_err = FOTA_GENERAL_ERR;
				goto __exit_upgrade;
			}

			if (fota_write_app_part(total_copy_size  + APP_HEADER_LEN, dcprs_buff, dcprs_size) < 0)
			{
				fota_err = FOTA_COPY_FAILED;
				goto __exit_upgrade;
			}

			total_copy_size += dcprs_size;
		}
		/* no compress option */
		else
		{
			fw_raw_len = fota_read_part(part, fw_raw_pos, aes_ctx, aes_iv, crypt_buf, FOTA_ALGO_BUFF_SIZE);
			if (fw_raw_len < 0)
			{
				LOG_D("AES256 algorithm failed.");
				fota_err = FOTA_PART_READ_ERR;
				goto __exit_upgrade;
			}		
			fw_raw_pos += fw_raw_len;

			if (fota_write_app_part(total_copy_size + APP_HEADER_LEN, crypt_buf, fw_raw_len) < 0)
			{
				fota_err = FOTA_COPY_FAILED;
				goto __exit_upgrade;
			}
			
			total_copy_size += fw_raw_len;
		}
	}

	/* it has compress option */
	if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) != FOTA_CRYPT_ALGO_NONE)
	{
        while (total_copy_size < part_head->raw_size)
        {
            if ((block_hdr_pos < fw_raw_len) && ((fw_raw_len - block_hdr_pos) > FOTA_BLOCK_HEADER_SIZE))
            {
                memcpy(block_hdr_buf, &crypt_buf[block_hdr_pos], FOTA_BLOCK_HEADER_SIZE);
                block_size = block_hdr_buf[0] * (1 << 24) + block_hdr_buf[1] * (1 << 16) + block_hdr_buf[2] * (1 << 8) + block_hdr_buf[3];
				if (block_size >= malloc_usable_size(cmprs_buff))
				{
					LOG_D("AES256 Get block_size[%ld] too large.", block_size);
					fota_err = FOTA_GENERAL_ERR;
					goto __exit_upgrade;
				}
                if ((fw_raw_len - block_hdr_pos - FOTA_BLOCK_HEADER_SIZE) >= block_size)
                {
                    memset(cmprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE + padding_size);				
                    memcpy(cmprs_buff, &crypt_buf[block_hdr_pos + FOTA_BLOCK_HEADER_SIZE], block_size);
                    memset(dcprs_buff, 0x0, FOTA_CMPRS_BUFFER_SIZE);
                    
                    block_hdr_pos += (block_size + FOTA_BLOCK_HEADER_SIZE);

                    if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_FASTLZ) 
                    {
                        dcprs_size = fastlz_decompress((const void *)&cmprs_buff[0], block_size, &dcprs_buff[0], FOTA_CMPRS_BUFFER_SIZE);
                    }
                    else if ((part_head->fota_algo & FOTA_CMPRS_STAT_MASK) == FOTA_CMPRS_ALGO_QUICKLZ) 
                    {
                        dcprs_size = qlz_decompress((const char *)&cmprs_buff[0], &dcprs_buff[0], dcprs_state);
                    }
                
                    if (dcprs_size <= 0)
                    {
                        LOG_D("Decompress failed: %ld.", dcprs_size);
                        fota_err = FOTA_GENERAL_ERR;
                        goto __exit_upgrade;
                    }

                    if (fota_write_app_part(total_copy_size + APP_HEADER_LEN, dcprs_buff, dcprs_size) < 0)
                    {
                        fota_err = FOTA_COPY_FAILED;
                        goto __exit_upgrade;
                    }

                    total_copy_size += dcprs_size;
                }
                else
                {
                    break;
                }                                
            }
            else
            {
                break;
            }
        }
	}
    printf("\r\n");

	/* 有可能两个值不相等,因为AES需要填充16字节整数,但最后的解密解压值的代码数量必须是大于等于raw_size */
	/* 比较好的方法是做一个校验,目前打包软件的HASH_CODE算法不知道 */
	if (total_copy_size < part_head->raw_size)
	{
		LOG_D("Decompress check failed.");
		fota_err = FOTA_GENERAL_ERR;
	}

__exit_upgrade:
	if (aes_ctx)
		free(aes_ctx);

	if (aes_iv)
		free(aes_iv);

	if (crypt_buf)
		free(crypt_buf);

	if (cmprs_buff)
		free(cmprs_buff);

	if (dcprs_buff)
		free(dcprs_buff);

	if (dcprs_state)
		free(dcprs_state);

	if (fota_err == FOTA_NO_ERR)
	{
    	LOG_I("Upgrade success, total %ld bytes.", total_copy_size);
	}
	return fota_err;
}

int fota_copy_version(const char *part_name)
{
#define THE_NOR_FLASH_GRANULARITY		4096

	int fota_res = FOTA_NO_ERR;
	const struct fal_partition *part;
    
    fota_part_head_t part_head = NULL;
    uint8_t *cache_buf = NULL;

	part = fal_partition_find(part_name);
	if (part == NULL)
	{
		LOG_D("Find partition[%s] not found.", part_name);
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __exit_copy_version;
	}
	    
    cache_buf = malloc(THE_NOR_FLASH_GRANULARITY);
    if (cache_buf == NULL)
    {
        LOG_D("Not enough memory for head erase.");
        fota_res = FOTA_NO_MEM_ERR;
        goto __exit_copy_version;
    }
    part_head = (fota_part_head_t)cache_buf;
	
	if (fal_partition_read(part, 0, cache_buf, THE_NOR_FLASH_GRANULARITY) < 0)
	{
		LOG_I("Read partition[%s] failed.", part_name);
		fota_res = FOTA_PART_READ_ERR;
		goto __exit_copy_version;
	}
	
	memcpy(part_head->current_version, part_head->download_version, sizeof(part_head->current_version));
	extern uint32_t fota_crc(uint8_t *buf, uint32_t len);
	part_head->head_crc = fota_crc((uint8_t *)part_head, sizeof(fota_part_head) - 4);
	
    if (fal_partition_erase(part, 0, THE_NOR_FLASH_GRANULARITY) < 0)
    {
		LOG_D("Erase partition[%s] failed.", part_name);
		fota_res = FOTA_PART_ERASE_ERR;
		goto __exit_copy_version;
    }
	
	if (fal_partition_write(part, 0, (const uint8_t *)cache_buf, THE_NOR_FLASH_GRANULARITY) < 0)
	{
		LOG_I("Write partition[%s] failed.", part_name);
		fota_res = FOTA_PART_WRITE_ERR;
		goto __exit_copy_version;
	}
__exit_copy_version:
	if (cache_buf)
		free(cache_buf);
	
	if (fota_res != FOTA_NO_ERR)
    {
        LOG_I("Copy firmware version failed!");
    }
	else
    {
        LOG_I("Copy firmware version Success!");
#ifdef FOTA_INSTALL_RESTART
		__set_FAULTMASK(1);
		HAL_NVIC_SystemReset();  // 复位
#endif
    }

	return fota_res;
}

int fota_check_application(void)
{
#define THE_NOR_FLASH_GRANULARITY		4096

	int fota_res = FOTA_NO_ERR;
	uint32_t read_offset = 0;
	const struct fal_partition *part;

	fota_part_head_t part_head = NULL;
	uint8_t *cache_buf = NULL;

	uint32_t hash_code = 2166136261u;
	uint8_t *app_buff = NULL;

	part = fal_partition_find(FOTA_APP_PART_NAME);
	if (part == NULL)
	{
		LOG_D("Find partition[%s] not found.", FOTA_APP_PART_NAME);
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __exit_check_app;
	}

	cache_buf = malloc(THE_NOR_FLASH_GRANULARITY);
    if (cache_buf == NULL)
    {
        LOG_D("Not enough memory for head erase.");
		fota_res = FOTA_NO_MEM_ERR;
		goto __exit_check_app;
    }
    part_head = (fota_part_head_t)cache_buf;

	if (fal_partition_read(part, 0, cache_buf, sizeof(fota_part_head)) < 0)
	{
		LOG_I("Read partition[%s] failed.", FOTA_APP_PART_NAME);
		fota_res = FOTA_PART_READ_ERR;
		goto __exit_check_app;
	}
	app_buff = malloc(FOTA_ALGO_BUFF_SIZE);
	if (app_buff == NULL)
    {
        LOG_D("Not enough memory for head erase.");
		fota_res = FOTA_NO_MEM_ERR;
		goto __exit_check_app;
    }

	while(read_offset < part_head->raw_size)
	{
		uint32_t read_len;
		if ((read_offset + THE_NOR_FLASH_GRANULARITY) > part_head->raw_size)
		{
			read_len = part_head->raw_size - read_offset;
		}
		else
		{
			read_len = THE_NOR_FLASH_GRANULARITY;
		}
		
		if (fal_partition_read(part, read_offset + APP_HEADER_LEN, app_buff, read_len) < 0)
		{
			LOG_I("Read partition[%s] failed.", FOTA_APP_PART_NAME);
			fota_res = FOTA_PART_READ_ERR;
			goto __exit_check_app;
		}

		hash_code = fota_calc_hash_code(app_buff, read_len, hash_code);

		read_offset += THE_NOR_FLASH_GRANULARITY;
	}

	if (hash_code != part_head->hash_val)
	{
		fota_res = FOTA_FW_VERIFY_FAILED;
		goto __exit_check_app;
	}
	
__exit_check_app:
	if (cache_buf)
		free(cache_buf);

	if (app_buff)
		free(app_buff);
	
	return fota_res;
}

static int fota_start_application(void)
{
	int fota_res = FOTA_NO_ERR;
	const struct fal_partition *part;
	uint32_t app_addr;

	part = fal_partition_find(FOTA_APP_PART_NAME);
	if (part == NULL)
	{		
		LOG_D("Partition[%s] not found.", rbl_fota_part_head.app_part_name);
		fota_res = FOTA_GENERAL_ERR;
		goto __exit_start_application;
	}

	app_addr = part->offset + 0x08000000 + APP_HEADER_LEN;
	LOG_DEBUG("app_addr 0x%08lx", app_addr);
	//判断是否为0x08XXXXXX.
	if (((*(volatile uint32_t *)(app_addr + 4)) & 0xff000000) != 0x08000000)
	{
		LOG_I("Illegal Flash code.");
		fota_res = FOTA_GENERAL_ERR;
		goto __exit_start_application;
	}
	// 检查栈顶地址是否合法.
	if (((*(volatile uint32_t *)app_addr) & 0x2ff80000) != 0x20000000)	
	{
		LOG_I("Illegal Stack code.");
		fota_res = FOTA_GENERAL_ERR;
		goto __exit_start_application;
	}

	LOG_I("Implement application now.");      
        
    __disable_irq();
	// 关闭所有中断
    __set_FAULTMASK(1);

    //Resets the RCC clock configuration to the default reset state.
    // HAL_RCC_DeInit();
    
    // SysTick->CTRL = 0;
    // SysTick->LOAD = 0;
    // SysTick->VAL = 0;
    // HAL_DeInit();

	//用户代码区第二个字为程序开始地址(复位地址)
	app_func = (fota_app_func)*(volatile uint32_t *)(app_addr + 4);
	/* Configure main stack */ 
	__set_MSP(*(volatile uint32_t *)app_addr);       
           
	/* jump to application */
	app_func();
	
__exit_start_application:
	LOG_I("Implement application failed.");
	return fota_res;
}

void fota_init(void)
{
    int fota_err = FOTA_NO_ERR;

    /* Partition initialized */
	fota_err = fota_boot_verify();
    if (fota_err != FOTA_NO_ERR)
	{
		LOG_D("Partition initialized failed.\r\n");
	}
#if defined(FOTA_ROLLBACK)
	if (fota_rollback_check() == 0)
	{
		printf("\r\n------Start manual firmware rollback------\r\n");
		goto __rollback_entry;
	}
#endif

#if defined(FOTA_YMODEM)
	if ((fota_ymodem_check() == 0) || (is_first_enter == 0))
	{
		printf("\r\n------Enter ymodem mode------\r\n");
		fota_ymodem_ota();
	}
#endif

	printf("\r\n");
    /* Firmware partition verify */
    fota_err = fota_part_fw_verify(FOTA_FM_PART_NAME);
	if (fota_err != FOTA_NO_ERR)
		goto __exit_boot_entry;

    /* Check upgrade status */
    if (fota_check_upgrade() <= 0)
		goto __exit_boot_entry;

	/*Check upgrade bin file*/
	fota_err = fota_upgrade_verify(FOTA_FM_PART_NAME);
    if (fota_err != FOTA_NO_ERR)
    {
        /*validation failed, write a message, the next boot will not be upgraded*/
        fota_copy_version(FOTA_FM_PART_NAME);
        goto __exit_boot_entry;
    }

#if defined(FOTA_ROLLBACK)
	/* copy app to rollback */
	fota_rollback_copy(FOTA_ROLLBACK_PART_NAME, FOTA_APP_PART_NAME);
#endif

    /* Implement upgrade, copy firmware partition to app partition */
    fota_err = fota_upgrade(FOTA_FM_PART_NAME);
	if (fota_err != FOTA_NO_ERR)
		goto __exit_boot_entry;

	/* Update new application verison in RBL file of firmware partition */
	fota_err = fota_copy_version(FOTA_FM_PART_NAME);	
	if (fota_err != FOTA_NO_ERR)
		goto __exit_boot_entry;

	
__exit_boot_entry:
	/* check application */
	if (fota_check_application() == FOTA_NO_ERR)
	{
		/* Implement application */
		fota_start_application();
	}

	LOG_DEBUG("The firmware is corrupted!");
#if defined(FOTA_ROLLBACK)
	printf("\r\n------Start auto firmware rollback------\r\n");
__rollback_entry:
	/* copy rollback to app */
	fota_rollback_copy(FOTA_APP_PART_NAME, FOTA_ROLLBACK_PART_NAME);
	/* check application */
	if (fota_check_application() == FOTA_NO_ERR)
	{
		/* Implement application */
		fota_start_application();
	}

	printf("\r\n------Firmware in rollback area is corrupted------\r\n");
#endif

	is_first_enter = 0;
}