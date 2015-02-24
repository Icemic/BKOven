NDK_TOOLCHAIN_VERSION=clang
#APP_OPTIM        := debug
APP_STL := gnustl_static
#APP_CPPFLAGS := -frtti -finput-charset=UTF-8 -fpermissive -std=c++11 -Wno-format-security -Wno-literal-suffix #-DCOCOS2D_DEBUG=1
#for clang
APP_CPPFLAGS := -O2 -frtti -fpermissive -std=c++11 -Wno-deprecated-register -Wno-bool-conversion -Wno-format-security -Wno-literal-range #-DCOCOS2D_DEBUG=1 -ggdb
#TARGET_CPU_API := armeabi armeabi-v7a
APP_ABI := armeabi-v7a
