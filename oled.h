#ifndef __OLED_H
#define __OLED_H

#include "main.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x, uint8_t y, char chr);
void OLED_ShowString(uint8_t x, uint8_t y, char *str);
void OLED_ShowChinese16(uint8_t x, uint8_t y, uint8_t index);

void OLED_ShowStringCenter(uint8_t y, char *str);
void OLED_ShowChineseCenter16(uint8_t y, uint8_t start_index, uint8_t count);

void OLED_ShowBMP(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bmp);

void OLED_ShowBMP(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bmp);
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

void OLED_ShowChinese16Clip(int16_t x, uint8_t y, uint8_t index);
void OLED_ClearArea(uint8_t x, uint8_t y, uint8_t w, uint8_t h);


#endif