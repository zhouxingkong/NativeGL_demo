package xingkong.demo.nativegl_demo;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceView;

public class MainActivity extends AppCompatActivity {
    private SurfaceView sSurfaceView=null;
    private SViewHolder sHolder=null;
    // Used to load the 'native-lib' library on application startup.


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        JNIProxy.CalPixel();
        sSurfaceView=(SurfaceView)findViewById(R.id.glViewS);
        sHolder=new SViewHolder();
        sSurfaceView.getHolder().addCallback(sHolder);

    }
    @Override
    protected void onResume() {
        super.onResume();
//        JNIProxy.StartRenderS();
    }
    @Override
    protected void onStop() {
        super.onStop();
        JNIProxy.StopRenderS();
    }

}
