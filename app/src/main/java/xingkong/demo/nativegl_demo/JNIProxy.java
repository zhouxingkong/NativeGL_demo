/*
*存放所有JNI接口函数
*
 */

package xingkong.demo.nativegl_demo;

import android.view.Surface;

public class JNIProxy {
    static {
        System.loadLibrary("native-lib");
    }
    //初始化函数
    public static native void CalPixel();
    //OpenGL函数--使用Native方案调用OpenGL使用下面6个函数
    public static native void StartRenderS();
    public static native void StopRenderS();
    public static native void SetSurfaceS(Surface surface);
}