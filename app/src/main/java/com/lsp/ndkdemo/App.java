package com.lsp.ndkdemo;

import android.app.Application;

/**
 * Created by Kenney on 2017-07-04 15:30
 */

public class App extends Application {
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    @Override
    public void onCreate() {
        super.onCreate();
    }
}
