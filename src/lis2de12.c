/**
 * @brief   Device driver for LIS2DE12.
 */

#include "lis2de12.h"
#include "assert.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define LIS2DE12_I2C_CLK_HZ                     100000

#define LIS2DE12_I2C_CLK_ENABLE()               __HAL_RCC_I2C1_CLK_ENABLE()
#define LIS2DE12_I2C_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define LIS2DE12_I2C_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define LIS2DE12_I2C_SCL_PIN                    GPIO_PIN_6
#define LIS2DE12_I2C_SCL_GPIO_PORT              GPIOB
#define LIS2DE12_I2C_SCL_AF                     GPIO_AF4_I2C1

#define LIS2DE12_I2C_SDA_PIN                    GPIO_PIN_9
#define LIS2DE12_I2C_SDA_GPIO_PORT              GPIOB
#define LIS2DE12_I2C_SDA_AF                     GPIO_AF4_I2C1

#define LIS2D12_SHIFTED_ADDR(addr)              (addr << 1)
#define LIS2D12_DEV_REG_INC(reg, autoInc)       ((reg & 0x7F) | (autoInc << 7))
#define LIS2D12_TIMEOUT_MS                      1000

static I2C_HandleTypeDef i2cHandle;

/**
 * Initialize LIS2DE12 device using I2C interface.
 */
void LIS2DE12_Init()
{
    i2cHandle.Instance = I2C1;

    i2cHandle.Init.ClockSpeed = LIS2DE12_I2C_CLK_HZ;
    i2cHandle.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
    i2cHandle.Init.OwnAddress1 = 0;
    i2cHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2cHandle.Init.OwnAddress2 = 0;
    i2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2cHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    i2cHandle.State = HAL_I2C_STATE_RESET;

    ASSERT(HAL_I2C_Init(&i2cHandle) == HAL_OK);
}

/**
 * Read register.
 *
 * @param   deviceAddress       I2C device address.
 * @param   regAddress          Register address to be read.
 * @param   data                Memory to store the read data.
 *
 * @returns It returns 1 if register has been read with success. Otherwise, it
 *          returns 0.
 */
uint8_t LIS2DE12_ReadReg(uint8_t deviceAddress, uint8_t regAddress,
        uint8_t *data)
{
    HAL_StatusTypeDef status;
    uint8_t devReg;

    devReg = LIS2D12_DEV_REG_INC(regAddress, false);

    status = HAL_I2C_Mem_Read(&i2cHandle, LIS2D12_SHIFTED_ADDR(deviceAddress),
        devReg, I2C_MEMADD_SIZE_8BIT, data, 1, LIS2D12_TIMEOUT_MS);

    return (status == HAL_OK);
}

/**
 * Write register.
 *
 * @param   deviceAddress       I2C device address.
 * @param   regAddress          Register address to be written.
 * @param   data                Data to be written.
 *
 * @returns It returns 1 if register has been written with success. Otherwise,
 *          it returns 0.
 */
uint8_t LIS2DE12_WriteReg(uint8_t deviceAddress, uint8_t regAddress,
        uint8_t data)
{
    HAL_StatusTypeDef status;
    uint8_t devReg;

    devReg = LIS2D12_DEV_REG_INC(regAddress, false);

    status = HAL_I2C_Mem_Write(&i2cHandle, LIS2D12_SHIFTED_ADDR(deviceAddress),
        devReg, I2C_MEMADD_SIZE_8BIT, &data, 1, LIS2D12_TIMEOUT_MS);

    return (status == HAL_OK);
}

/**
 * Enable temperature sensor.
 *
 * @returns It returns 1 if the temperature sensor has been enabled with
 *          success. Otherwise, it returns 0.
 */
uint8_t LIS2DE12_EnableTemp()
{
    /* XXX: As the given interface does not specify the device address when
     * enabling the temperature feature, the default device address will be
     * used */
    return LIS2DE12_WriteReg(LI2DE12_I2C_DEFAULT_ADDR, LI2DE12_TEMP_CFG_REG,
        LI2DE12_TEMP_ENABLED);
}

/**
 * Read temperature.
 *
 * @param   val     Memory where the temperature read shall be stored.
 *
 * @returns It returns 1 if the temperature has been read with success.
 *          Otherwise, it returns 0.
 */
uint8_t LIS2DE12_ReadTemp(int *val)
{
    HAL_StatusTypeDef status;
    uint8_t devReg;

    devReg = LIS2D12_DEV_REG_INC(LI2DE12_OUT_TEMP_L, true);

    /* XXX: As the given interface does not specify the device address when
     * enabling the temperature feature, the default device address will be
     * used.
     *
     * As the datasheet doesn't say the conversion factor, it's assumed that
     * the conversion is value/°C (25 -> 25°C) */
    status = HAL_I2C_Mem_Read(&i2cHandle,
        LIS2D12_SHIFTED_ADDR(LI2DE12_I2C_DEFAULT_ADDR), devReg,
        I2C_MEMADD_SIZE_8BIT, (uint8_t *) val, 2, LIS2D12_TIMEOUT_MS);

    return (status == HAL_OK);
}

/**
 * Initialize the I2C peripheral.
 *
 * @param   i2c     I2C to be initialized.
 */
void HAL_I2C_MspInit(I2C_HandleTypeDef *i2c)
{
    GPIO_InitTypeDef gpioInit;

    /* Enable clocks */
    LIS2DE12_I2C_SDA_GPIO_CLK_ENABLE();
    LIS2DE12_I2C_SCL_GPIO_CLK_ENABLE();
    LIS2DE12_I2C_CLK_ENABLE();

    /* I2C TX GPIO pin configuration  */
    gpioInit.Pin = LIS2DE12_I2C_SCL_PIN;
    gpioInit.Mode = GPIO_MODE_AF_OD;
    gpioInit.Pull = GPIO_PULLUP;
    gpioInit.Speed = GPIO_SPEED_FAST;
    gpioInit.Alternate = LIS2DE12_I2C_SCL_AF;

    HAL_GPIO_Init(LIS2DE12_I2C_SCL_GPIO_PORT, &gpioInit);

    /* I2C RX GPIO pin configuration  */
    gpioInit.Pin = LIS2DE12_I2C_SDA_PIN;
    gpioInit.Alternate = LIS2DE12_I2C_SDA_AF;

    HAL_GPIO_Init(LIS2DE12_I2C_SDA_GPIO_PORT, &gpioInit);
}

