//
// Created by xingkong on 2017/9/21.
//
#include "GLRenderer.h"

GLRenderer::GLRenderer() {
    pthread_mutex_init(&renderPara._mutex, 0);
}

GLRenderer::~GLRenderer() {
    pthread_join(renderPara._threadId,NULL);
    DestroyRender();
    running=0;
    Inited=0;
    pthread_mutex_destroy(&renderPara._mutex);
}

//编译shader
GLuint GLRenderer::loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        //检查错误
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

//编译shader,创建project
GLuint GLRenderer::createProgram(const char* pVertexSource, const char* pFragmentSource) {

    GLuint GvertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource); //读取编译shader
    GLuint GpixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, GvertexShader);
        glAttachShader(program, GpixelShader);
        glLinkProgram(program);
        //检查错误
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

void GLRenderer::SetWindow(JNIEnv *env, jobject surface) {
    // notify render thread that window has changed
    if (surface != 0) {
        renderPara._window  = ANativeWindow_fromSurface(env, surface);
        pthread_mutex_lock(&renderPara._mutex);
        renderPara._msg = renderPara.MSG_WINDOW_SET;
        pthread_mutex_unlock(&renderPara._mutex);

    } else {
        ANativeWindow_release(renderPara._window);
    }
    return;
}


bool GLRenderer::InitRender(){
    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_NONE
    };
    EGLint attribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE }; // OpenGL 2.0
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);
    /* Here, the application chooses the configuration it desires.
     * find the best match if possible, otherwise use the very first one
     */
    eglChooseConfig(display, attribs, nullptr,0, &numConfigs);
    std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
//    assert(supportedConfigs);
    eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);
//    assert(numConfigs);
    auto i = 0;
    for (; i < numConfigs; i++) {
        auto& cfg = supportedConfigs[i];
        EGLint r, g, b, d;
        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r)   &&
            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b)  &&
            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 && d == 0 ) {

            config = supportedConfigs[i];
            break;
        }
    }
    if (i == numConfigs) {
        config = supportedConfigs[0];
    }

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config,renderPara._window, NULL);
    context = eglCreateContext(display, config, NULL, attribList);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    renderPara._display = display;
    renderPara._surface = surface;
    renderPara._context = context;
    renderPara._width=w;
    renderPara._height=h;
    glEnable(GL_CULL_FACE);
//    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);

    SurfaceCreate();
    SurfaceChange(w,h);

    window_seted=1;

    return true;
}
void GLRenderer::DestroyRender() {

    eglMakeCurrent(renderPara._display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(renderPara._display, renderPara._context);
    eglDestroySurface(renderPara._display, renderPara._surface);
    eglTerminate(renderPara._display);

    renderPara._display = EGL_NO_DISPLAY;
    renderPara._surface = EGL_NO_SURFACE;
    renderPara._context = EGL_NO_CONTEXT;

    return;
}

void GLRenderer::StartRenderThread(){
    if(Inited==0){
        pthread_create(&renderPara._threadId, 0, RenderThread, this);
        Inited=1;
    }
    running=1;
}
void GLRenderer::StopRenderThread() {

//    // send message to render thread to stop rendering
//    pthread_mutex_lock(&renderPara._mutex);
//    renderPara._msg = renderPara.MSG_RENDER_LOOP_EXIT;
//    pthread_mutex_unlock(&renderPara._mutex);
//
//    pthread_join(renderPara._threadId, 0);
//    LOGE("Renderer thread stopped");
    running=0;

    return;
}

void * GLRenderer::RenderThread(void *args){
    GLRenderer *render= (GLRenderer*)args;
    render->RenderLoop();
    pthread_exit(0);
    return 0;
}

void GLRenderer::RenderLoop(){
    bool renderingEnabled = true;
    while (renderingEnabled) {
        if(running){

            // process incoming messages
            if(renderPara._msg==renderPara.MSG_WINDOW_SET){
                InitRender();
            }
            if(renderPara._msg==renderPara.MSG_RENDER_LOOP_EXIT){
                renderingEnabled = false;
                DestroyRender();
            }
            renderPara._msg = renderPara.MSG_NONE;

            if(window_seted==0){ //如果没有初始化窗口，就不能启动
                usleep(16000);
                continue;
            }

            if (renderPara._display) {
                pthread_mutex_lock(&renderPara._mutex);
                DrawFrame();
                if (!eglSwapBuffers(renderPara._display, renderPara._surface)) {
//                    LOGE("GLrenderS::eglSwapBuffers() returned error %d", eglGetError());
                }
                pthread_mutex_unlock(&renderPara._mutex);
            }
            else{
                usleep(16000);  //如果没有初始化好EGL，就等下一帧再渲染
            }
        }
        else{
            usleep(16000);  //如果没有初始化好EGL，就等下一帧再渲染
        }

    }

}


