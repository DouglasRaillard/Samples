ifeq ("$(TARGET_PRODUCT)","Unix")

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CATEGORY_PATH := samples
LOCAL_MODULE := JumpingSumoPiloting
LOCAL_DESCRIPTION := Jumping Sumo Piloting

LOCAL_LIBRARIES := ARSDKBuildUtils libARSAL libARCommands libARNetwork libARNetworkAL libARDiscovery

LOCAL_SRC_FILES := \
	$(call all-c-files-under,.)

LOCAL_LDLIBS := -lncurses

include $(BUILD_EXECUTABLE)

endif