#pragma once

#include "../ParserHelper/parser/defines.h"

#ifdef _MSC_VER
#pragma warning(disable:4996) //no "unsafe" functions
#endif

//#if BKPULONG_MAX<0xFFFFFFFF
//#error "longs on this implementation are too small."
//#endif

typedef float bkfloat;
typedef double bkdouble;
typedef bkplong bklong;
typedef bkpulong bkulong;
typedef bkplonglong bklonglong;
typedef bkpulonglong bkulonglong;
typedef bkpshort bkshort;
typedef bkpushort bkushort;
typedef bkpchar bkchar;
typedef bkpuchar bkuchar;
typedef bkpwchar bkwchar;

#define BKULONG_MAX BKPULONG_MAX
#define BKLONG_MAX BKPLONG_MAX
#define BKLONG_MIN BKPLONG_MIN

#ifndef FASTCALL
#if WIN32
#define FASTCALL __fastcall
#else
#define FASTCALL
#endif
#endif

/////////////////////config////////////////////////
