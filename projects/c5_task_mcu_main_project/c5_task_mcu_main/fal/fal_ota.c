/******************************************************************
 ** Copyright (C),  CVTE Electronics CO.Ltd 2020-2021.
 ** File name:
 ** Author:         sulikang
 ** Version:        V0.0.1
 ** Date:           2021-3-25
 ** Description:
 ** Others:
 ** Function List:
 ** History:        2021-08 jianyongxiang 创建
 ** <time>          <author>     <version >    <desc>
 ** 2021-8-26     jianyongxiang    0.0.1       创建文件
 ******************************************************************/
#include "fal_ota.h"
#include "md5.h"
#include "string.h"
#include "cmsis_os.h"
#include "log.h"
#include "queue.h"
#include <flashdb.h>

#define LOG_TAG "fal_ota"
#include "elog.h"

#define APP_KEY "c3af03910f6e6d306b70560f75229cbc9fc66900"

static service                                   g_upgrade;
static chassis_interfaces__srv__Upgrade_Request  upgrade_req;
static chassis_interfaces__srv__Upgrade_Response upgrade_res;

static service                                           g_get_updating_info;
static chassis_interfaces__srv__GetUpdatingInfo_Request  get_updating_info_req;
static chassis_interfaces__srv__GetUpdatingInfo_Response get_updating_info_res;

static subscrption          g_discover;
static std_msgs__msg__UInt8 discover;

static publisher                               g_upgrade_progress;
static chassis_interfaces__msg__UpgradeProgess upgrade_progress;

static publisher                             g_firmware_info;
static chassis_interfaces__msg__FirmwareInfo firmware_info;

uint8_t         file_temp_buff[OTA_PACKSIZE] = {0};
updating_info_t kv_updating_info             = {0};
firmware_info_t kv_firmware_info             = {0};
ota_t           kv_ota_config                = {0};

struct fdb_blob        blob;
extern struct fdb_kvdb kvdb;

static void fal_ota_check_update(void);

static void fal_ota_discovered(const void *msgin);
static void fal_upgrade(const void *req, void *res);
static void fal_get_updating_info(const void *req, void *res);

void fal_ota_init(void) {
    subscrption_init(&g_discover, ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8), "/ota/discover", &discover, BEST, fal_ota_discovered);

    publisher_init(&g_firmware_info, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, FirmwareInfo), "/ota/firmware_info", &firmware_info,
                   BEST, OVERWRITE, sizeof(chassis_interfaces__msg__FirmwareInfo));

    publisher_init(&g_upgrade_progress, ROSIDL_GET_MSG_TYPE_SUPPORT(chassis_interfaces, msg, UpgradeProgess),
                   "/ota_" APP_KEY "/upgrade_progress", &upgrade_progress, BEST, OVERWRITE, sizeof(chassis_interfaces__msg__UpgradeProgess));

    service_init(&g_upgrade, ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, Upgrade), "/ota_" APP_KEY "/upgrade", &upgrade_req,
                 &upgrade_res, BEST, fal_upgrade);

    service_init(&g_get_updating_info, ROSIDL_GET_SRV_TYPE_SUPPORT(chassis_interfaces, srv, GetUpdatingInfo),
                 "/ota_" APP_KEY "/get_updating_info", &get_updating_info_req, &get_updating_info_res, BEST, fal_get_updating_info);

    MallocString(&upgrade_req.package_md5, REQ, STRING_MAX);
    MallocString(&upgrade_req.total_md5, REQ, STRING_MAX);
    upgrade_req.package_data.data     = file_temp_buff;
    upgrade_req.package_data.size     = 0;
    upgrade_req.package_data.capacity = sizeof(file_temp_buff);

    fal_ota_check_update();
}

int ota_write_file_temp(uint32_t addr, uint8_t *buffer, uint32_t count) {
    const struct fal_partition *partition = NULL;

    partition = fal_partition_find("main_mcu_temp");

    if (partition == NULL) {
        log_w("Find partition (%s) failed!\n", "main_mcu_temp");
        return -1;
    }

    return fal_partition_write(partition, addr, buffer, count);
}

void ota_read_file_temp(uint32_t addr, uint8_t *buffer, uint32_t count) {
    const struct fal_partition *partition = NULL;

    partition = fal_partition_find("main_mcu_temp");

    if (partition == NULL) {
        log_w("Find partition (%s) failed!\n", "main_mcu_temp");
        return;
    }

    fal_partition_read(partition, addr, buffer, count);
}

void ota_erase_file_temp(void) {
    const struct fal_partition *partition = NULL;

    partition = fal_partition_find("main_mcu_temp");

    if (partition == NULL) {
        log_w("Find partition (%s) failed!\n", "main_mcu_temp");
        return;
    }

    if (fal_partition_erase_all(partition) < 0) {
        log_w("Partition (%s) erase failed!\n", partition->name);
        return;
    }

    log_d("Erase (%s) partition finish!\n", "main_mcu_temp");
}

static void fal_ota_check_update(void) {
    fdb_kv_get_blob(&kvdb, "kv_updating_info", fdb_blob_make(&blob, &kv_updating_info, sizeof(kv_updating_info)));
    fdb_kv_get_blob(&kvdb, "kv_firmware_info", fdb_blob_make(&blob, &kv_firmware_info, sizeof(kv_firmware_info)));

    /*版本不一致视为更新成功*/
    if (strcmp(kv_firmware_info.version, SOFTWARE_VERSION) != 0) {
        /*删除续传信息*/
        kv_updating_info.buffered_size = 0;
        memset(kv_updating_info.total_md5, 0, sizeof(kv_updating_info.total_md5));
        fdb_kv_set_blob(&kvdb, "kv_updating_info", fdb_blob_make(&blob, &kv_updating_info, sizeof(kv_updating_info)));
        /*OTA完成后，更新时间*/
        pal_uros_msg_set_timestamp(&kv_firmware_info.stamp);
        strcpy(kv_firmware_info.version, SOFTWARE_VERSION);
        fdb_kv_set_blob(&kvdb, "kv_firmware_info", fdb_blob_make(&blob, &kv_firmware_info, sizeof(kv_firmware_info)));
        /*发布升级结果*/
        // upgrade_progress.progress = 1.0;
        // upgrade_progress.upgradestatus = 1;
        // message_publish(&g_upgrade_progress);
        log_d("ota finish");
    }
}

void fal_ota_discovered(const void *msgin) {
    log_d("OTA discoverd");
    strcpy(kv_firmware_info.version, SOFTWARE_VERSION);
    log_d("kv_firmware_info.version :%s", kv_firmware_info.version);

    firmware_info.header.stamp = kv_firmware_info.stamp;

    firmware_info.priority = 1;

    firmware_info.package_size = OTA_PACKSIZE;

    firmware_info.app_key.data = APP_KEY;
    firmware_info.app_key.size = strlen(firmware_info.app_key.data);

    firmware_info.version.data = kv_firmware_info.version;
    firmware_info.version.size = strlen(firmware_info.version.data);

    firmware_info.platform.data = "android_rom";
    firmware_info.platform.size = strlen(firmware_info.platform.data);

    firmware_info.upgrade_service_name.data = "/ota_" APP_KEY "/upgrade";
    firmware_info.upgrade_service_name.size = strlen(firmware_info.upgrade_service_name.data);

    firmware_info.updating_info_service_name.data = "/ota_" APP_KEY "/get_updating_info";
    firmware_info.updating_info_service_name.size = strlen(firmware_info.updating_info_service_name.data);

    firmware_info.upgrade_progress_topic_name.data = "/ota_" APP_KEY "/upgrade_progress";
    firmware_info.upgrade_progress_topic_name.size = strlen(firmware_info.upgrade_progress_topic_name.data);
    /*接受到discover请求后，发送固件信息topic*/
    message_publish(&g_firmware_info);
}

void fal_upgrade(const void *req, void *res) {
    chassis_interfaces__srv__Upgrade_Request * req_in = (chassis_interfaces__srv__Upgrade_Request *) req;
    chassis_interfaces__srv__Upgrade_Response *res_in = (chassis_interfaces__srv__Upgrade_Response *) res;

    if (req_in->package_data.size != req_in->package_size) {
        log_w("error message, package_data.size:%d, package_size:%ld", req_in->package_data.size, req_in->package_size);
        res_in->success = 0;
        return;
    }

    if ((req_in->package_size >= req_in->total_size) || (req_in->package_size > OTA_PACKSIZE)) {
        log_w("error message, package_size:%ld, total_size:%ld, ota_packsize:%d", req_in->package_size, req_in->total_size, OTA_PACKSIZE);
        res_in->success = 0;
        return;
    }

    /*计算接受文件md5*/
    char    md5_cal_temp[33] = {0};
    char    string_temp[3]   = {0};
    uint8_t decrypt[16]      = {0};
    MD5_CTX md5;

    MD5Init(&md5);
    MD5Update(&md5, req_in->package_data.data, req_in->package_size);
    MD5Final(&md5, decrypt);

    for (uint8_t i = 0; i < 16; i++) {
        sprintf(string_temp, "%02x", decrypt[i]);
        strcat(md5_cal_temp, string_temp);
    }

    if (memcmp(md5_cal_temp, req_in->package_md5.data, 32)) {
        log_w("md5 cal :%s", md5_cal_temp);
        log_w("md5 receive :%s", req_in->package_md5.data);
        res_in->success = 0;
    } else {
        /*认为第一次接受文件*/
        if (req_in->package_offset == 0) {
            /*存储待升级文件总md5*/
            memset(kv_updating_info.total_md5, 0, sizeof(kv_updating_info.total_md5));
            memcpy(kv_updating_info.total_md5, req_in->total_md5.data, req_in->total_md5.size);
            /*擦除备份区*/
            log_d("ota erase file temp");
            ota_erase_file_temp();
            kv_updating_info.buffered_size = 0;
        }

        /*更新已写入文件大小*/
        if (ota_write_file_temp(req_in->package_offset, (uint8_t *) req_in->package_data.data, req_in->package_size) < 0) {
            log_w("flash write 0x%8lx error", req_in->package_offset);
            res_in->success = 0;
        } else {
            log_d("flash write 0x%8lx ok, package_size: %ld", req_in->package_offset, req_in->package_size);
            log_d("ota receive %ld percent", (kv_updating_info.buffered_size * 100 / req_in->total_size));

            kv_updating_info.buffered_size += req_in->package_size;
            res_in->success = 1;
        }

        fdb_kv_set_blob(&kvdb, "kv_updating_info", fdb_blob_make(&blob, &kv_updating_info, sizeof(kv_updating_info)));

        /*升级文件已完全写入*/
        if (kv_updating_info.buffered_size == req_in->total_size) {
            log_d("ota receive all %ld Byte ok", req_in->total_size);

            memset(&md5, 0, sizeof(MD5_CTX));
            MD5Init(&md5);
            uint32_t i = 0;

            for (i = 0; i < req_in->total_size / OTA_PACKSIZE; i++) {
                ota_read_file_temp(i * OTA_PACKSIZE, file_temp_buff, OTA_PACKSIZE);
                MD5Update(&md5, file_temp_buff, OTA_PACKSIZE);
            }

            ota_read_file_temp(i * OTA_PACKSIZE, file_temp_buff, req_in->total_size - i * OTA_PACKSIZE);
            MD5Update(&md5, file_temp_buff, req_in->total_size - i * OTA_PACKSIZE);
            MD5Final(&md5, decrypt);
            memset(md5_cal_temp, 0, sizeof(md5_cal_temp));

            for (uint8_t i = 0; i < 16; i++) {
                sprintf(string_temp, "%02x", decrypt[i]);
                strcat(md5_cal_temp, string_temp);
            }

            if (memcmp(md5_cal_temp, req_in->total_md5.data, 32)) {
                log_w("total md5 cal :%s", md5_cal_temp);
                log_w("total md5 receive :kv_updating_info :%s", req_in->total_md5.data);
                res_in->success = 0;
            } else {
                kv_ota_config.ready_flag = 1;
                res_in->success          = 1;
                /*发布升级中topic*/
                upgrade_progress.progress      = 0.1;
                upgrade_progress.upgradestatus = 0;
                message_publish(&g_upgrade_progress);
            }
        }
    }
}

void fal_get_updating_info(const void *req, void *res) {
    // chassis_interfaces__srv__GetUpdatingInfo_Request * req_in =
    // (chassis_interfaces__srv__GetUpdatingInfo_Request *) req;
    chassis_interfaces__srv__GetUpdatingInfo_Response *res_in = (chassis_interfaces__srv__GetUpdatingInfo_Response *) res;

    res_in->total_md5.data = kv_updating_info.total_md5;
    res_in->total_md5.size = strlen(res_in->total_md5.data);

    /*计算文件md5*/
    char     string_temp[3] = {0};
    uint8_t  decrypt[16]    = {0};
    MD5_CTX  md5;
    uint32_t i = 0;

    MD5Init(&md5);

    for (i = 0; i < kv_updating_info.buffered_size / OTA_PACKSIZE; i++) {
        ota_read_file_temp(i * OTA_PACKSIZE, file_temp_buff, OTA_PACKSIZE);
        MD5Update(&md5, file_temp_buff, OTA_PACKSIZE);
    }

    ota_read_file_temp(i * OTA_PACKSIZE, file_temp_buff, kv_updating_info.buffered_size - i * OTA_PACKSIZE);
    MD5Update(&md5, file_temp_buff, kv_updating_info.buffered_size - i * OTA_PACKSIZE);
    MD5Final(&md5, decrypt);

    memset(kv_updating_info.buffered_md5, 0, sizeof(kv_updating_info.buffered_md5));

    for (uint8_t i = 0; i < 16; i++) {
        sprintf(string_temp, "%02x", decrypt[i]);
        strcat(kv_updating_info.buffered_md5, string_temp);
    }

    res_in->buffered_md5.data = kv_updating_info.buffered_md5;
    res_in->buffered_md5.size = strlen(res_in->buffered_md5.data);

    res_in->buffered_size = kv_updating_info.buffered_size;

    log_d("total_md5:%s", res_in->total_md5.data);
    log_d("buffered_md5:%s", res_in->buffered_md5.data);
    log_d("buffered_size:%ld", res_in->buffered_size);
}