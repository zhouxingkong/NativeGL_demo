//
// Created by xingkong on 2017/8/14.
//

#ifndef NATIVEGL_DEMO2_GLINTERFACE_H
#define NATIVEGL_DEMO2_GLINTERFACE_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <pthread.h>
#include <cstdlib>
#include <math.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <unistd.h>
#include <sys/time.h>
#include <initializer_list>
#include <memory>
#include <jni.h>


class RenderPara{

public:

    enum RenderThreadMessage {
        MSG_NONE = 0,
        MSG_WINDOW_SET,
        MSG_RENDER_LOOP_EXIT
    };
    pthread_t _threadId;
    pthread_mutex_t _mutex;
    enum RenderThreadMessage _msg;

    // android window, supported by NDK r5 and newer
    ANativeWindow* _window;

    EGLDisplay _display;
    EGLSurface _surface;
    EGLContext _context;

    EGLint _width;
    EGLint _height;
};

class GLRenderer {
private:
    RenderPara renderPara;

//    int start;
    bool Inited=0;
    bool running=0;
    bool window_seted=0;
public:
    GLRenderer();
    ~GLRenderer();

    virtual GLuint loadShader(GLenum shaderType, const char* pSource);
    virtual GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);

    virtual void SurfaceCreate()=0;
    virtual void SurfaceChange(int width, int height)=0;
    virtual void DrawFrame()=0;


    virtual void SetWindow(JNIEnv *env, jobject surface);
    virtual bool InitRender();
    virtual void DestroyRender();
    virtual void StartRenderThread();
    virtual void StopRenderThread();
    static void * RenderThread(void *args);
    virtual void RenderLoop();
};



#endif //PHASEDARRAY2_0_GLINTERFACE_H
