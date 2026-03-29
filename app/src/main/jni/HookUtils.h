#pragma once

#include <cstdint>

#include "Includes/Logger.h"
#include "Includes/Utils.h"

namespace mod::hooking {

inline void *ResolveOffset(const char *library_name, uintptr_t offset) {
    return reinterpret_cast<void *>(getAbsoluteAddress(library_name, offset));
}

template <typename FunctionT>
inline FunctionT ResolveFunctionAtOffset(const char *library_name, uintptr_t offset) {
    return reinterpret_cast<FunctionT>(ResolveOffset(library_name, offset));
}

template <typename OriginalT>
inline bool InstallHookAtAddress(const char *label,
                                 void *target,
                                 void *replacement,
                                 OriginalT **original,
                                 bool &installed_flag,
                                 uint64_t &address_slot) {
    address_slot = reinterpret_cast<uint64_t>(target);
    if (target == nullptr) {
        installed_flag = false;
        LOGE("%s target missing", label);
        return false;
    }

    hook(target, replacement, reinterpret_cast<void **>(original));
    installed_flag = original != nullptr && *original != nullptr;
    if (installed_flag) {
        LOGI("%s hook installed at %p", label, target);
    } else {
        LOGE("%s hook failed at %p", label, target);
    }
    return installed_flag;
}

template <typename OriginalT>
inline bool InstallHookAtOffset(const char *label,
                                const char *library_name,
                                uintptr_t offset,
                                void *replacement,
                                OriginalT **original,
                                bool &installed_flag,
                                uint64_t &address_slot) {
    void *target = ResolveOffset(library_name, offset);
    if (target == nullptr) {
        address_slot = 0;
        installed_flag = false;
        LOGE("%s target missing (offset 0x%llX)", label, static_cast<unsigned long long>(offset));
        return false;
    }

    return InstallHookAtAddress(label, target, replacement, original, installed_flag, address_slot);
}

}  // namespace mod::hooking
