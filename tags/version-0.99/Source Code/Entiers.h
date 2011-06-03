#ifndef __ENTIERS_H
#define __ENTIERS_H

#ifdef _MSC_VER
	#define uint8_t unsigned __int8
	#define uint16_t unsigned __int16
	#define uint32_t unsigned __int32
	#define uint64_t unsigned __int64
#else
	#include <stdint.h>
#endif

#endif
