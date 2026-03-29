#pragma once

#include <cstdint>

namespace mod {
namespace offsets {

inline constexpr uintptr_t kGainSoulstone = 0x5496534;
inline constexpr uintptr_t kGainGoldshard = 0x54BD508;
inline constexpr uintptr_t kObscuredDoubleGenerateKey = 0x2C16960;
inline constexpr uintptr_t kObscuredDoubleEncrypt = 0x2C16964;
inline constexpr uintptr_t kObscuredDoubleSetEncrypted = 0x2C16AC0;
inline constexpr uintptr_t kUserDataServiceGainIap = 0x54AE148;
inline constexpr uintptr_t kSafeCurrencyGain = 0x5641698;
inline constexpr uintptr_t kSafeNumberGain = 0x5641704;
inline constexpr uintptr_t kObscuredDoubleInternalDecrypt = 0x2C16B84;
inline constexpr uintptr_t kBattlerIsPlayerSelf = 0x52B50D0;
inline constexpr uintptr_t kBattlerPVal = 0x52A9428;
inline constexpr uintptr_t kBattlerCalcedPVal = 0x52A9410;
inline constexpr uintptr_t kBattlerAddPassive = 0x552A7AC;
inline constexpr uintptr_t kBattlerAddTemp = 0x55241F0;
inline constexpr uintptr_t kSecureKeyValueSetGain = 0x5642390;
inline constexpr uintptr_t kSecureKeyValueSetLose = 0x5642464;
inline constexpr uintptr_t kSecureKeyValueSetSet = 0x5642538;
inline constexpr uintptr_t kSecureKeyValueSetGet = 0x56425F0;
inline constexpr uintptr_t kSealedNumberGet = 0x564212C;
inline constexpr uintptr_t kBattlerFieldCurrentHP = 0x120;
inline constexpr uintptr_t kBattlerFieldMaxHP = 0x128;
inline constexpr uintptr_t kBattlerFieldCurrentShield = 0x140;
inline constexpr uintptr_t kBattlerFieldMaxShield = 0x148;
inline constexpr uintptr_t kBattlerFieldIsEnemy = 0x178;
inline constexpr uintptr_t kBattlerFieldBattleSkills = 0x1A8;
inline constexpr uintptr_t kUsableSkillCurrentCooldown = 0x5520A88;
inline constexpr uintptr_t kUsableSkillCurrentRemain = 0x55219A0;
inline constexpr uintptr_t kUsableSkillDamageTypesShort = 0x5521E78;
inline constexpr uintptr_t kUsableSkillPVal = 0x5520F48;
inline constexpr uintptr_t kUsableSkillCanUse = 0x551F368;
inline constexpr uintptr_t kUsableSkillCantUseReason = 0x5525C14;
inline constexpr uintptr_t kUsableSkillCurrentName = 0x5526CD4;
inline constexpr uintptr_t kUsableSkillUserPVal = 0x552182C;

}  // namespace offsets

namespace types {

struct ObscuredDoublePayload {
    uint64_t words[6];
};

struct Il2CppArrayHeader {
    void *klass;
    void *monitor;
    void *bounds;
    uintptr_t max_length;
};

struct SecureKeyValueSetEntry {
    int32_t hashCode;
    int32_t next;
    MonoString *key;
    void *value;
};

struct SecureKeyValueSetDictionary {
    void *klass;
    void *monitor;
    void *_buckets;
    void *_entries;
    int32_t _count;
    int32_t _freeList;
    int32_t _freeCount;
    int32_t _version;
    void *_comparer;
    void *_keys;
    void *_values;
    void *_syncRoot;
};

}  // namespace types
}  // namespace mod
