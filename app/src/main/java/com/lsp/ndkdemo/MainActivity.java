package com.lsp.ndkdemo;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Spinner;
import android.widget.SpinnerAdapter;
import android.widget.TextView;
import android.widget.Toast;

import com.lsp.ndkdemo.util.FileUtil;
import com.lsp.ndkdemo.util.PosixThread;
import com.lsp.ndkdemo.util.VoiceEffectUtils;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private String name = "lsp";
    private static String desc = "This is desc";
    private TextView tv;
    private Spinner sp;

    private PosixThread posixThread;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

        sp = (Spinner) findViewById(R.id.sp);

        List<String> list = new ArrayList<String>();
        list.add("正常");
        list.add("萝莉");
        list.add("大叔");
        list.add("惊悚");
        list.add("搞怪");
        list.add("空灵");
        SpinnerAdapter adapter = new ArrayAdapter<String>(this, android.R.layout.simple_spinner_dropdown_item, list);
        sp.setAdapter(adapter);

        posixThread = new PosixThread();
        posixThread.init();
    }


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void accessJavaStringField();
    public native void accessJavaStaticStringField();
    //数组处理
    public native void sortArray(int array[]);

    public void accessJavaStringFieldClick(View view){
        accessJavaStringField();
        tv.setText(name);
    }

    public void accessJavaStaticStringFieldClick(View view){
        accessJavaStaticStringField();
        tv.setText(desc);
    }
    public void arraySortClick(View view){
        int arr[] = { 3, 2, 4, 5, 1, 0 };
        sortArray(arr);
        tv.setText(Arrays.toString(arr));
    }

    public void diffClick(View view) {
        final String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/video_20160605_121904.mp4";;//getAbsolutePath() + "/video_20160605_121904.mp4";
        final String pattern = Environment.getExternalStorageDirectory().getAbsolutePath() + "/patch-%d";
        File file = new File(path);
        if(!file.exists()){
            Toast.makeText(this, "文件不存在", Toast.LENGTH_SHORT).show();
            return ;
        }
        new Thread(){
            @Override
            public void run() {
                FileUtil.diff(path, pattern, 7);
            }
        }.start();

    }

    public void patchClick(View view) {
        final String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/video_20160605_121904_patched.mp4";;//getAbsolutePath() + "/video_20160605_121904.mp4";
        final String pattern = Environment.getExternalStorageDirectory().getAbsolutePath() + "/patch-%d";
        /*File file = new File(path);
        if(!file.exists()){
            Toast.makeText(this, "文件不存在", Toast.LENGTH_SHORT).show();
            return ;
        }*/
        new Thread(){
            @Override
            public void run() {
                FileUtil.patch(path, pattern, 7);
            }
        }.start();
    }

    public void play(View view) {
        final String path = Environment.getExternalStorageDirectory().getAbsolutePath() + "/test.ogg";
        File file = new File(path);
        if(!file.exists()){
            Toast.makeText(this, "文件不存在", Toast.LENGTH_SHORT).show();
            return ;
        }
        new Thread(){
            @Override
            public void run() {
                VoiceEffectUtils.playFixVoice(path, sp.getSelectedItemPosition());
            }
        }.start();

    }

    /**
     * 开启线程
     * @param view
     */
    public void startThread(View view) {

        posixThread.posixThread();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        posixThread.destroy();
    }
}
