/**
 * @brief   Device driver for LIS2DE12.
 */

#ifndef LIS2DE12_H_
#define LIS2DE12_H_

#include <stdint.h>

/** LI2DE12 I2C address when SA0 is low */
#define LIS2DE12_I2C_ADDR_1         0x18

/** LI2DE12 I2C address when SA0 is high */
#define LIS2DE12_I2C_ADDR_2         0x19

/** Default I2C address */
#define LI2DE12_I2C_DEFAULT_ADDR    LIS2DE12_I2C_ADDR_1

/** LI2DE12 registers. */
#define LI2DE12_STATUS_REG_AUX      0x07
#define LI2DE12_OUT_TEMP_L          0x0C
#define LI2DE12_OUT_TEMP_H          0x0D
#define LI2DE12_WHO_AM_I            0x0F
#define LI2DE12_CTRL_REG0           0x1E
#define LI2DE12_TEMP_CFG_REG        0x1F
#define LI2DE12_CTRL_REG1           0x20
#define LI2DE12_CTRL_REG2           0x21
#define LI2DE12_CTRL_REG3           0x22
#define LI2DE12_CTRL_REG4           0x23
#define LI2DE12_CTRL_REG5           0x24
#define LI2DE12_CTRL_REG6           0x25
#define LI2DE12_REFERENCE           0x26
#define LI2DE12_STATUS_REG          0x27
#define LI2DE12_FIFO_READ_START     0x28
#define LI2DE12_OUT_X_H             0x29
#define LI2DE12_OUT_Y_H             0x2B
#define LI2DE12_OUT_Z_H             0x2D
#define LI2DE12_FIFO_CTRL_REG       0x2E
#define LI2DE12_FIFO_SRC_REG        0x2F
#define LI2DE12_INT1_CFG            0x30
#define LI2DE12_INT1_SRC            0x31
#define LI2DE12_INT1_THS            0x32
#define LI2DE12_INT1_DURATION       0x33
#define LI2DE12_INT2_CFG            0x34
#define LI2DE12_INT2_SRC            0x35
#define LI2DE12_INT2_THS            0x36
#define LI2DE12_INT2_DURATION       0x37
#define LI2DE12_CLICK_CFG           0x38
#define LI2DE12_CLICK_SRC           0x39
#define LI2DE12_CLICK_THS           0x3A
#define LI2DE12_TIME_LIMIT          0x3B
#define LI2DE12_TIME_LATENCY        0x3C
#define LI2DE12_TIME_WINDOW         0x3D
#define LI2DE12_ACT_THS             0x3E
#define LI2DE12_ACT_DUR             0x3F

/** LI2DE12 temperature state. */
#define LI2DE12_TEMP_DISABLED       (0b00 << 6)
#define LI2DE12_TEMP_ENABLED        (0b11 << 6)

void LIS2DE12_Init();
uint8_t LIS2DE12_ReadReg(uint8_t deviceAddress, uint8_t regAddress,
        uint8_t *data);
uint8_t LIS2DE12_WriteReg(uint8_t deviceAddress, uint8_t regAddress,
        uint8_t data);
uint8_t LIS2DE12_EnableTemp();
uint8_t LIS2DE12_ReadTemp(int *val);

#endif /* LIS2DE12_H_ */
