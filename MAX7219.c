#include "max7219.h"

extern SPI_HandleTypeDef hspi1;

#define MAX7219_CS_PORT   GPIOA
#define MAX7219_CS_PIN    GPIO_PIN_4

static void MAX7219_CS_Low(void)
{
    HAL_GPIO_WritePin(MAX7219_CS_PORT, MAX7219_CS_PIN, GPIO_PIN_RESET);
}

static void MAX7219_CS_High(void)
{
    HAL_GPIO_WritePin(MAX7219_CS_PORT, MAX7219_CS_PIN, GPIO_PIN_SET);
}

static void MAX7219_Send(uint8_t address, uint8_t data)
{
    uint8_t tx_buf[2];
    tx_buf[0] = address;
    tx_buf[1] = data;

    MAX7219_CS_Low();
    HAL_SPI_Transmit(&hspi1, tx_buf, 2, 100);
    MAX7219_CS_High();
}

void MAX7219_Clear(void)
{
    uint8_t i;
    for (i = 1; i <= 8; i++)
    {
        MAX7219_Send(i, 0x00);
    }
}

void MAX7219_Init(void)
{
    MAX7219_CS_High();

    MAX7219_Send(0x0C, 0x00);  // shutdown
    MAX7219_Send(0x09, 0x00);  // no decode
    MAX7219_Send(0x0A, 0x03);  // intensity 0~15
    MAX7219_Send(0x0B, 0x07);  // scan limit 0~7
    MAX7219_Send(0x0F, 0x00);  // display test off
    MAX7219_Send(0x0C, 0x01);  // normal operation

    MAX7219_Clear();
}

static void MAX7219_Rotate90CW(const uint8_t src[8], uint8_t dst[8])
{
    uint8_t row;
    uint8_t col;

    for (row = 0; row < 8; row++)
    {
        dst[row] = 0x00;
    }

    for (row = 0; row < 8; row++)
    {
        for (col = 0; col < 8; col++)
        {
            if (src[row] & (0x80 >> col))
            {
                dst[col] |= (1 << row);
            }
        }
    }
}


void MAX7219_ShowPattern(const uint8_t pattern[8])
{
    uint8_t i;
    uint8_t rotated[8];

    MAX7219_Rotate90CW(pattern, rotated);

    for (i = 0; i < 8; i++)
    {
        MAX7219_Send(i + 1, rotated[i]);
    }
}

void MAX7219_ShowSmile(void)
{
    static const uint8_t smile[8] =
    {
        0x3C,
        0x42,
        0xA5,
        0x81,
        0xA5,
        0x99,
        0x42,
        0x3C
    };

    MAX7219_ShowPattern(smile);
}

void MAX7219_ShowNeutral(void)
{
    static const uint8_t neutral[8] =
    {
        0x3C,
        0x42,
        0xA5,
        0x81,
        0xA5,
        0xBD,
        0x42,
        0x3C
    };

    MAX7219_ShowPattern(neutral);
}

void MAX7219_ShowAlarm(void)
{
    static const uint8_t alarm[8] =
    {
        0x18,
        0x18,
        0x18,
        0x18,
        0x18,
        0x00,
        0x18,
        0x00
    };

    MAX7219_ShowPattern(alarm);
}

void MAX7219_ShowBlink(void)
{
    static const uint8_t blink[8] =
    {
        0x3C,
        0x42,
        0x00,
        0x3C,
        0x00,
        0x99,
        0x42,
        0x3C
    };

    MAX7219_ShowPattern(blink);
}

