package com.lsp.ndkdemo.util;

import java.util.UUID;

/**
 * Created by Kenney on 2017-08-16 10:29
 */

public class PosixThread {
    public native void init();//创建全局引用
    public native void posixThread();
    public native void destroy();//销毁全局引用
    public static String getUUID(){
        return UUID.randomUUID().toString();
    }
}
