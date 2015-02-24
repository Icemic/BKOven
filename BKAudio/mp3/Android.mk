LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE   := arm
LOCAL_MODULE    := bkmp3
LOCAL_SRC_FILES := $(notdir $(wildcard $(LOCAL_PATH)/*.cpp))
LOCAL_CFLAGS := -fexceptions -DANDROID

include $(BUILD_STATIC_LIBRARY)
