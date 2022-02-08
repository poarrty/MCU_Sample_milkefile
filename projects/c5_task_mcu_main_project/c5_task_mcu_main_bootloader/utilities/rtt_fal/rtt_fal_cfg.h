/*
 * File      : fal_cfg.h
 * This file is part of FAL (Flash Abstraction Layer) package
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#define FAL_DEBUG 1
#define FAL_PART_HAS_TABLE_CFG
#define NOR_FLASH_DEV_NAME             "norflash0"

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev stm32_onchip_flash;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &stm32_onchip_flash,                                             \
}
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG

#define FLASH_LEN                   (1024 * 1024)
#define OTA_BOOT_LEN                (32 * 1024)
#define OTA_APP_LEN                 (328 * 1024)
#define OTA_FILETEMP_LEN            (328 * 1024)
#define OTA_ROLLBACK_LEN            (328 * 1024)
#define OTA_PAR_LEN                 (FLASH_LEN - OTA_BOOT_LEN - OTA_APP_LEN - OTA_FILETEMP_LEN - OTA_ROLLBACK_LEN)
/* partition table */
#define FAL_PART_TABLE                                                                                                                    \
{                                                                                                                                         \
    {FAL_PART_MAGIC_WORD,"bl",               "stm32_onchip",   0,                                                                 OTA_BOOT_LEN,      0}, \
    {FAL_PART_MAGIC_WORD,"main_mcu_app",     "stm32_onchip",   OTA_BOOT_LEN,                                                      OTA_APP_LEN,       0}, \
    {FAL_PART_MAGIC_WORD,"main_mcu_temp",    "stm32_onchip",   OTA_BOOT_LEN + OTA_APP_LEN,                                        OTA_FILETEMP_LEN, 0}, \
    {FAL_PART_MAGIC_WORD,"main_mcu_rollback","stm32_onchip",   OTA_BOOT_LEN + OTA_APP_LEN + OTA_FILETEMP_LEN,                     OTA_ROLLBACK_LEN,  0}, \
    {FAL_PART_MAGIC_WORD,"fdb_kvdb",         "stm32_onchip",   OTA_BOOT_LEN + OTA_APP_LEN + OTA_FILETEMP_LEN + OTA_ROLLBACK_LEN,  OTA_PAR_LEN,       0}, \
}
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
