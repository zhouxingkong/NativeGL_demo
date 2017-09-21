# NativeGL_demo

## 1方案介绍
在实际应用中，经常遇到OpenGL ES渲染性能达不到要求，图像卡顿等GG问题。这时，在不改变平台选型的情况下，选择一个好的方案实现OpenGL渲染很重要。
在android应用程序中使用OpenGL ES共有四种方案
1. 使用`GLSurfaceView`作为绘图的窗口，使用`GLSurfaceView.Renderer`实现OpenGL渲染上下文，并通过调用`android.opengl.GLES20`中的API函数实现对图像的渲染
2. 使用`GLSurfaceView`作为绘图的窗口，使用`GLSurfaceView.Renderer`实现OpenGL渲染上下文，和1不一样的是通过JNI接口调用`#include <GLES2/gl2.h>`中的API函数来实现图形渲染
3. 使用`NativeActivity`实现OpenGL渲染上下文，并通过JNI接口调用`#include <GLES2/gl2.h>`中的API函数来实现图形渲染。使用这种方案就意味着整个APP全部用C++语言编写，不能实现android基本控件的绘制，如Button/TextView
4. 最后一种方法，也是我所提倡使用的方法，就是使用SurfaceView作为绘图的窗口，并使用native层的pthread作为OpenGL渲染上下文，并通过pthread调用`#include <GLES2/gl2.h>`实现图形渲染

本例程以帮助类的形式将方案4的OpenGL渲染上下文封装在GLRender类中。用户使用时只需要继承GLRender类并像使用`GLSurfaceView.Renderer`接口一样，实现渲染上下文的回调函数，并在回调函数中调用`#include <GLES2/gl2.h>`中的API函数

## 2方案实现方法

使用本方案需要注意以下几点：
1. 编写CMakeLists.txt,添加如下语句。如果不添加会使程序编译GG。
``` cmake
# now build app's shared lib
#不加这句不能用std命名空间的函数
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

#使用这句将编译后的native-lib库和android、log、EGL、GLESv2四个库进行链接。
#不加这个会GG，将导致上面提到的四个功能无法使用
target_link_libraries( # Specifies the target library.
                       native-lib
                       # Links the target library to the log library
                       # included in the NDK.
                       android
                       log
                       EGL
                       GLESv2 )
```
2. 在布局文件中定义一个SurfaceView，并实现SurfaceHolder.Callback接口。在接口中使用JNI调用GLRender.SetWindow函数将SurfaceView与NativeWindow关联。以以便在Native空间进行绘图。
``` c++
public class SViewHolder implements SurfaceHolder.Callback {
//...............
    public void surfaceCreated(SurfaceHolder holder) {
        JNIProxy.SetSurfaceS(holder.getSurface());
        JNIProxy.StartRenderS();
    }
//................
}
```
3. 在Native空间继承GLRenderer类，并实现GLRenderer类中的以下抽象函数。不实现程序会GG
``` c++
/*
 * 下面三个函数为OpenGL渲染上下文回调函数，这三个函数需要用户通过继承GLRenderer类的方法来实现
 * 实现方法同android.opengl.GLSurfaceView.Render中的方法。详细请参考develop.google
 * */
virtual void SurfaceCreate()=0;
virtual void SurfaceChange(int width, int height)=0;
virtual void DrawFrame()=0;
```
