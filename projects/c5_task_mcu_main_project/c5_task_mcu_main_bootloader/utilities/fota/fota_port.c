#include "main.h"
#include "fota.h"
#include "fota_port.h"

#if defined(FOTA_ROLLBACK)
/**
 * @brief   上电检查是否需要执行版本回滚.
 * @return  返回结果，0：成功，其他：失败.
 */
int fota_rollback_check(void) {
    return 1;
}
#endif

#if defined(FOTA_YMODEM)
/**
 * @brief   上电检查是否需要通过ymodem升级.
 * @return  返回结果，0：成功，其他：失败.
 */
int fota_ymodem_check(void) {
    return 1;
}
#endif
