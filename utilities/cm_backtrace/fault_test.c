/*
 * fault_test.c
 *
 *  Created on: 2016/12/25
 *      Author: Armink
 */

#include "stdio.h"
#include "shell.h"

#if 1
void fault_test_by_unalign(void) {
    volatile int * SCB_CCR = (volatile int *) 0xE000ED14; // SCB->CCR
    volatile int * p;
    volatile int value;

    *SCB_CCR |= (1 << 3); /* bit3: UNALIGN_TRP. */

    p = (int *) 0x00;
    value = *p;
    printf("addr:0x%02X value:0x%08X\r\n", (int) p, value);

    p = (int *) 0x04;
    value = *p;
    printf("addr:0x%02X value:0x%08X\r\n", (int) p, value);

    p = (int *) 0x03;
    value = *p;
    printf("addr:0x%02X value:0x%08X\r\n", (int) p, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fault_test_by_unalign, fault_test_by_unalign, fault_test_by_unalign);

void fault_test_by_div0(void) {
    volatile int * SCB_CCR = (volatile int *) 0xE000ED14; // SCB->CCR
    int x, y, z;

    *SCB_CCR |= (1 << 4); /* bit4: DIV_0_TRP. */

    x = 10;
    y = 0;
    z = x / y;
    printf("z:%d\n", z);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fault_test_by_div0, fault_test_by_div0, fault_test_by_div0);

void fault_test_by_pcPointer(void) {
    typedef  void (*pFunction)(void);
    pFunction func;
    func = (pFunction)0x0;
    func();
    printf("pc pointer fault");
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fault_test_by_pcPointer, fault_test_by_pcPointer, fault_test_by_pcPointer);

void fault_test_by_addr_invalid(void) {
    volatile int * p;
    volatile int value;

    p = (int *)0xDFFF0000;
    value = *p;
    printf("addr:0x%02X value:0x%08X\r\n", (int) p, value);
}
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0)|SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), fault_test_by_addr_invalid, fault_test_by_addr_invalid, fault_test_by_addr_invalid);

#endif

