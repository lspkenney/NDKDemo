#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <android/log.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

//日志输出宏定义
#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO, "NDKLog",FORMAT,__VA_ARGS__)
#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG,"NDKLog",FORMAT,__VA_ARGS__)
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"NDKLog",FORMAT,__VA_ARGS__)

JavaVM* javaVM;
jobject posixThreadObj;
/*
 * 动态库加载时候调用
 * */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    javaVM = vm;
    LOGI("%s", "JNI_OnLoad");
    return JNI_VERSION_1_4;
}

void* start_fun(void* arg){
    JNIEnv *env = NULL;
    //(*javaVM)->GetEnv(javaVM, &env, JNI_VERSION_1_4);
    //挂载到JavaVM并获取当前线程的JNIEnv指针
    (*javaVM)->AttachCurrentThread(javaVM, &env, NULL);

    //必须采用GetObjectClass方式获取
    jclass clazz = (*env)->GetObjectClass(env, posixThreadObj);
    jmethodID mid = (*env)->GetStaticMethodID(env, clazz, "getUUID", "()Ljava/lang/String;");

    int name = (int)arg;
    int i = 0;
    for(; i < 5; i++){
        LOGI("thread %d print %d", name, i);
        jstring jstr = (*env)->CallStaticObjectMethod(env, clazz, mid);
        char* uuid = (*env)->GetStringUTFChars(env, jstr, NULL);
        LOGI("thread %d print %d uuid = %s", name, i, uuid);
        (*env)->ReleaseStringUTFChars(env, jstr, uuid);
        sleep(1);
    }
    (*javaVM)->DetachCurrentThread(javaVM);
    pthread_exit(0);
}

JNIEXPORT void
JNICALL
Java_com_lsp_ndkdemo_util_PosixThread_init(
        JNIEnv *env,
        jobject jobj) {
    posixThreadObj = (*env)->NewGlobalRef(env, jobj);
    LOGI("%s", "PosixThread_init");
}


JNIEXPORT void
JNICALL
Java_com_lsp_ndkdemo_util_PosixThread_posixThread(
        JNIEnv *env,
        jobject jobj) {
    pthread_t tids[5];//创建5个线程
    int i = 0;
    for(;i < 5; i++){
        pthread_create(&tids[i], NULL,start_fun, i);
    }
}


JNIEXPORT void
JNICALL
Java_com_lsp_ndkdemo_util_PosixThread_destroy(
        JNIEnv *env,
        jobject jobj) {
    (*env)->DeleteGlobalRef(env, posixThreadObj);
    LOGI("%s", "PosixThread_destroy");
}


JNIEXPORT jstring
JNICALL
Java_com_lsp_ndkdemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject jobj) {
    return (*env)->NewStringUTF(env, "Hello from C");
}

JNIEXPORT void JNICALL
Java_com_lsp_ndkdemo_MainActivity_accessJavaStringField(JNIEnv *env, jobject instance) {
    jclass clazz = (*env)->GetObjectClass(env, instance);
    jfieldID jfID = (*env)->GetFieldID(env, clazz, "name", "Ljava/lang/String;");
    jstring jstr = (*env)->GetObjectField(env, instance, jfID);

    char* cstr = (*env)->GetStringUTFChars(env, jstr, JNI_FALSE);

    char text[256] = "This is C Chars ";
    strcat(text, cstr);

    jstring res = (*env)->NewStringUTF(env, text);
    (*env)->SetObjectField(env, instance, jfID, res);

    (*env)->ReleaseStringUTFChars(env, jstr, cstr);
}

JNIEXPORT void JNICALL
Java_com_lsp_ndkdemo_MainActivity_accessJavaStaticStringField(JNIEnv *env, jobject instance) {
    jclass clazz = (*env)->GetObjectClass(env, instance);
    jfieldID jfID = (*env)->GetStaticFieldID(env, clazz, "desc", "Ljava/lang/String;");
    jstring  jstr = (*env)->GetStaticObjectField(env, clazz, jfID);

    char* cstr = (*env)->GetStringUTFChars(env, jstr, JNI_FALSE);

    char text[256] = "Char in C ";
    strcat(text, cstr);

    jstring res = (*env)->NewStringUTF(env, text);

    (*env)->SetStaticObjectField(env, clazz, jfID, res);

    //与GetStringUTFChars成对出现
    (*env)->ReleaseStringUTFChars(env, jstr, cstr);
}

//比较器
int compare(const int* a, const int* b){
    return (*a) -(*b);
}


JNIEXPORT void JNICALL
Java_com_lsp_ndkdemo_MainActivity_sortArray(JNIEnv *env, jobject instance, jintArray array_) {
    //Java数组转C数组
    jint *array = (*env)->GetIntArrayElements(env, array_, JNI_FALSE);

    jsize len = (*env)->GetArrayLength(env, array_);

    qsort(array, len, sizeof(jint), compare);
    //释放c数组并将数据同步到Java数组中
    /*0：            Java数组进行更新，并且释放C/C++数组。
    JNI_ABORT：    Java数组不进行更新，但是释放C/C++数组。
    JNI_COMMIT：    Java数组进行更新，不释放C/C++数组（函数执行完，数组还是会释放）。*/
    (*env)->ReleaseIntArrayElements(env, array_, array, JNI_COMMIT);
}

/**
 * 获取文件大小
 * @param file
 * @return
 */
long getFileSize(FILE* file){
    fseek(file, 0, SEEK_END);
    return ftell(file);
}

/**
 * 文件拆分
 * @param env
 * @param type
 * @param path_
 * @param pattern_
 * @param count
 */
JNIEXPORT void JNICALL
Java_com_lsp_ndkdemo_util_FileUtil_diff(JNIEnv *env, jclass type, jstring path_, jstring pattern_,
                                        jint count) {
    LOGI("开始拆分文件...\n",NULL);
    //需要分割的文件路径
    const char *path = (*env)->GetStringUTFChars(env, path_, 0);
    //分割之后的小文件命名规则
    const char *pattern = (*env)->GetStringUTFChars(env, pattern_, 0);

    //分割之后的小文件的命名列表(字符串数组)，分配内存
    char** patches = malloc(sizeof(char *) * count);
    int i = 0;
    for(;i < count; i++){
        //为每个小文件名称分配内存
        patches[i] = malloc(sizeof(char) * 100);
        //生成文件名
        sprintf(patches[i], pattern, i);
        //打印文件名
        LOGI("part%d name:%s\n",i, patches[i]);
    }

    //打开文件
    FILE* srcFile = fopen(path, "rb");
    //计算文件大小
    long fileSize = getFileSize(srcFile);
    //文件指针返回文件开头位置,计算文件大小时移动到了文件末尾
    fseek(srcFile, 0, SEEK_SET);
    LOGI("文件大小为:%ld\n", fileSize);
    if(fileSize % count == 0){//能整除
        //每部分大小
        int partSize = fileSize / count;
        LOGI("能整除，每部分大小为:%d\n", partSize);
        i = 0;
        for(; i < count; i++){
            FILE* fPart = fopen(patches[i], "wb");
            int j = 0;
            for(; j < partSize; j++){
                //从文件指针stream指向的文件中读取一个字符，读取一个字符后，光标位置后移一个字节。
                fputc(fgetc(srcFile), fPart);
            }
            fclose(fPart);
        }
    }else{//不能整除
        //前count -1每部分大小
        int partSize = fileSize / (count - 1);
        i = 0;
        for(; i < count; i++){
            FILE* fPart = fopen(patches[i], "wb");
            if(i == count - 1){//最后一部分的文件大小
                LOGI("不能整除，前%d个文件每个文件大小为:%d\n", (count - 1),partSize);
                partSize = fileSize % partSize;
                LOGI("不能整除，最后一部分文件大小为:%d\n", partSize);
            }

            int j = 0;
            for(; j < partSize; j++){
                fputc(fgetc(srcFile), fPart);
            }
            fclose(fPart);
        }
    }

    //释放内存
    fclose(srcFile);
    i = 0;
    for(; i < count; i++){
        free(patches[i]);
    }
    free(patches);

    (*env)->ReleaseStringUTFChars(env, path_, path);
    (*env)->ReleaseStringUTFChars(env, pattern_, pattern);
    LOGI("拆分文件结束...\n",NULL);
}

/**
 * 文件合并
 * @param env
 * @param type
 * @param path_
 * @param pattern_
 * @param count
 */
JNIEXPORT void JNICALL
Java_com_lsp_ndkdemo_util_FileUtil_patch(JNIEnv *env, jclass type, jstring path_, jstring pattern_,
                                         jint count) {
    LOGI("开始合并文件...\n",NULL);
    const char *path = (*env)->GetStringUTFChars(env, path_, 0);
    const char *pattern = (*env)->GetStringUTFChars(env, pattern_, 0);

    //文件名列表列表
    char** patches = malloc(sizeof(char*) * count);
    int i = 0;
    for(; i < count; i++){
        patches[i] = malloc(sizeof(char) * 100);
        sprintf(patches[i], pattern, i);
        LOGI("part%d name:%s\n",i, patches[i]);
    }

    //合并后的文件
    FILE* destFile = fopen(path, "wb");

    i = 0;
    for(; i < count; i++){
        FILE* fPart = fopen(patches[i], "rb");
        int partSize = getFileSize(fPart);
        fseek(fPart, 0, SEEK_SET);
        int j = 0;
        for(; j < partSize; j++){
            fputc(fgetc(fPart), destFile);
        }
        fclose(fPart);
    }

    //释放内存
    fclose(destFile);
    i = 0;
    for(; i < count; i++){
        free(patches[i]);
    }
    free(patches);

    (*env)->ReleaseStringUTFChars(env, path_, path);
    (*env)->ReleaseStringUTFChars(env, pattern_, pattern);
    LOGI("合并文件结束...\n",NULL);
}