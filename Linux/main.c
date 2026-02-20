#include "cy_pdl.h"
#include "cybsp.h"

/*
From device driver
// Define I2C register addresses
#define I2C_REG_VER 0x00      // Firmware version register
#define I2C_REG_CUSTOM 0x01   // Custom register (not used in this code)
#define I2C_REG_LED 0x04      // LED control register

*/
// Register map definition
#define REGISTER_COUNT 5
#define INVALID_REGISTER 0xFF

// Register indices
#define REG_DEVICE_STATUS 0 //read fw version i.e read device reg status
#define REG_CONTROL_COMMAND 1
#define REG_CURRENT_VALUE 2
#define REG_CONFIGURATION 3
#define REG_RESERVED 4

// Register map array
uint8_t registerMap[REGISTER_COUNT] = {0xBB, 0x00, 0x00, 0x00, 0x00};

// I2C slave buffers
uint8_t i2cReadBuffer[1];  // Buffer for master read
uint8_t i2cWriteBuffer[2]; // Buffer for master write (register index + value)

// Declare i2cContext as a global variable
cy_stc_scb_i2c_context_t i2cContext;

void I2C_Slave_InterruptHandler(void);
static void SlaveCallback(uint32_t event);

void I2C_Slave_InterruptHandler(void)
{
    /* ISR implementation for I2C */
    Cy_SCB_I2C_SlaveInterrupt(CYBSP_I2C_HW, &i2cContext);
}

// I2C interrupt handler
static void SlaveCallback(uint32_t event)
{
    if (0UL != (CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT & event))
    {
        // Master completed a write; update the corresponding register
        if (i2cWriteBuffer[0] < REGISTER_COUNT) // uint8_t i2cWriteBuffer[2]; // Buffer for master write (register index + value) i.e i2cWriteBuffer[0] = register index 
        {
            if (i2cWriteBuffer[0] != REG_DEVICE_STATUS)// command is not to read fw version, but to write to register => i2cWriteBuffer[0] = 1
            {
                registerMap[i2cWriteBuffer[0]] = i2cWriteBuffer[1]; //i2cWriteBuffer[0] = index, registerMap[1] = value , i.e. new value is available at i2cWriteBuffer[1]
            }
            i2cReadBuffer[0] = registerMap[i2cWriteBuffer[0]];  //i2cReadBuffer[0] = registerMap[0] = FW Version
        }
        Cy_SCB_I2C_SlaveConfigWriteBuf(CYBSP_I2C_HW, i2cWriteBuffer, sizeof(i2cWriteBuffer), &i2cContext);
    }

    if (0UL != (CY_SCB_I2C_SLAVE_RD_CMPLT_EVENT & event))
    {
        // Master completed a read; prepare the next read value
        if (i2cWriteBuffer[0] < REGISTER_COUNT)
        {
            i2cReadBuffer[0] = registerMap[i2cWriteBuffer[0]];
        }
        else
        {
            i2cReadBuffer[0] = INVALID_REGISTER; // Invalid register
        }
        Cy_SCB_I2C_SlaveConfigReadBuf(CYBSP_I2C_HW, i2cReadBuffer, sizeof(i2cReadBuffer), &i2cContext);
    }

}

int main(void)
{
    cy_rslt_t result;

    // Initialize the device and board peripherals
    result = cybsp_init();
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    // Configure interrupt
    cy_stc_sysint_t i2cIntrConfig = {
        .intrSrc = CYBSP_I2C_IRQ,
        .intrPriority = 3
    };
    Cy_SysInt_Init(&i2cIntrConfig, &I2C_Slave_InterruptHandler);
    NVIC_EnableIRQ(CYBSP_I2C_IRQ);
    
    Cy_SCB_I2C_DeInit(CYBSP_I2C_HW);
    Cy_SCB_I2C_Init(CYBSP_I2C_HW, &CYBSP_I2C_config, &i2cContext);
    Cy_SCB_I2C_SlaveConfigReadBuf(CYBSP_I2C_HW, i2cReadBuffer, sizeof(i2cReadBuffer), &i2cContext);
    Cy_SCB_I2C_SlaveConfigWriteBuf(CYBSP_I2C_HW, i2cWriteBuffer, sizeof(i2cWriteBuffer), &i2cContext);
    /* Register Callback function for interrupt */
    Cy_SCB_I2C_RegisterEventCallback(CYBSP_I2C_HW,
        (cy_cb_scb_i2c_handle_events_t) SlaveCallback,
        &i2cContext);
    Cy_SCB_I2C_Enable(CYBSP_I2C_HW, &i2cContext); // Pass the context as the second argument
    
    __enable_irq();


    for (;;)
    {
        if(registerMap[REG_RESERVED] != 0UL)
        {
            Cy_GPIO_Write(USER_LED_PORT, USER_LED_NUM, 1);
            Cy_SysLib_Delay(1000);
            Cy_GPIO_Write(USER_LED_PORT, USER_LED_NUM, 0);
            Cy_SysLib_Delay(1000);
        }
    }
}
