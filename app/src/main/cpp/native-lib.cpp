
#include "native-lib.h"


extern "C" {
    JNIEXPORT void JNICALL
    Java_xingkong_demo_nativegl_1demo_JNIProxy_CalPixel(JNIEnv *env, jclass type) {
        renderS=new GLrenderS;
        renderS->cal_pixel();
    }

    JNIEXPORT void JNICALL
    Java_xingkong_demo_nativegl_1demo_JNIProxy_StartRenderS(JNIEnv *env, jclass type) {
        renderS->StartRenderThread();
    }

    JNIEXPORT void JNICALL
    Java_xingkong_demo_nativegl_1demo_JNIProxy_StopRenderS(JNIEnv *env, jclass type) {
        renderS->StopRenderThread();
    }

    JNIEXPORT void JNICALL
    Java_xingkong_demo_nativegl_1demo_JNIProxy_SetSurfaceS(JNIEnv *env, jclass type, jobject surface) {
        renderS->SetWindow(env,surface);
    }
}

