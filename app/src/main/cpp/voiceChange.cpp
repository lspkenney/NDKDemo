#include <jni.h>
#include "inc/fmod.hpp"
#include <stdlib.h>
#include <unistd.h>
#include <android/log.h>

#define LOGI(FORMAT, ...) __android_log_print(ANDROID_LOG_INFO, "NDKLog",FORMAT,__VA_ARGS__)
#define LOGD(FORMAT, ...) __android_log_print(ANDROID_LOG_DEBUG,"NDKLog",FORMAT,__VA_ARGS__)
#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"NDKLog",FORMAT,__VA_ARGS__)

//变声的类型
#define TYPE_NORMAL 0//正常
#define TYPE_LUOLI 1//萝莉
#define TYPE_DASHU 2//大叔
#define TYPE_JINGSONG 3//惊悚
#define TYPE_GAOGUAI 4//搞怪
#define TYPE_KONGLING 5//空灵

//使用fmod的命名空间
using namespace FMOD;

extern "C"{

    JNIEXPORT void JNICALL
    Java_com_lsp_ndkdemo_util_VoiceEffectUtils_playFixVoice(JNIEnv *env, jclass type_, jstring path_,
    jint type) {
        const char *path = env->GetStringUTFChars(path_, NULL);

        System* system = NULL;
        Sound* sound = NULL;
        Channel* channel = NULL;
        DSP* dsp = NULL;
        bool isPlaying = true;
        float  frequency;

        //fmod初始化
        System_Create(&system);
        //指定最大的声轨数等
        system->init(32, FMOD_INIT_NORMAL, NULL);

        //创建声音对象
        system->createSound(path, FMOD_DEFAULT, NULL, &sound);

        switch (type){
            case TYPE_NORMAL:
                //原声播放
                //指定的是音轨0，最后update的时候才会播放
                system->playSound(sound, 0, false, &channel);
                break;
            case TYPE_LUOLI://萝莉
                //创建一个数字信号处理对象DSP
                //DSP（数字信号处理）主要原理是：通过改变声音的两个参数：响度（振幅） 声调（频率）
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
                //设置参数，提高频率，升高一个八度
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 2.5);
                //把处理对象添加到Channel的音轨0中(注意这里要先播放然后添加音轨特效)
                system->playSound(sound, 0, false, &channel);
                channel->addDSP(0, dsp);
                LOGI("%s\n", "TYPE_LUOLI");
                break;
            case TYPE_DASHU://大叔
                system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &dsp);
                dsp->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, 0.8);
                system->playSound(sound, 0, false, & channel);
                channel->addDSP(0, dsp);
                LOGI("%s\n", "TYPE_DASHU");
                break;
            case TYPE_JINGSONG:
                system->createDSPByType(FMOD_DSP_TYPE_TREMOLO, &dsp);
                dsp->setParameterFloat(FMOD_DSP_TREMOLO_SKEW, 0.5);
                system->playSound(sound, 0, false, &channel);
                channel->addDSP(0, dsp);
                LOGI("%s\n", "TYPE_JINGSONG");
                break;
            case TYPE_GAOGUAI:
                //提高说话的速度
                system->playSound(sound, 0, false, &channel);
                channel->getFrequency(&frequency);
                frequency = frequency * 1.6;
                channel->setFrequency(frequency);
                LOGI("%s\n", "TYPE_GAOGUAI");
                break;
            case TYPE_KONGLING:
                system->createDSPByType(FMOD_DSP_TYPE_ECHO, &dsp);
                dsp->setParameterFloat(FMOD_DSP_ECHO_DELAY, 300);
                dsp->setParameterFloat(FMOD_DSP_ECHO_FEEDBACK, 20);
                system->playSound(sound, 0, false, &channel);
                channel->addDSP(0, dsp);
                LOGI("%s\n", "TYPE_KONGLING");
                break;
            default:
                break;
        }

        //update的时候才会播放
        system->update();
        //每秒钟判断一下是否在播放
        while (isPlaying){
            channel->isPlaying(&isPlaying);
            usleep(1* 1000 * 1000);//单位是微秒，这里是一秒延时
        }

        //释放资源
        sound->release();
        system->close();
        system->release();

        env->ReleaseStringUTFChars(path_, path);
    }

}
