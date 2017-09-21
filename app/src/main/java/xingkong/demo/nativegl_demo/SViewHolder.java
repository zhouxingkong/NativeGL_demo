package xingkong.demo.nativegl_demo;

import android.view.SurfaceHolder;

/**
 * Created by xingkong on 2017/8/17.
 */

public class SViewHolder implements SurfaceHolder.Callback {
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        JNIProxy.SetSurfaceS(holder.getSurface());
        JNIProxy.StartRenderS();
    }

    public void surfaceCreated(SurfaceHolder holder) {
        JNIProxy.SetSurfaceS(holder.getSurface());
        JNIProxy.StartRenderS();
    }

    public void surfaceDestroyed(SurfaceHolder holder) {
        JNIProxy.SetSurfaceS(null);
    }
}
