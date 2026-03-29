#pragma once

#include <dlfcn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#include "HookUtils.h"
#include "Includes/Logger.h"

namespace mod::resolver {

constexpr size_t kFingerprintProbeBytes = 262144;
constexpr uint64_t kExpectedLibil2cppSize = 113920896ull;
constexpr uint64_t kExpectedLibil2cppHeadHash = 0x5C22DF3F45C77557ull;

enum class ReturnKind {
    kAny,
    kVoid,
    kBoolean,
    kInt32,
    kInt64,
    kDouble,
};

enum class ResolveMode {
    kMissing,
    kByName,
    kFallbackOffset,
    kFallbackBlockedByVersionGuard,
};

struct MethodSpec {
    const char *label;
    const char *namespaze;
    const char *class_name;
    const char *method_name;
    int param_count;
    uintptr_t fallback_offset;
    ReturnKind return_kind;
    const char *preferred_image;
    bool critical;
};

struct ResolvedMethod {
    void *address = nullptr;
    const MethodInfo *method_info = nullptr;
    ResolveMode mode = ResolveMode::kMissing;
    bool signature_valid = false;
};

struct ApiState {
    void *handle = nullptr;
    bool ready = false;
    Il2CppDomain *(*domain_get)() = nullptr;
    const Il2CppAssembly **(*domain_get_assemblies)(const Il2CppDomain *domain, size_t *size) = nullptr;
    const Il2CppImage *(*assembly_get_image)(const Il2CppAssembly *assembly) = nullptr;
    const Il2CppClass *(*class_from_name)(const Il2CppImage *image, const char *namespaze, const char *name) = nullptr;
    const MethodInfo *(*class_get_method_from_name)(const Il2CppClass *klass, const char *name, int args_count) = nullptr;
    const char *(*class_get_name)(const Il2CppClass *klass) = nullptr;
    const char *(*class_get_namespace)(const Il2CppClass *klass) = nullptr;
    const Il2CppImage *(*class_get_image)(const Il2CppClass *klass) = nullptr;
    const Il2CppClass *(*method_get_class)(const MethodInfo *method) = nullptr;
    uint32_t (*method_get_param_count)(const MethodInfo *method) = nullptr;
    const Il2CppType *(*method_get_return_type)(const MethodInfo *method) = nullptr;
    const char *(*image_get_name)(const Il2CppImage *image) = nullptr;
};

struct VersionGuardState {
    bool initialized = false;
    bool fingerprint_ready = false;
    bool matches_expected = false;
    uint64_t file_size = 0;
    uint64_t head_hash = 0;
    char library_path[260] = "";
    char fingerprint_text[160] = "pending";
};

struct RuntimeStats {
    uint64_t by_name_count = 0;
    uint64_t fallback_count = 0;
    uint64_t blocked_fallback_count = 0;
};

using domain_get_t = Il2CppDomain *(*)();
using domain_get_assemblies_t = const Il2CppAssembly **(*)(const Il2CppDomain *domain, size_t *size);
using assembly_get_image_t = const Il2CppImage *(*)(const Il2CppAssembly *assembly);
using class_from_name_t = const Il2CppClass *(*)(const Il2CppImage *image, const char *namespaze, const char *name);
using class_get_method_from_name_t = const MethodInfo *(*)(const Il2CppClass *klass, const char *name, int args_count);
using class_get_name_t = const char *(*)(const Il2CppClass *klass);
using class_get_namespace_t = const char *(*)(const Il2CppClass *klass);
using class_get_image_t = const Il2CppImage *(*)(const Il2CppClass *klass);
using method_get_class_t = const Il2CppClass *(*)(const MethodInfo *method);
using method_get_param_count_t = uint32_t (*)(const MethodInfo *method);
using method_get_return_type_t = const Il2CppType *(*)(const MethodInfo *method);
using image_get_name_t = const char *(*)(const Il2CppImage *image);

inline ApiState &GetApiState() {
    static ApiState state;
    return state;
}

inline VersionGuardState &GetVersionGuardState() {
    static VersionGuardState state;
    return state;
}

inline RuntimeStats &GetRuntimeStats() {
    static RuntimeStats stats;
    return stats;
}

inline uint64_t Fnv1a64(const uint8_t *data, size_t length) {
    uint64_t hash = 0xcbf29ce484222325ull;
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 0x100000001b3ull;
    }
    return hash;
}

inline bool StringsEqual(const char *left, const char *right) {
    if (left == nullptr || right == nullptr) {
        return left == right;
    }
    return std::strcmp(left, right) == 0;
}

inline bool MatchesPreferredImage(const Il2CppImage *image, const char *preferred_image) {
    if (preferred_image == nullptr || preferred_image[0] == '\0') {
        return true;
    }
    ApiState &api = GetApiState();
    if (image == nullptr || api.image_get_name == nullptr) {
        return false;
    }

    const char *resolved_image_name = api.image_get_name(image);
    if (resolved_image_name == nullptr) {
        return false;
    }

    if (std::strcmp(resolved_image_name, preferred_image) == 0) {
        return true;
    }

    std::string preferred(preferred_image);
    if (preferred.find('.') == std::string::npos) {
        preferred += ".dll";
    }
    return preferred == resolved_image_name;
}

inline const char *GetResolveModeName(ResolveMode mode) {
    switch (mode) {
        case ResolveMode::kByName:
            return "by-name";
        case ResolveMode::kFallbackOffset:
            return "offset-fallback";
        case ResolveMode::kFallbackBlockedByVersionGuard:
            return "blocked-by-guard";
        case ResolveMode::kMissing:
        default:
            return "missing";
    }
}

inline void CopyResolveModeText(ResolveMode mode, char *buffer, size_t buffer_size) {
    if (buffer == nullptr || buffer_size == 0) {
        return;
    }
    std::snprintf(buffer, buffer_size, "%s", GetResolveModeName(mode));
}

inline bool EnsureApiLoaded(const char *library_name) {
    ApiState &state = GetApiState();
    if (state.ready) {
        return true;
    }

    state.handle = dlopen(library_name, RTLD_NOW);
    if (state.handle == nullptr) {
        LOGE("Resolver failed to dlopen %s: %s", library_name, dlerror());
        return false;
    }

    state.domain_get = reinterpret_cast<domain_get_t>(dlsym(state.handle, "il2cpp_domain_get"));
    state.domain_get_assemblies = reinterpret_cast<domain_get_assemblies_t>(dlsym(state.handle, "il2cpp_domain_get_assemblies"));
    state.assembly_get_image = reinterpret_cast<assembly_get_image_t>(dlsym(state.handle, "il2cpp_assembly_get_image"));
    state.class_from_name = reinterpret_cast<class_from_name_t>(dlsym(state.handle, "il2cpp_class_from_name"));
    state.class_get_method_from_name = reinterpret_cast<class_get_method_from_name_t>(dlsym(state.handle, "il2cpp_class_get_method_from_name"));
    state.class_get_name = reinterpret_cast<class_get_name_t>(dlsym(state.handle, "il2cpp_class_get_name"));
    state.class_get_namespace = reinterpret_cast<class_get_namespace_t>(dlsym(state.handle, "il2cpp_class_get_namespace"));
    state.class_get_image = reinterpret_cast<class_get_image_t>(dlsym(state.handle, "il2cpp_class_get_image"));
    state.method_get_class = reinterpret_cast<method_get_class_t>(dlsym(state.handle, "il2cpp_method_get_class"));
    state.method_get_param_count = reinterpret_cast<method_get_param_count_t>(dlsym(state.handle, "il2cpp_method_get_param_count"));
    state.method_get_return_type = reinterpret_cast<method_get_return_type_t>(dlsym(state.handle, "il2cpp_method_get_return_type"));
    state.image_get_name = reinterpret_cast<image_get_name_t>(dlsym(state.handle, "il2cpp_image_get_name"));

    state.ready = state.domain_get != nullptr &&
                  state.domain_get_assemblies != nullptr &&
                  state.assembly_get_image != nullptr &&
                  state.class_from_name != nullptr &&
                  state.class_get_method_from_name != nullptr &&
                  state.class_get_name != nullptr &&
                  state.class_get_namespace != nullptr &&
                  state.class_get_image != nullptr &&
                  state.method_get_class != nullptr &&
                  state.method_get_param_count != nullptr &&
                  state.method_get_return_type != nullptr &&
                  state.image_get_name != nullptr;

    if (!state.ready) {
        LOGE("Resolver missing il2cpp metadata exports");
    }
    return state.ready;
}

inline bool EnsureVersionGuardReady(const char *library_name) {
    VersionGuardState &state = GetVersionGuardState();
    if (state.initialized) {
        return state.fingerprint_ready;
    }

    state.initialized = true;

    if (!EnsureApiLoaded(library_name)) {
        std::snprintf(state.fingerprint_text, sizeof(state.fingerprint_text), "metadata api missing");
        return false;
    }

    void *probe_symbol = dlsym(GetApiState().handle, "il2cpp_domain_get");
    if (probe_symbol == nullptr) {
        std::snprintf(state.fingerprint_text, sizeof(state.fingerprint_text), "probe symbol missing");
        return false;
    }

    Dl_info info{};
    if (dladdr(probe_symbol, &info) == 0 || info.dli_fname == nullptr) {
        std::snprintf(state.fingerprint_text, sizeof(state.fingerprint_text), "dladdr failed");
        return false;
    }

    std::snprintf(state.library_path, sizeof(state.library_path), "%s", info.dli_fname);

    const int fd = open(state.library_path, O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        std::snprintf(state.fingerprint_text, sizeof(state.fingerprint_text), "open failed: %d", errno);
        return false;
    }

    struct stat st{};
    if (fstat(fd, &st) != 0) {
        close(fd);
        std::snprintf(state.fingerprint_text, sizeof(state.fingerprint_text), "stat failed: %d", errno);
        return false;
    }

    state.file_size = static_cast<uint64_t>(st.st_size);

    uint8_t buffer[kFingerprintProbeBytes];
    const ssize_t bytes_read = read(fd, buffer, sizeof(buffer));
    close(fd);
    if (bytes_read <= 0) {
        std::snprintf(state.fingerprint_text, sizeof(state.fingerprint_text), "read failed");
        return false;
    }

    state.head_hash = Fnv1a64(buffer, static_cast<size_t>(bytes_read));
    state.matches_expected =
            state.file_size == kExpectedLibil2cppSize &&
            state.head_hash == kExpectedLibil2cppHeadHash;
    state.fingerprint_ready = true;

    std::snprintf(state.fingerprint_text,
                  sizeof(state.fingerprint_text),
                  "size=%" PRIu64 " hash=%016" PRIX64 " %s",
                  state.file_size,
                  state.head_hash,
                  state.matches_expected ? "(known)" : "(new)");
    return true;
}

inline bool WaitForAssembliesReady(const char *library_name, int max_attempts = 40) {
    if (!EnsureApiLoaded(library_name)) {
        return false;
    }

    ApiState &api = GetApiState();
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        Il2CppDomain *domain = api.domain_get();
        if (domain != nullptr) {
            size_t assembly_count = 0;
            const Il2CppAssembly **assemblies = api.domain_get_assemblies(domain, &assembly_count);
            if (assemblies != nullptr && assembly_count > 0) {
                return true;
            }
        }
        usleep(250000);
    }

    LOGE("Resolver timed out waiting for IL2CPP domain assemblies");
    return false;
}

inline bool MatchesReturnType(const MethodInfo *method_info, ReturnKind return_kind) {
    if (return_kind == ReturnKind::kAny) {
        return true;
    }
    ApiState &api = GetApiState();
    if (method_info == nullptr || api.method_get_return_type == nullptr) {
        return false;
    }

    const Il2CppType *return_type = api.method_get_return_type(method_info);
    if (return_type == nullptr) {
        return false;
    }

    const auto type = return_type->type;
    switch (return_kind) {
        case ReturnKind::kVoid:
            return type == Il2CppTypeEnum::IL2CPP_TYPE_VOID;
        case ReturnKind::kBoolean:
            return type == Il2CppTypeEnum::IL2CPP_TYPE_BOOLEAN;
        case ReturnKind::kInt32:
            return type == Il2CppTypeEnum::IL2CPP_TYPE_I4;
        case ReturnKind::kInt64:
            return type == Il2CppTypeEnum::IL2CPP_TYPE_I8;
        case ReturnKind::kDouble:
            return type == Il2CppTypeEnum::IL2CPP_TYPE_R8;
        case ReturnKind::kAny:
        default:
            return true;
    }
}

inline bool ValidateMethodInfo(const MethodSpec &spec, const MethodInfo *method_info) {
    ApiState &api = GetApiState();
    if (method_info == nullptr || method_info->methodPointer == nullptr ||
        api.method_get_class == nullptr || api.class_get_name == nullptr ||
        api.class_get_namespace == nullptr || api.method_get_param_count == nullptr ||
        api.class_get_image == nullptr) {
        return false;
    }

    const Il2CppClass *declaring_class = api.method_get_class(method_info);
    if (declaring_class == nullptr) {
        return false;
    }

    const char *resolved_class_name = api.class_get_name(declaring_class);
    if (spec.class_name != nullptr && !StringsEqual(resolved_class_name, spec.class_name)) {
        return false;
    }

    const char *expected_namespace = spec.namespaze != nullptr ? spec.namespaze : "";
    const char *resolved_namespace = api.class_get_namespace(declaring_class);
    if (!StringsEqual(expected_namespace, resolved_namespace != nullptr ? resolved_namespace : "")) {
        return false;
    }

    if (!MatchesPreferredImage(api.class_get_image(declaring_class), spec.preferred_image)) {
        return false;
    }

    if (spec.param_count >= 0 && api.method_get_param_count(method_info) != static_cast<uint32_t>(spec.param_count)) {
        return false;
    }

    return MatchesReturnType(method_info, spec.return_kind);
}

inline const MethodInfo *ResolveMethodInfoByName(const char *library_name, const MethodSpec &spec) {
    if (!EnsureApiLoaded(library_name)) {
        return nullptr;
    }
    if (spec.class_name == nullptr || spec.method_name == nullptr || spec.param_count < 0) {
        return nullptr;
    }
    if (!WaitForAssembliesReady(library_name)) {
        return nullptr;
    }

    ApiState &api = GetApiState();
    Il2CppDomain *domain = api.domain_get();
    if (domain == nullptr) {
        return nullptr;
    }

    size_t assembly_count = 0;
    const Il2CppAssembly **assemblies = api.domain_get_assemblies(domain, &assembly_count);
    if (assemblies == nullptr || assembly_count == 0) {
        return nullptr;
    }

    for (size_t i = 0; i < assembly_count; ++i) {
        const Il2CppAssembly *assembly = assemblies[i];
        const Il2CppImage *image = api.assembly_get_image(assembly);
        if (!MatchesPreferredImage(image, spec.preferred_image)) {
            continue;
        }

        const Il2CppClass *klass = api.class_from_name(image, spec.namespaze != nullptr ? spec.namespaze : "", spec.class_name);
        if (klass == nullptr) {
            continue;
        }

        const MethodInfo *method_info = api.class_get_method_from_name(klass, spec.method_name, spec.param_count);
        if (!ValidateMethodInfo(spec, method_info)) {
            if (method_info != nullptr) {
                LOGE("Resolver rejected %s due to signature mismatch", spec.label);
            }
            continue;
        }

        return method_info;
    }

    return nullptr;
}

inline ResolvedMethod ResolveMethod(const char *library_name, const MethodSpec &spec) {
    EnsureVersionGuardReady(library_name);

    ResolvedMethod result;
    const MethodInfo *method_info = ResolveMethodInfoByName(library_name, spec);
    if (method_info != nullptr) {
        result.address = reinterpret_cast<void *>(method_info->methodPointer);
        result.method_info = method_info;
        result.signature_valid = true;
        result.mode = ResolveMode::kByName;
        ++GetRuntimeStats().by_name_count;
        LOGI("Resolver matched %s by-name at %p", spec.label, result.address);
        return result;
    }

    if (spec.fallback_offset == 0) {
        result.mode = ResolveMode::kMissing;
        LOGE("Resolver missing %s and no fallback offset is available", spec.label);
        return result;
    }

    const VersionGuardState &guard = GetVersionGuardState();
    if (!guard.matches_expected) {
        result.mode = ResolveMode::kFallbackBlockedByVersionGuard;
        ++GetRuntimeStats().blocked_fallback_count;
        LOGE("Resolver blocked offset fallback for %s because libil2cpp fingerprint changed", spec.label);
        return result;
    }

    result.address = mod::hooking::ResolveOffset(library_name, spec.fallback_offset);
    result.mode = result.address != nullptr ? ResolveMode::kFallbackOffset : ResolveMode::kMissing;
    if (result.address != nullptr) {
        ++GetRuntimeStats().fallback_count;
        LOGI("Resolver used offset fallback for %s at %p", spec.label, result.address);
    } else {
        LOGE("Resolver fallback target missing for %s at offset 0x%llX",
             spec.label,
             static_cast<unsigned long long>(spec.fallback_offset));
    }
    return result;
}

template <typename OriginalT>
inline bool InstallHook(const char *library_name,
                        const MethodSpec &spec,
                        void *replacement,
                        OriginalT **original,
                        bool &installed_flag,
                        uint64_t &address_slot,
                        char *mode_buffer = nullptr,
                        size_t mode_buffer_size = 0) {
    const ResolvedMethod resolved = ResolveMethod(library_name, spec);
    CopyResolveModeText(resolved.mode, mode_buffer, mode_buffer_size);
    if (resolved.address == nullptr) {
        address_slot = 0;
        installed_flag = false;
        return false;
    }

    return mod::hooking::InstallHookAtAddress(spec.label,
                                              resolved.address,
                                              replacement,
                                              original,
                                              installed_flag,
                                              address_slot);
}

template <typename FunctionT>
inline FunctionT ResolveFunction(const char *library_name,
                                 const MethodSpec &spec,
                                 uint64_t &address_slot,
                                 char *mode_buffer = nullptr,
                                 size_t mode_buffer_size = 0) {
    const ResolvedMethod resolved = ResolveMethod(library_name, spec);
    CopyResolveModeText(resolved.mode, mode_buffer, mode_buffer_size);
    address_slot = reinterpret_cast<uint64_t>(resolved.address);
    return reinterpret_cast<FunctionT>(resolved.address);
}

inline bool IsVersionGuardMatched() {
    return GetVersionGuardState().matches_expected;
}

inline bool IsVersionGuardReady() {
    return GetVersionGuardState().fingerprint_ready;
}

inline const char *GetFingerprintText() {
    return GetVersionGuardState().fingerprint_text;
}

inline uint64_t GetByNameResolveCount() {
    return GetRuntimeStats().by_name_count;
}

inline uint64_t GetFallbackResolveCount() {
    return GetRuntimeStats().fallback_count;
}

inline uint64_t GetBlockedFallbackResolveCount() {
    return GetRuntimeStats().blocked_fallback_count;
}

}  // namespace mod::resolver

