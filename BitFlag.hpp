#pragma once

#define Bit(num) ((unsigned int)1 << (num))

#define BitIf(value, flag) (((value) & (flag)) != 0)

#define BitIfn(value, flag) ((~(value) & (flag)) != 0)

#define BitOn(value, flag) ((value) |= (flag))

#define BitOff(value, flag) ((value) &= ~(flag))