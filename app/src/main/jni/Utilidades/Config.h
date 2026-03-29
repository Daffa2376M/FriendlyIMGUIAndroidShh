


//Exibe o Modelo do Dispositivo 

extern "C" void StartNativeRuntimeThreads();
JavaVM* javaVM;

jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    javaVM = vm; // Armazena o JavaVM globalmente
    StartNativeRuntimeThreads();
    return JNI_VERSION_1_6; // Retorna a versão suportada do JNI
}



JNIEnv* GetJNIEnv() {
    JNIEnv* env = nullptr;
    if (javaVM->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) {
        return nullptr;
    }
    return env;
}

	
	#include <string>

std::string GetDeviceModel(JNIEnv* env) {
  
    jclass buildClass = env->FindClass("android/os/Build");
    jfieldID modelFieldID = env->GetStaticFieldID(buildClass, "MODEL", "Ljava/lang/String;");
    jstring modelString = (jstring)env->GetStaticObjectField(buildClass, modelFieldID);

    const char* modelCStr = env->GetStringUTFChars(modelString, nullptr);
    std::string model(modelCStr);

    env->ReleaseStringUTFChars(modelString, modelCStr);
    env->DeleteLocalRef(modelString);
    env->DeleteLocalRef(buildClass);

    return model;
}

	
//Exibe a Hora do Dispositivo 
	
std::string GetCurrentDateTime() {
  
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%m-%d-%Y | %H:%M:%S", std::localtime(&now_time));
    return std::string(buffer);
}
