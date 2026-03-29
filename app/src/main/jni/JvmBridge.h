#pragma once

#include <jni.h>

namespace mod::jvm {

struct ScopedEnvAttachment {
    JavaVM *vm = nullptr;
    JNIEnv *env = nullptr;
    bool attached = false;

    bool Attach(JavaVM *java_vm) {
        vm = java_vm;
        if (vm == nullptr) {
            return false;
        }

        const jint get_env_result = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        if (get_env_result == JNI_OK) {
            return true;
        }

        if (get_env_result != JNI_EDETACHED) {
            env = nullptr;
            return false;
        }

        JavaVMAttachArgs attach_args{};
        attach_args.version = JNI_VERSION_1_6;
        attach_args.name = const_cast<char *>("NUSRATESP-AutoBootstrap");
        attach_args.group = nullptr;
        if (vm->AttachCurrentThread(&env, &attach_args) != JNI_OK) {
            env = nullptr;
            return false;
        }

        attached = true;
        return true;
    }

    ~ScopedEnvAttachment() {
        if (attached && vm != nullptr) {
            vm->DetachCurrentThread();
        }
    }
};

inline jobject FindUnityCurrentActivity(JNIEnv *env) {
    if (env == nullptr) {
        return nullptr;
    }

    jclass unity_player_class = env->FindClass("com/unity3d/player/UnityPlayer");
    if (unity_player_class == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }

    jfieldID current_activity_field = env->GetStaticFieldID(
            unity_player_class, "currentActivity", "Landroid/app/Activity;");
    if (current_activity_field == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(unity_player_class);
        return nullptr;
    }

    jobject activity = env->GetStaticObjectField(unity_player_class, current_activity_field);
    if (env->ExceptionCheck()) {
        env->ExceptionClear();
        activity = nullptr;
    }
    env->DeleteLocalRef(unity_player_class);
    return activity;
}

inline jobject FindCurrentActivityViaActivityThread(JNIEnv *env) {
    if (env == nullptr) {
        return nullptr;
    }

    jclass activity_thread_class = env->FindClass("android/app/ActivityThread");
    if (activity_thread_class == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }

    jmethodID current_activity_thread = env->GetStaticMethodID(
            activity_thread_class, "currentActivityThread", "()Landroid/app/ActivityThread;");
    if (current_activity_thread == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(activity_thread_class);
        return nullptr;
    }

    jobject activity_thread = env->CallStaticObjectMethod(activity_thread_class, current_activity_thread);
    if (env->ExceptionCheck() || activity_thread == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(activity_thread_class);
        return nullptr;
    }

    jfieldID activities_field = env->GetFieldID(activity_thread_class, "mActivities", "Landroid/util/ArrayMap;");
    if (activities_field == nullptr) {
        env->ExceptionClear();
        activities_field = env->GetFieldID(activity_thread_class, "mActivities", "Ljava/util/Map;");
    }
    if (activities_field == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(activity_thread);
        env->DeleteLocalRef(activity_thread_class);
        return nullptr;
    }

    jobject activities = env->GetObjectField(activity_thread, activities_field);
    env->DeleteLocalRef(activity_thread);
    env->DeleteLocalRef(activity_thread_class);
    if (env->ExceptionCheck() || activities == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }

    jclass activities_class = env->GetObjectClass(activities);
    if (activities_class == nullptr) {
        env->DeleteLocalRef(activities);
        return nullptr;
    }

    jmethodID values_method = env->GetMethodID(activities_class, "values", "()Ljava/util/Collection;");
    env->DeleteLocalRef(activities_class);
    if (values_method == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(activities);
        return nullptr;
    }

    jobject values = env->CallObjectMethod(activities, values_method);
    env->DeleteLocalRef(activities);
    if (env->ExceptionCheck() || values == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }

    jclass collection_class = env->FindClass("java/util/Collection");
    jmethodID iterator_method = collection_class != nullptr
                                ? env->GetMethodID(collection_class, "iterator", "()Ljava/util/Iterator;")
                                : nullptr;
    if (collection_class != nullptr) {
        env->DeleteLocalRef(collection_class);
    }
    if (iterator_method == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(values);
        return nullptr;
    }

    jobject iterator = env->CallObjectMethod(values, iterator_method);
    env->DeleteLocalRef(values);
    if (env->ExceptionCheck() || iterator == nullptr) {
        env->ExceptionClear();
        return nullptr;
    }

    jclass iterator_class = env->FindClass("java/util/Iterator");
    jmethodID has_next_method = iterator_class != nullptr
                                ? env->GetMethodID(iterator_class, "hasNext", "()Z")
                                : nullptr;
    jmethodID next_method = iterator_class != nullptr
                            ? env->GetMethodID(iterator_class, "next", "()Ljava/lang/Object;")
                            : nullptr;
    if (iterator_class != nullptr) {
        env->DeleteLocalRef(iterator_class);
    }
    if (has_next_method == nullptr || next_method == nullptr) {
        env->ExceptionClear();
        env->DeleteLocalRef(iterator);
        return nullptr;
    }

    jobject fallback_activity = nullptr;
    while (env->CallBooleanMethod(iterator, has_next_method) == JNI_TRUE) {
        jobject record = env->CallObjectMethod(iterator, next_method);
        if (env->ExceptionCheck() || record == nullptr) {
            env->ExceptionClear();
            break;
        }

        jclass record_class = env->GetObjectClass(record);
        if (record_class == nullptr) {
            env->DeleteLocalRef(record);
            continue;
        }

        jfieldID paused_field = env->GetFieldID(record_class, "paused", "Z");
        jfieldID activity_field = env->GetFieldID(record_class, "activity", "Landroid/app/Activity;");
        if (paused_field == nullptr || activity_field == nullptr) {
            env->ExceptionClear();
            env->DeleteLocalRef(record_class);
            env->DeleteLocalRef(record);
            continue;
        }

        const jboolean paused = env->GetBooleanField(record, paused_field);
        jobject activity = env->GetObjectField(record, activity_field);
        env->DeleteLocalRef(record_class);
        env->DeleteLocalRef(record);

        if (env->ExceptionCheck()) {
            env->ExceptionClear();
            if (activity != nullptr) {
                env->DeleteLocalRef(activity);
            }
            continue;
        }

        if (activity == nullptr) {
            continue;
        }

        if (paused == JNI_FALSE) {
            if (fallback_activity != nullptr) {
                env->DeleteLocalRef(fallback_activity);
            }
            env->DeleteLocalRef(iterator);
            return activity;
        }

        if (fallback_activity == nullptr) {
            fallback_activity = activity;
        } else {
            env->DeleteLocalRef(activity);
        }
    }

    env->DeleteLocalRef(iterator);
    return fallback_activity;
}

inline jobject FindBestCurrentActivity(JNIEnv *env) {
    jobject unity_activity = FindUnityCurrentActivity(env);
    if (unity_activity != nullptr) {
        return unity_activity;
    }
    return FindCurrentActivityViaActivityThread(env);
}

}  // namespace mod::jvm
