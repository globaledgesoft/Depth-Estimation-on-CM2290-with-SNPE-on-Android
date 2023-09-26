#include <jni.h>
#include <string>
#include <android/native_window_jni.h>
#include <android/asset_manager_jni.h>
#include <thread>
#include <android_native_app_glue.h>
#include "Depth.h"

static Depth app;

#ifdef __cplusplus
extern "C" {
#endif

jint JNI_OnLoad(JavaVM *vm, void *) {
    // We need to store a reference to the Java VM so that we can call back
    app.SetJavaVM(vm);
    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL
Java_com_example_depthestimation_MainActivity_onCreateJNI(JNIEnv *env, jobject clazz, jobject activity, jobject j_asset_manager) {
    LOGI("+++++++++++++++++++++++++++++++++++++++");

    std::stringstream path;
    path << "/system/lib" << "/data/local/tmp/snpeexample/dsp/lib;/system/lib/rfsa/adsp;/system/vendor/lib/rfsa/adsp;/dsp;/vendor/lib";
    setenv("ADSP_LIBRARY_PATH", path.str().c_str(), 1 /*override*/);
    path.clear();
    path << "/data/local/tmp/snpeexample/arm-android-clang6.0/lib:/vendor/lib:data/app/com.example.onetry-MQbe7lUJmwPQpLAWeUjLOQ==/lib/arm";
    setenv("LD_LIBRARY_PATH", path.str().c_str(), 1 /*override*/);

    std::string adsp = getenv("ADSP_LIBRARY_PATH");
    std::string ld = getenv("LD_LIBRARY_PATH");
    LOGI("+++++++++++ adsp %s\t ld %s", adsp.c_str(), ld.c_str());

    app.OnCreate();
}


extern "C" JNIEXPORT void JNICALL
Java_com_example_depthestimation_MainActivity_setSurface(
        JNIEnv* env, jobject clazz,
        jobject surface) {

    app.SetNativeWindow(ANativeWindow_fromSurface(env, surface));
    app.SetUpCamera();
    std::thread loopThread(&Depth::CameraLoop, &app);
    loopThread.detach();
}

#ifdef __cplusplus
}
#endif