package com.lsp.ndkdemo.util;

/**
 * Created by Kenney on 2017-07-04 15:26
 */

public class FileUtil {
    /**
     * 文件拆分
     * @param path
     * @param pattern
     * @param count
     */
    public static native void diff(String path, String pattern, int count);

    /**
     * 文件合并
     * @param path
     * @param pattern
     * @param count
     */
    public static native void patch(String path, String pattern, int count);
}
