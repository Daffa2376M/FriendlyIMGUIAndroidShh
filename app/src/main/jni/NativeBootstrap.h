#pragma once

#include <jni.h>

#include "EmbeddedOverlayDex.h"
#include "Includes/Logger.h"

extern "C" JNIEXPORT void JNICALL Java_com_unity3d_player_GLES3JNIView_init(JNIEnv *env, jclass clazz);
extern "C" JNIEXPORT void JNICALL Java_com_unity3d_player_GLES3JNIView_resize(JNIEnv *env, jobject obj, jint width, jint height);
extern "C" JNIEXPORT void JNICALL Java_com_unity3d_player_GLES3JNIView_step(JNIEnv *env, jobject obj);
extern "C" JNIEXPORT void JNICALL Java_com_unity3d_player_GLES3JNIView_imgui_1Shutdown(JNIEnv *env, jobject obj);
extern "C" JNIEXPORT void JNICALL Java_com_unity3d_player_GLES3JNIView_MotionEventClick(JNIEnv *env, jobject obj, jboolean down, jfloat pos_x, jfloat pos_y);
extern "C" JNIEXPORT jstring JNICALL Java_com_unity3d_player_GLES3JNIView_getWindowRect(JNIEnv *env, jobject thiz);

namespace mod::bootstrap {

inline jclass LoadClassFromLoader(JNIEnv *env, jobject class_loader, const char *class_name) {
    if (env == nullptr || class_loader == nullptr || class_name == nullptr) {
        return nullptr;
    }

    jclass class_loader_class = env->FindClass("java/lang/ClassLoader");
    if (class_loader_class == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }

    jmethodID load_class = env->GetMethodID(class_loader_class, "loadClass",
                                            "(Ljava/lang/String;)Ljava/lang/Class;");
    env->DeleteLocalRef(class_loader_class);
    if (load_class == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }

    jstring target_name = env->NewStringUTF(class_name);
    jobject class_obj = env->CallObjectMethod(class_loader, load_class, target_name);
    env->DeleteLocalRef(target_name);
    if (env->ExceptionCheck() || class_obj == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }

    return static_cast<jclass>(class_obj);
}

inline bool RegisterOverlayNatives(JNIEnv *env, jobject class_loader) {
    jclass gles_view_class = LoadClassFromLoader(env, class_loader, "com.unity3d.player.GLES3JNIView");
    if (gles_view_class == nullptr) {
        LOGE("Failed to load GLES3JNIView for RegisterNatives");
        return false;
    }

    JNINativeMethod methods[] = {
            {"init", "()V", reinterpret_cast<void *>(Java_com_unity3d_player_GLES3JNIView_init)},
            {"resize", "(II)V", reinterpret_cast<void *>(Java_com_unity3d_player_GLES3JNIView_resize)},
            {"step", "()V", reinterpret_cast<void *>(Java_com_unity3d_player_GLES3JNIView_step)},
            {"imgui_Shutdown", "()V", reinterpret_cast<void *>(Java_com_unity3d_player_GLES3JNIView_imgui_1Shutdown)},
            {"MotionEventClick", "(ZFF)V", reinterpret_cast<void *>(Java_com_unity3d_player_GLES3JNIView_MotionEventClick)},
            {"getWindowRect", "()Ljava/lang/String;", reinterpret_cast<void *>(Java_com_unity3d_player_GLES3JNIView_getWindowRect)},
    };

    const jint result = env->RegisterNatives(
            gles_view_class,
            methods,
            static_cast<jint>(sizeof(methods) / sizeof(methods[0])));
    env->DeleteLocalRef(gles_view_class);
    if (result != JNI_OK || env->ExceptionCheck()) {
        env->ExceptionClear();
        LOGE("RegisterNatives failed for GLES3JNIView");
        return false;
    }

    LOGI("RegisterNatives succeeded for GLES3JNIView");
    return true;
}

inline bool InvokeOverlayBootstrapClass(JNIEnv *env, jobject class_loader, jobject activity) {
    if (env == nullptr || class_loader == nullptr || activity == nullptr) {
        return false;
    }

    if (!RegisterOverlayNatives(env, class_loader)) {
        return false;
    }

    jclass bootstrap_class = LoadClassFromLoader(env, class_loader, "com.unity3d.player.UnityOverlayBootstrap");
    if (bootstrap_class == nullptr) {
        return false;
    }
    jmethodID start_method = env->GetStaticMethodID(bootstrap_class, "start", "(Landroid/app/Activity;)V");
    if (start_method == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(bootstrap_class);
        return false;
    }

    env->CallStaticVoidMethod(bootstrap_class, start_method, activity);
    const bool success = !env->ExceptionCheck();
    if (!success) {
        env->ExceptionClear();
    }
    env->DeleteLocalRef(bootstrap_class);
    return success;
}

inline bool BootstrapOverlayFromEmbeddedDex(JNIEnv *env, jobject activity) {
    if (env == nullptr || activity == nullptr) {
        return false;
    }

    jclass context_class = env->FindClass("android/content/Context");
    if (context_class == nullptr) {
        env->ExceptionClear();
        return false;
    }

    jmethodID get_class_loader = env->GetMethodID(context_class, "getClassLoader", "()Ljava/lang/ClassLoader;");
    if (get_class_loader == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(context_class);
        return false;
    }

    jobject app_class_loader = env->CallObjectMethod(activity, get_class_loader);
    env->DeleteLocalRef(context_class);
    if (env->ExceptionCheck() || app_class_loader == nullptr) {
        env->ExceptionClear();
        return false;
    }

    if (InvokeOverlayBootstrapClass(env, app_class_loader, activity)) {
        LOGI("Overlay bootstrap loaded from app class loader");
        env->DeleteLocalRef(app_class_loader);
        return true;
    }

    jclass dex_loader_class = env->FindClass("dalvik/system/InMemoryDexClassLoader");
    if (dex_loader_class == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(app_class_loader);
        LOGE("InMemoryDexClassLoader not available");
        return false;
    }

    jmethodID dex_loader_ctor = env->GetMethodID(
            dex_loader_class, "<init>", "(Ljava/nio/ByteBuffer;Ljava/lang/ClassLoader;)V");
    if (dex_loader_ctor == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(dex_loader_class);
        env->DeleteLocalRef(app_class_loader);
        LOGE("InMemoryDexClassLoader ctor missing");
        return false;
    }

    jobject dex_buffer = env->NewDirectByteBuffer(
            const_cast<unsigned char *>(kEmbeddedOverlayDex),
            static_cast<jlong>(kEmbeddedOverlayDexSize));
    if (dex_buffer == nullptr) {
        env->DeleteLocalRef(dex_loader_class);
        env->DeleteLocalRef(app_class_loader);
        LOGE("Failed to create ByteBuffer for embedded dex");
        return false;
    }

    jobject embedded_loader = env->NewObject(dex_loader_class, dex_loader_ctor, dex_buffer, app_class_loader);
    env->DeleteLocalRef(dex_buffer);
    env->DeleteLocalRef(dex_loader_class);
    env->DeleteLocalRef(app_class_loader);
    if (env->ExceptionCheck() || embedded_loader == nullptr) {
        env->ExceptionClear();
        LOGE("Failed to construct InMemoryDexClassLoader");
        return false;
    }

    const bool success = InvokeOverlayBootstrapClass(env, embedded_loader, activity);
    env->DeleteLocalRef(embedded_loader);
    if (success) {
        LOGI("Overlay bootstrap loaded from embedded dex");
    } else {
        LOGE("Failed to invoke overlay bootstrap from embedded dex");
    }
    return success;
}

}  // namespace mod::bootstrap
