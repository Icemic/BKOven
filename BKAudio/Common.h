#pragma once

#ifdef _MSC_VER
#pragma warning(disable:4996) //no "unsafe" functions
#endif

//#if BKPULONG_MAX<0xFFFFFFFF
//#error "longs on this implementation are too small."
//#endif

#include <cstdint>
#include <qglobal.h>

typedef float bkfloat;
typedef double bkdouble;
typedef int32_t bklong;
typedef uint32_t bkulong;
typedef int64_t bklonglong;
typedef uint64_t bkulonglong;
typedef int16_t bkshort;
typedef uint16_t bkushort;
typedef char bkchar;
typedef unsigned char bkuchar;
typedef wchar_t bkwchar;

#define BKULONG_MAX BKPULONG_MAX
#define BKLONG_MAX BKPLONG_MAX
#define BKLONG_MIN BKPLONG_MIN

#ifdef Q_OS_WIN
#define BKE_SYS_WINDOWS
#endif

#ifndef FASTCALL
#ifdef WIN32
#define FASTCALL __fastcall
#else
#define FASTCALL
#endif
#endif

/////////////////////config////////////////////////
