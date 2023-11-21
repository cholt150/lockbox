#ifndef LOCKBOX_COMMON_H
#define LOCKBOX_COMMON_H

#include <Arduino.h>

#define ONE_SECOND 1000 / portTICK_PERIOD_MS
#define MS(x) x / portTICK_PERIOD_MS

#define GPIO_ON 1
#define GPIO_OFF 0

template<class T> inline Print &operator <<(Print &obj, T arg);

enum lock_state
{
    RESET,
    STAGE_1,
    STAGE_2,
    STAGE_3,
    STAGE_4,
    STAGE_5,
};

#endif // !LOCKBOX_COMMON_H
