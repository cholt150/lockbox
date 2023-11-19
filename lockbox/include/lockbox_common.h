#ifndef LOCKBOX_COMMON_H
#define LOCKBOX_COMMON_H

#include <Arduino.h>

#define ONE_SECOND 1000 / portTICK_PERIOD_MS
#define MS(x) x / portTICK_PERIOD_MS

template<class T> inline Print &operator <<(Print &obj, T arg);

#endif // !LOCKBOX_COMMON_H
