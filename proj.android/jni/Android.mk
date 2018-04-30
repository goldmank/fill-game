LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

$(call import-add-path,$(LOCAL_PATH)/../../cocos2d)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/external)
$(call import-add-path,$(LOCAL_PATH)/../../cocos2d/cocos)

LOCAL_MODULE := cocos2dcpp_shared

LOCAL_MODULE_FILENAME := libcocos2dcpp

LOCAL_SRC_FILES := hellocpp/main.cpp \
                   hellocpp/SystemUtilsAndroid.cpp \
                   ../../Classes/SystemUtils.cpp \
                   ../../Classes/AppDelegate.cpp \
                   ../../Classes/BaseScene.cpp \
                   ../../Classes/Pref.cpp \
                   ../../Classes/GameSceneBlocks.cpp \
                   ../../Classes/GameScene_Grid.cpp \
                   ../../Classes/GameScene_RunUp.cpp \
                   ../../Classes/GameScene_FlyGuy.cpp \
                   ../../Classes/GameScene_ZombieRun.cpp \
                   ../../Classes/MainScene.cpp \
                   ../../Classes/GameScene_Mayhem.cpp \
                   ../../Classes/GameScene_DontShootYourself.cpp \
                   ../../Classes/GameScene_2Balls.cpp \
                   ../../Classes/GameScene_HoldToWalk.cpp \
                   ../../Classes/GameScene_3Sticks.cpp \
                   ../../Classes/GameScene_MapPuzzle.cpp \
                   ../../Classes/GameScene.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../Classes \
$(LOCAL_PATH)/../../cocos2d

LOCAL_WHOLE_STATIC_LIBRARIES := cocos2dx_static
LOCAL_WHOLE_STATIC_LIBRARIES += cocosdenshion_static

LOCAL_WHOLE_STATIC_LIBRARIES += box2d_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocosbuilder_static
# LOCAL_WHOLE_STATIC_LIBRARIES += spine_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocostudio_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_network_static
# LOCAL_WHOLE_STATIC_LIBRARIES += cocos_extension_static


include $(BUILD_SHARED_LIBRARY)

$(call import-module,.)
$(call import-module,audio/android)

$(call import-module,Box2D)
# $(call import-module,editor-support/cocosbuilder)
# $(call import-module,editor-support/spine)
# $(call import-module,editor-support/cocostudio)
# $(call import-module,network)
# $(call import-module,extensions)
