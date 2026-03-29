#pragma once

#include "GameOffsets.h"
#include "RuntimeMethodResolver.h"

namespace mod::hookspecs {

using mod::resolver::MethodSpec;
using mod::resolver::ReturnKind;

inline constexpr const char *kAssemblyCSharp = "Assembly-CSharp";

inline constexpr MethodSpec kGainSoulstone{
        "BB2UserStoredData$$GainSoulstone",
        "",
        "BB2UserStoredData",
        "GainSoulstone",
        2,
        mod::offsets::kGainSoulstone,
        ReturnKind::kVoid,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kGainGoldshard{
        "BB2UserStoredData$$GainGoldshard",
        "",
        "BB2UserStoredData",
        "GainGoldshard",
        2,
        mod::offsets::kGainGoldshard,
        ReturnKind::kVoid,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kBattlerIsPlayerSelf{
        "BattlerStoredData$$IsPlayerSelf",
        "",
        "BattlerStoredData",
        "IsPlayerSelf",
        0,
        mod::offsets::kBattlerIsPlayerSelf,
        ReturnKind::kBoolean,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kBattlerPVal{
        "BattlerStoredData$$PVal",
        "",
        "BattlerStoredData",
        "PVal",
        1,
        mod::offsets::kBattlerPVal,
        ReturnKind::kDouble,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kBattlerCalcedPVal{
        "BattlerStoredData$$CalcedPVal",
        "",
        "BattlerStoredData",
        "CalcedPVal",
        3,
        mod::offsets::kBattlerCalcedPVal,
        ReturnKind::kDouble,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kBattlerAddPassive{
        "BattlerStateSetStoredData$$AddPassive",
        "",
        "BattlerStateSetStoredData",
        "AddPassive",
        2,
        mod::offsets::kBattlerAddPassive,
        ReturnKind::kVoid,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kBattlerAddTemp{
        "BattlerStateSetStoredData$$AddTemp",
        "",
        "BattlerStateSetStoredData",
        "AddTemp",
        7,
        mod::offsets::kBattlerAddTemp,
        ReturnKind::kAny,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kBattlerDoAct{
        "BattlerStoredData$$DoAct",
        "",
        "BattlerStoredData",
        "DoAct",
        4,
        0,
        ReturnKind::kAny,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kBattlerDoTurnStartCommonAction{
        "BattlerStoredData$$DoTurnStartCommonAction",
        "",
        "BattlerStoredData",
        "DoTurnStartCommonAction",
        1,
        0,
        ReturnKind::kVoid,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kSecureKeyValueSetGain{
        "SecureKeyValueSet$$Gain",
        "",
        "SecureKeyValueSet",
        "Gain",
        2,
        mod::offsets::kSecureKeyValueSetGain,
        ReturnKind::kVoid,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kSecureKeyValueSetLose{
        "SecureKeyValueSet$$Lose",
        "",
        "SecureKeyValueSet",
        "Lose",
        2,
        mod::offsets::kSecureKeyValueSetLose,
        ReturnKind::kVoid,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kSecureKeyValueSetSet{
        "SecureKeyValueSet$$Set",
        "",
        "SecureKeyValueSet",
        "Set",
        2,
        mod::offsets::kSecureKeyValueSetSet,
        ReturnKind::kVoid,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kSecureKeyValueSetGet{
        "SecureKeyValueSet$$Get",
        "",
        "SecureKeyValueSet",
        "Get",
        1,
        mod::offsets::kSecureKeyValueSetGet,
        ReturnKind::kDouble,
        kAssemblyCSharp,
        true,
};

inline constexpr MethodSpec kBattlerCurrentSkill{
        "BattlerStoredData$$CurrentSkill",
        "",
        "BattlerStoredData",
        "CurrentSkill",
        0,
        0,
        ReturnKind::kAny,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kBattlerUsableSkills{
        "BattlerStoredData$$UsableSkills",
        "",
        "BattlerStoredData",
        "UsableSkills",
        0,
        0,
        ReturnKind::kAny,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kUsableSkillCurrentName{
        "UsableSkillStoredData$$CurrentName",
        "",
        "UsableSkillStoredData",
        "CurrentName",
        1,
        mod::offsets::kUsableSkillCurrentName,
        ReturnKind::kAny,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kUsableSkillDamageTypesShort{
        "UsableSkillStoredData$$strCurrentLastRemain_DamageTypes",
        "",
        "UsableSkillStoredData",
        "_strCurrentLastRemain_DamageTypes",
        1,
        mod::offsets::kUsableSkillDamageTypesShort,
        ReturnKind::kAny,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kUsableSkillCanUse{
        "UsableSkillStoredData$$CanUse",
        "",
        "UsableSkillStoredData",
        "CanUse",
        0,
        mod::offsets::kUsableSkillCanUse,
        ReturnKind::kBoolean,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kUsableSkillCantUseReason{
        "UsableSkillStoredData$$CantUseReason",
        "",
        "UsableSkillStoredData",
        "CantUseReason",
        0,
        mod::offsets::kUsableSkillCantUseReason,
        ReturnKind::kAny,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kUsableSkillCurrentCooldown{
        "UsableSkillStoredData$$CurrentCooldown",
        "",
        "UsableSkillStoredData",
        "CurrentCooldown",
        1,
        mod::offsets::kUsableSkillCurrentCooldown,
        ReturnKind::kInt32,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kUsableSkillCurrentRemain{
        "UsableSkillStoredData$$CurrentRemain",
        "",
        "UsableSkillStoredData",
        "CurrentRemain",
        2,
        mod::offsets::kUsableSkillCurrentRemain,
        ReturnKind::kInt32,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kUsableSkillPVal{
        "UsableSkillStoredData$$PVal",
        "",
        "UsableSkillStoredData",
        "PVal",
        1,
        mod::offsets::kUsableSkillPVal,
        ReturnKind::kDouble,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kUsableSkillUserPVal{
        "UsableSkillStoredData$$UserPVal",
        "",
        "UsableSkillStoredData",
        "UserPVal",
        3,
        mod::offsets::kUsableSkillUserPVal,
        ReturnKind::kInt32,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kSafeCurrencyGain{
        "SafeCurrency$$Gain",
        "",
        "SafeCurrency",
        "Gain",
        2,
        mod::offsets::kSafeCurrencyGain,
        ReturnKind::kVoid,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kSafeNumberGain{
        "SafeNumber$$Gain",
        "",
        "SafeNumber",
        "Gain",
        1,
        mod::offsets::kSafeNumberGain,
        ReturnKind::kVoid,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kSealedNumberGet{
        "SealedNumber$$Get",
        "",
        "SealedNumber",
        "Get",
        0,
        mod::offsets::kSealedNumberGet,
        ReturnKind::kDouble,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kUserDataServiceGainIap{
        "UserDataService$$GainIAP",
        nullptr,
        nullptr,
        nullptr,
        -1,
        mod::offsets::kUserDataServiceGainIap,
        ReturnKind::kAny,
        kAssemblyCSharp,
        false,
};

inline constexpr MethodSpec kObscuredDoubleGenerateKey{
        "ObscuredDouble$$GenerateKey",
        "CodeStage.AntiCheat.ObscuredTypes",
        "ObscuredDouble",
        "GenerateKey",
        0,
        mod::offsets::kObscuredDoubleGenerateKey,
        ReturnKind::kInt64,
        nullptr,
        false,
};

inline constexpr MethodSpec kObscuredDoubleEncrypt{
        "ObscuredDouble$$Encrypt",
        "CodeStage.AntiCheat.ObscuredTypes",
        "ObscuredDouble",
        "Encrypt",
        2,
        mod::offsets::kObscuredDoubleEncrypt,
        ReturnKind::kInt64,
        nullptr,
        false,
};

inline constexpr MethodSpec kObscuredDoubleSetEncrypted{
        "ObscuredDouble$$SetEncrypted",
        "CodeStage.AntiCheat.ObscuredTypes",
        "ObscuredDouble",
        "SetEncrypted",
        2,
        mod::offsets::kObscuredDoubleSetEncrypted,
        ReturnKind::kVoid,
        nullptr,
        false,
};

inline constexpr MethodSpec kObscuredDoubleInternalDecrypt{
        "ObscuredDouble$$InternalDecrypt",
        "CodeStage.AntiCheat.ObscuredTypes",
        "ObscuredDouble",
        "InternalDecrypt",
        0,
        mod::offsets::kObscuredDoubleInternalDecrypt,
        ReturnKind::kDouble,
        nullptr,
        false,
};

}  // namespace mod::hookspecs
