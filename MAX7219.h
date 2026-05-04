#ifndef __MAX7219_H
#define __MAX7219_H

#include "main.h"

void MAX7219_Init(void);
void MAX7219_Clear(void);
void MAX7219_ShowPattern(const uint8_t pattern[8]);

void MAX7219_ShowSmile(void);
void MAX7219_ShowNeutral(void);
void MAX7219_ShowAlarm(void);
void MAX7219_ShowBlink(void);


#endif
