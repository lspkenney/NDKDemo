package com.lsp.ndkdemo.util;

/**
 * Created by Kenney on 2017-07-24 14:49
 */

public class VoiceEffectUtils {
    public static final int TYPE_NORMAL = 0;
    public static final int TYPE_LUOLI = 1;
    public static final int TYPE_DASHU = 2;
    public static final int TYPE_JINGSONG = 3;
    public static final int TYPE_GAOGUAI = 4;
    public static final int TYPE_KONGLING = 5;

    static {
        System.loadLibrary("fmodL");
        System.loadLibrary("fmod");
        System.loadLibrary("effect-lib");
    }

    public native static void playFixVoice(String path, int type);
}
