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
1. 编写CmakeList.txt,添加如下语句。如果不添加会使程序编译GG。

