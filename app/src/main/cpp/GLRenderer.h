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
    /*
     * 下面的函数为GLRender类内置函数，用户不需要实现
     * 其中声明为virtual的函数用户程序可覆盖实现
     * */
    GLRenderer();
    ~GLRenderer();
    virtual GLuint loadShader(GLenum shaderType, const char* pSource);  //加载着色器脚本（用户可调用
    virtual GLuint createProgram(const char* pVertexSource, const char* pFragmentSource);   //创建OpenGL渲染工程（用户可调用
    virtual bool InitRender();  //初始化EGL
    virtual void DestroyRender();   //清除EGL
    static void * RenderThread(void *args); //OpenGL渲染线程入口
    virtual void RenderLoop();  //渲染线程主循环

    /*
     * 将OpenGL渲染的窗口关联到java空间的Surface对象。实现在SurfaceView上的绘图
     * @env:JNI方法传入的JNIEnv对象
     * @surface:Surface对象
     */
    virtual void SetWindow(JNIEnv *env, jobject surface);
    /*
     * 开启OpenGL渲染线程。第一次调用时创建渲染线程。
     * 之后调用只是继续运行调用StartRenderThread函数后停止的渲染线程，并不会重新开启一个渲染线程。
     * 也就是说，每个GLRenderer对象只能创建一个渲染线程
     * */
    virtual void StartRenderThread();
    /*
     * 停止渲染线程，并不会释放线程资源，只是停止线程运行。
     * 当再次调用StartRenderThread函数后，线程继续运行。
     * */
    virtual void StopRenderThread();

    /*
     * 下面三个函数为OpenGL渲染上下文回调函数，这三个函数需要用户通过继承GLRenderer类的方法来实现
     * 实现方法同android.opengl.GLSurfaceView.Render中的方法。详细请参考develop.google
     * */
    virtual void SurfaceCreate()=0;
    virtual void SurfaceChange(int width, int height)=0;
    virtual void DrawFrame()=0;
};



#endif //PHASEDARRAY2_0_GLINTERFACE_H
