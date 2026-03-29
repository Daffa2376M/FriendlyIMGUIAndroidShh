#include <jni.h>
#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>

#include <atomic>
#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

#include "Includes.h"
#include "Includes/Logger.h"
#include "Includes/obfuscate.h"
#include "Includes/MonoString.h"
#include "Includes/Utils.h"
#include "KittyMemory/MemoryPatch.h"
#include "Includes/Macros.h"
#include "Themes/Theme.h"
#include "Font/autohelp.h"
#include "Font/autofont.h"
#include "Font/NotoSansBold.h"
#include "Font/Roboto_Regular.h"
#include "Font/Iconcpp2.h"
#include "Unity/il2cpp/il2cpp.h"
#include "GameOffsets.h"
#include "GameHookSpecs.h"
#include "HookUtils.h"
#include "RuntimeMethodResolver.h"
#include "JvmBridge.h"
#include "NativeBootstrap.h"
#include "StatCatalogSeed.h"

#define targetLibName OBFUSCATE("libil2cpp.so")

using namespace ImGui;

int screenWidth = 0;
int screenHeight = 0;
int ScreenWidht = 0;
int ScreenHeight = 0;

#include "Variable.h"

void BeginDraw();

namespace {
constexpr const char *kGameLibraryName = "libil2cpp.so";
namespace offsets = mod::offsets;
namespace hookspecs = mod::hookspecs;
using mod::types::Il2CppArrayHeader;
using mod::types::ObscuredDoublePayload;
using mod::types::SecureKeyValueSetDictionary;
using mod::types::SecureKeyValueSetEntry;

bool g_Initialized = false;
bool g_ShowMenu = true;
bool g_UseOverlayRenderer = true;
bool g_Il2CppApiReady = false;
int g_FrameWidth = 0;
int g_FrameHeight = 0;
ImGuiWindow *g_window = nullptr;
void *g_LastPlayerBattler = nullptr;
void *g_LastEnemyBattler = nullptr;
bool g_TouchIsDown = false;
bool g_TouchMovedWhileDown = false;
float g_LastTouchX = 0.0f;
float g_LastTouchY = 0.0f;
float g_TouchDownStartX = 0.0f;
float g_TouchDownStartY = 0.0f;
float g_MainPendingTouchScrollDeltaY = 0.0f;
float g_MainTouchScrollVelocityY = 0.0f;
float g_PickerPendingTouchScrollDeltaY = 0.0f;
float g_PickerTouchScrollVelocityY = 0.0f;
bool g_IgnoreNextPickerSelection = false;
bool g_TextEditorPopupOpen = false;
uint64_t g_LastTapUpTimeMs = 0;
float g_LastTapUpX = 0.0f;
float g_LastTapUpY = 0.0f;
bool g_HasPendingDoubleTap = false;
float g_PendingDoubleTapX = 0.0f;
float g_PendingDoubleTapY = 0.0f;
thread_local int g_GainContextDepth = 0;
thread_local int g_CalcedPValDepth = 0;
thread_local int g_GoldshardRootDepth = 0;
thread_local int g_GoldshardSafeNumberDepth = 0;
thread_local int g_GoldshardDecryptIndex = 0;
std::atomic<int> g_PendingStatApplySlot{-1};
std::atomic<bool> g_OverlayBootstrapAttempted{false};
std::atomic<bool> g_OverlayBootstrapCompleted{false};
std::atomic<bool> g_OverlayBootstrapThreadStarted{false};
std::atomic<bool> g_NativeRuntimeStarted{false};

EGLBoolean (*old_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface) = nullptr;
void (*old_GainSoulstone)(void *instance, int value, MonoString *reason, void *method) = nullptr;
void (*old_GainGoldshard)(void *instance, ObscuredDoublePayload *value, MonoString *reason, void *method) = nullptr;
void (*old_UserDataServiceGainIap)(void *instance, ObscuredDoublePayload *value, void *reason, void *method) = nullptr;
void (*old_SafeCurrencyGain)(void *instance, ObscuredDoublePayload *value, void *reason, void *method) = nullptr;
void (*old_SafeNumberGain)(void *instance, ObscuredDoublePayload *value, void *method) = nullptr;
double (*old_ObscuredDoubleInternalDecrypt)(void *instance, void *method) = nullptr;
int64_t (*ObscuredDouble_GenerateKey)(void *method) = nullptr;
int64_t (*ObscuredDouble_Encrypt)(double value, int64_t key, void *method) = nullptr;
void (*ObscuredDouble_SetEncrypted)(void *instance, int64_t encrypted, int64_t key, void *method) = nullptr;
double (*ObscuredDouble_InternalDecrypt_Direct)(void *instance, void *method) = nullptr;
bool (*old_BattlerStoredDataIsPlayerSelf)(void *instance, void *method) = nullptr;
double (*old_BattlerPVal)(void *instance, MonoString *key, void *method) = nullptr;
double (*old_BattlerCalcedPVal)(void *instance, MonoString *key, double gained, double multid, void *method) = nullptr;
void (*old_BattlerAddPassive)(void *instance, MonoString *key, int value, void *method) = nullptr;
void *(*old_BattlerAddTemp)(void *instance, MonoString *key, int value, int act, int turn, int combat, int room, int floor, void *method) = nullptr;
void *(*old_BattlerDoAct)(void *instance, void *opponent, void *m_act, void *skill_status, bool need_return, void *method) = nullptr;
void (*old_BattlerDoTurnStartCommonAction)(void *instance, void *opponent, void *method) = nullptr;
void (*old_SecureKeyValueSetGain)(void *instance, MonoString *key, double value, void *method) = nullptr;
void (*old_SecureKeyValueSetLose)(void *instance, MonoString *key, double value, void *method) = nullptr;
void (*old_SecureKeyValueSetSet)(void *instance, MonoString *key, double value, void *method) = nullptr;
double (*old_SecureKeyValueSetGet)(void *instance, MonoString *key, void *method) = nullptr;
double (*SealedNumber_Get)(void *instance, void *method) = nullptr;
void *g_LastBattlerAddPassiveMethod = nullptr;
void *g_LastSecureKeyValueSetGainMethod = nullptr;
void *g_LastSecureKeyValueSetLoseMethod = nullptr;
void *g_LastPValMethod = nullptr;

void InstallGameHooks();
void *GetPlayerStatusPointer();
void *GetPlayerTotalsPointer();
void QueueStatApplyRequest(int slot_index);
bool ProcessPendingStatApplyIfNeeded();
void SeedObservedStatCatalog();
void ResetCalcEnemySnapshot();
void ForcePopulateCalcStatsImpl(void* battler);
void EnsureCalcStatsPrimed(void *battler);
void RememberEnemyCalcValue(void *instance, const std::string &key, double value, bool is_calced);
void RefreshCalcEnemyFromMemory();
void CaptureEnemyFromActionPair(void *actor, void *opponent, const char *source_tag);
bool TryBootstrapOverlay(jobject activity, const char *source_tag);
void *overlay_bootstrap_thread(void *);
void *hack_thread(void *);
extern "C" void StartOverlayBootstrapThread();
extern "C" void StartNativeRuntimeThreads();

struct GainContextScope {
    GainContextScope() {
        ++g_GainContextDepth;
    }

    ~GainContextScope() {
        --g_GainContextDepth;
    }
};

bool IsGainContextActive() {
    return g_GainContextDepth > 0;
}

struct GoldshardRootScope {
    GoldshardRootScope() {
        ++g_GoldshardRootDepth;
    }

    ~GoldshardRootScope() {
        --g_GoldshardRootDepth;
    }
};

struct GoldshardSafeNumberScope {
    GoldshardSafeNumberScope() {
        ++g_GoldshardSafeNumberDepth;
        g_GoldshardDecryptIndex = 0;
    }

    ~GoldshardSafeNumberScope() {
        --g_GoldshardSafeNumberDepth;
        g_GoldshardDecryptIndex = 0;
    }
};

bool IsGoldshardRootActive() {
    return g_GoldshardRootDepth > 0;
}

bool IsGoldshardSafeNumberActive() {
    return g_GoldshardSafeNumberDepth > 0;
}

void CopyStringToBuffer(const std::string &text, char *buffer, size_t buffer_size) {
    if (buffer_size == 0) {
        return;
    }

    if (text.empty()) {
        buffer[0] = '\0';
        return;
    }

    std::snprintf(buffer, buffer_size, "%s", text.c_str());
}

uint64_t GetNowMs() {
    return static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
}

void SetApplyStatus(const char *format, ...) {
    va_list args;
    va_start(args, format);
    std::vsnprintf(lastApplyStatus, sizeof(lastApplyStatus), format, args);
    va_end(args);
}

void RefreshResolverDebugState() {
    CopyStringToBuffer(mod::resolver::GetFingerprintText(), resolverFingerprintText, sizeof(resolverFingerprintText));
    resolverVersionGuardMatched = mod::resolver::IsVersionGuardMatched();
    resolverFingerprintReady = mod::resolver::IsVersionGuardReady();
    resolverByNameCount = mod::resolver::GetByNameResolveCount();
    resolverFallbackCount = mod::resolver::GetFallbackResolveCount();
    resolverBlockedFallbackCount = mod::resolver::GetBlockedFallbackResolveCount();
}

bool TryDecodeObscuredDouble(ObscuredDoublePayload *payload, double &out_value) {
    if (payload == nullptr || ObscuredDouble_InternalDecrypt_Direct == nullptr) {
        return false;
    }

    out_value = ObscuredDouble_InternalDecrypt_Direct(payload, nullptr);
    return true;
}

bool TryEncodeObscuredDouble(ObscuredDoublePayload *payload, double value) {
    if (payload == nullptr || ObscuredDouble_GenerateKey == nullptr ||
        ObscuredDouble_Encrypt == nullptr || ObscuredDouble_SetEncrypted == nullptr) {
        return false;
    }

    const int64_t key = ObscuredDouble_GenerateKey(nullptr);
    const int64_t encrypted = ObscuredDouble_Encrypt(value, key, nullptr);
    ObscuredDouble_SetEncrypted(payload, encrypted, key, nullptr);
    return true;
}

std::string GetManagedString(MonoString *value) {
    if (value == nullptr) {
        return std::string();
    }

    return value->toString();
}

bool TryBootstrapOverlay(jobject activity, const char *source_tag) {
    if (javaVM == nullptr || activity == nullptr) {
        return false;
    }

    mod::jvm::ScopedEnvAttachment scoped_env;
    if (!scoped_env.Attach(javaVM) || scoped_env.env == nullptr) {
        return false;
    }

    jobject activity_ref = activity;
    if (!scoped_env.env->IsSameObject(activity, nullptr)) {
        activity_ref = scoped_env.env->NewLocalRef(activity);
    }

    if (activity_ref == nullptr) {
        return false;
    }

    bool expected = false;
    if (!g_OverlayBootstrapAttempted.compare_exchange_strong(expected, true)) {
        scoped_env.env->DeleteLocalRef(activity_ref);
        return g_OverlayBootstrapCompleted.load();
    }

    const bool success = mod::bootstrap::BootstrapOverlayFromEmbeddedDex(scoped_env.env, activity_ref);
    scoped_env.env->DeleteLocalRef(activity_ref);
    if (success) {
        g_OverlayBootstrapCompleted.store(true);
        LOGI("Overlay bootstrap succeeded via %s", source_tag != nullptr ? source_tag : "unknown");
        return true;
    }

    g_OverlayBootstrapAttempted.store(false);
    LOGE("Overlay bootstrap failed via %s", source_tag != nullptr ? source_tag : "unknown");
    return false;
}

bool IsPlayerBattler(void *instance) {
    if (instance == nullptr || old_BattlerStoredDataIsPlayerSelf == nullptr) {
        return false;
    }

    return old_BattlerStoredDataIsPlayerSelf(instance, nullptr);
}

bool IsPlayerStatus(void *instance) {
    return instance != nullptr && instance == GetPlayerStatusPointer();
}

bool IsPlayerTotals(void *instance) {
    return instance != nullptr && instance == GetPlayerTotalsPointer();
}

void *ReadManagedFieldObject(void *instance, uintptr_t offset) {
    if (instance == nullptr) {
        return nullptr;
    }
    return *reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(instance) + offset);
}

bool ReadManagedBoolField(void *instance, uintptr_t offset) {
    if (instance == nullptr) {
        return false;
    }
    return *reinterpret_cast<bool *>(reinterpret_cast<uintptr_t>(instance) + offset);
}

double ReadSealedNumberValue(void *sealed_number) {
    if (sealed_number == nullptr || SealedNumber_Get == nullptr) {
        return 0.0;
    }
    return SealedNumber_Get(sealed_number, nullptr);
}

double ReadBattlerSealedNumber(void *instance, uintptr_t offset) {
    return ReadSealedNumberValue(ReadManagedFieldObject(instance, offset));
}

bool IsEnemyBattlerCandidate(void *instance) {
    if (instance == nullptr || instance == g_LastPlayerBattler) {
        return false;
    }
    if (old_BattlerStoredDataIsPlayerSelf != nullptr && old_BattlerStoredDataIsPlayerSelf(instance, nullptr)) {
        return false;
    }
    return ReadManagedBoolField(instance, offsets::kBattlerFieldIsEnemy);
}

void PromoteEnemyBattler(void *instance, const char *source_tag) {
    if (!IsEnemyBattlerCandidate(instance)) {
        return;
    }
    g_LastEnemyBattler = instance;
    calcEnemyCaptured = true;
    if (source_tag != nullptr && source_tag[0] != '\0') {
        std::snprintf(calcEnemySource, sizeof(calcEnemySource), "%s", source_tag);
    }
}

void CaptureEnemyFromActionPair(void *actor, void *opponent, const char *source_tag) {
    if (actor == nullptr || opponent == nullptr) {
        return;
    }

    const bool actor_is_player = IsPlayerBattler(actor);
    const bool opponent_is_player = IsPlayerBattler(opponent);

    if (actor_is_player && !opponent_is_player) {
        PromoteEnemyBattler(opponent, source_tag);
        return;
    }

    if (!actor_is_player && opponent_is_player) {
        PromoteEnemyBattler(actor, source_tag);
    }
}

void ResetObservedStats() {
    observedStatKeyCount = 0;
    std::memset(observedStatKeys, 0, sizeof(observedStatKeys));
    std::memset(observedStatSourceVal, 0, sizeof(observedStatSourceVal));
    std::memset(observedStatPVal, 0, sizeof(observedStatPVal));
    std::memset(observedStatCalcedVal, 0, sizeof(observedStatCalcedVal));
    std::memset(observedStatHasSourceVal, 0, sizeof(observedStatHasSourceVal));
    std::memset(observedStatHasPVal, 0, sizeof(observedStatHasPVal));
    std::memset(observedStatHasCalcedVal, 0, sizeof(observedStatHasCalcedVal));
    std::memset(lastObservedStatKey, 0, sizeof(lastObservedStatKey));
    SeedObservedStatCatalog();
}

std::string NormalizeCalcKey(const std::string &text) {
    std::string key = text;
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return key;
}

void ResetCalcEnemySnapshot() {
    calcEnemyCaptured = false;
    calcEnemyCurrentHp = 0.0;
    calcEnemyMaxHp = 0.0;
    calcEnemyShield = 0.0;
    calcEnemyPArmor = 0.0;
    calcEnemyMArmor = 0.0;
    calcTargetExpose = 0.0;
    calcTargetTakenMult = 1.0;
    calcMagicTakenMult = 1.0;
    calcHitChance = 1.0;
    calcFloorAmpElement = BB2CALC_ELEM_NONE;
    calcFloorElemAmp = 1.0;
    std::fill(std::begin(calcEnemyResist), std::end(calcEnemyResist), 0.0);
    std::snprintf(calcEnemySource, sizeof(calcEnemySource), "%s", "waiting");
    calcEnemyLastKey[0] = '\0';
    calcEnemySeenCount = 0;
    g_LastEnemyBattler = nullptr;
}

void RememberEnemyCalcValue(void *instance, const std::string &raw_key, double value, bool is_calced) {
    if (instance == nullptr || raw_key.empty()) {
        return;
    }

    const std::string key = NormalizeCalcKey(raw_key);
    if (key.empty()) {
        return;
    }

    PromoteEnemyBattler(instance, is_calced ? "enemy-calced" : "enemy-pval");
    if (g_LastEnemyBattler != instance) {
        return;
    }
    ++calcEnemySeenCount;
    CopyStringToBuffer(raw_key, calcEnemyLastKey, sizeof(calcEnemyLastKey));

    if (key == "currenthp" || key == "hp" || key == "currenthp#" || key == "hp#") {
        calcEnemyCurrentHp = value;
        return;
    }
    if (key.find("maxhp") != std::string::npos) {
        calcEnemyMaxHp = value;
        return;
    }
    if (key == "shield" || key == "shield#" || key == "currentshield") {
        calcEnemyShield = value;
        return;
    }
    if (key.find("maxshield") != std::string::npos) {
        calcEnemyShield = std::max(calcEnemyShield, value);
        return;
    }
    if (key == "parmor" || key == "physicalarmor") {
        calcEnemyPArmor = value;
        return;
    }
    if (key == "marmor" || key == "magicarmor") {
        calcEnemyMArmor = value;
        return;
    }
    if (key == "expose" || key == "armorbreak" || key == "armorpierced") {
        calcTargetExpose = std::clamp(value, 0.0, 1.0);
        return;
    }
    if (key == "damagetaken" || key == "takedmg" || key == "weaken") {
        calcTargetTakenMult = value > 0.0 ? value : calcTargetTakenMult;
        return;
    }
    if (key == "magicdamagetaken" || key == "cursemult" || key == "cursed") {
        calcMagicTakenMult = value > 0.0 ? value : calcMagicTakenMult;
        return;
    }
    if (key.find("fire") != std::string::npos && key.find("res") != std::string::npos) {
        calcEnemyResist[BB2CALC_ELEM_FIRE] = value;
        return;
    }
    if ((key.find("ice") != std::string::npos || key.find("cold") != std::string::npos) && key.find("res") != std::string::npos) {
        calcEnemyResist[BB2CALC_ELEM_ICE] = value;
        return;
    }
    if ((key.find("thunder") != std::string::npos || key.find("lightning") != std::string::npos) && key.find("res") != std::string::npos) {
        calcEnemyResist[BB2CALC_ELEM_THUNDER] = value;
        return;
    }
    if (key.find("poison") != std::string::npos && key.find("res") != std::string::npos) {
        calcEnemyResist[BB2CALC_ELEM_POISON] = value;
        return;
    }
    if (key.find("dark") != std::string::npos && key.find("res") != std::string::npos) {
        calcEnemyResist[BB2CALC_ELEM_DARK] = value;
        return;
    }
    if ((key.find("holy") != std::string::npos || key.find("light") != std::string::npos) && key.find("res") != std::string::npos) {
        calcEnemyResist[BB2CALC_ELEM_HOLY] = value;
        return;
    }
}

void RefreshCalcEnemyFromMemory() {
    if (!IsEnemyBattlerCandidate(g_LastEnemyBattler)) {
        return;
    }

    PromoteEnemyBattler(g_LastEnemyBattler, "enemy-memory");
    calcEnemyCurrentHp = ReadBattlerSealedNumber(g_LastEnemyBattler, offsets::kBattlerFieldCurrentHP);
    calcEnemyMaxHp = ReadBattlerSealedNumber(g_LastEnemyBattler, offsets::kBattlerFieldMaxHP);
    calcEnemyShield = ReadBattlerSealedNumber(g_LastEnemyBattler, offsets::kBattlerFieldCurrentShield);

    const double max_shield = ReadBattlerSealedNumber(g_LastEnemyBattler, offsets::kBattlerFieldMaxShield);
    if (calcEnemyShield <= 0.0 && max_shield > 0.0) {
        calcEnemyShield = max_shield;
    }

    if (calcEnemyCurrentHp > 0.0) {
        ++calcEnemySeenCount;
    }
}

void EnsureCalcStatsPrimed(void *battler) {
    static void *s_LastPrimedBattler = nullptr;
    if (battler == nullptr || !IsPlayerBattler(battler)) {
        return;
    }
    if (s_LastPrimedBattler == battler) {
        return;
    }
    ForcePopulateCalcStatsImpl(battler);
    s_LastPrimedBattler = battler;
}

double GetConfiguredStatBonus(const std::string &key) {
    return 0.0;
}

double PeekConfiguredStatBonus(const std::string &key) {
    return 0.0;
}

double ApplyStatBonusIfMatched(void *instance, const std::string &key, double value) {
    return value;
}

void AddObservedStatKey(const std::string &key) {
    if (key.empty()) {
        return;
    }

    for (int i = 0; i < observedStatKeyCount; ++i) {
        if (key == observedStatKeys[i]) {
            return;
        }
    }

    if (observedStatKeyCount >= kObservedStatCapacity) {
        return;
    }

    CopyStringToBuffer(key, observedStatKeys[observedStatKeyCount], sizeof(observedStatKeys[observedStatKeyCount]));
    CopyStringToBuffer(key, lastObservedStatKey, sizeof(lastObservedStatKey));
    ++observedStatKeyCount;
}

void SeedObservedStatCatalog() {
    for (int i = 0; i < kSeedStatCatalogCount; ++i) {
        AddObservedStatKey(kSeedStatCatalog[i]);
    }
}

int FindObservedStatIndex(const std::string &key) {
    if (key.empty()) {
        return -1;
    }

    for (int i = 0; i < observedStatKeyCount; ++i) {
        if (key == observedStatKeys[i]) {
            return i;
        }
    }

    return -1;
}

void RememberObservedStatValue(const std::string &key, double value, bool is_calced) {
    if (key.empty()) {
        return;
    }

    AddObservedStatKey(key);
    const int index = FindObservedStatIndex(key);
    if (index < 0 || index >= kObservedStatCapacity) {
        return;
    }

    if (is_calced) {
        observedStatCalcedVal[index] = value;
        observedStatHasCalcedVal[index] = true;
    } else {
        observedStatPVal[index] = value;
        observedStatHasPVal[index] = true;
    }
}

void RememberObservedStatSourceValue(const std::string &key, double value) {
    if (key.empty()) {
        return;
    }

    AddObservedStatKey(key);
    const int index = FindObservedStatIndex(key);
    if (index < 0 || index >= kObservedStatCapacity) {
        return;
    }

    observedStatSourceVal[index] = value;
    observedStatHasSourceVal[index] = true;
}

void AddDeltaToObservedStatValue(const std::string &key, double delta) {
    if (key.empty() || delta == 0.0) {
        return;
    }

    AddObservedStatKey(key);
    const int index = FindObservedStatIndex(key);
    if (index < 0 || index >= kObservedStatCapacity) {
        return;
    }

    if (observedStatHasSourceVal[index]) {
        observedStatSourceVal[index] += delta;
    } else {
        observedStatSourceVal[index] = delta;
        observedStatHasSourceVal[index] = true;
    }

    if (observedStatHasPVal[index]) {
        observedStatPVal[index] += delta;
    }

    if (observedStatHasCalcedVal[index]) {
        observedStatCalcedVal[index] += delta;
    }

    CopyStringToBuffer(key, lastObservedStatKey, sizeof(lastObservedStatKey));
}

void *GetPlayerStatusPointer() {
    if (g_LastPlayerBattler == nullptr) {
        return nullptr;
    }

    return *reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(g_LastPlayerBattler) + 0x1E8);
}

void *GetPlayerTotalsPointer() {
    void *status = GetPlayerStatusPointer();
    if (status == nullptr) {
        return nullptr;
    }

    return *reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(status) + 0x50);
}

bool EnsureIl2CppApiReady() {
    if (g_Il2CppApiReady && il2cpp_string_new != nullptr) {
        return true;
    }

    APIHook();
    g_Il2CppApiReady = il2cpp_string_new != nullptr;
    return g_Il2CppApiReady;
}

void QueueStatApplyRequest(int slot_index) {
    if (slot_index < 0 || slot_index >= kStatSlotCount) {
        ++statApplyFailureCount;
        SetApplyStatus("Slot apply nggak valid.");
        return;
    }

    if (statBonusKeys[slot_index][0] == '\0') {
        ++statApplyFailureCount;
        SetApplyStatus("Pilih key stat dulu sebelum apply.");
        return;
    }

    const int delta = static_cast<int>(std::lround(statBonusValues[slot_index]));
    if (delta == 0) {
        ++statApplyFailureCount;
        SetApplyStatus("Nilai add buat %s masih 0.", statBonusKeys[slot_index]);
        return;
    }

    g_PendingStatApplySlot.store(slot_index, std::memory_order_release);
    ++statApplyRequestCount;
    CopyStringToBuffer(statBonusKeys[slot_index], lastAppliedStatKey, sizeof(lastAppliedStatKey));
    lastAppliedStatDelta = delta;
    SetApplyStatus("Queued %s %+d, nunggu hook player jalan.", statBonusKeys[slot_index], delta);
}

bool ProcessPendingStatApplyIfNeeded() {
    const int slot_index = g_PendingStatApplySlot.load(std::memory_order_acquire);
    if (slot_index < 0 || slot_index >= kStatSlotCount) {
        return false;
    }

    if (statBonusKeys[slot_index][0] == '\0') {
        g_PendingStatApplySlot.store(-1, std::memory_order_release);
        ++statApplyFailureCount;
        SetApplyStatus("Apply dibatalin karena key slot kosong.");
        return false;
    }

    const int delta = static_cast<int>(std::lround(statBonusValues[slot_index]));
    if (delta == 0) {
        g_PendingStatApplySlot.store(-1, std::memory_order_release);
        ++statApplyFailureCount;
        SetApplyStatus("Apply dibatalin karena delta 0.");
        return false;
    }

    void *status = GetPlayerStatusPointer();
    if (status == nullptr || old_BattlerAddPassive == nullptr) {
        return false;
    }

    if (!EnsureIl2CppApiReady()) {
        return false;
    }

    MonoString *managed_key = il2cpp_string_new(statBonusKeys[slot_index]);
    if (managed_key == nullptr) {
        g_PendingStatApplySlot.store(-1, std::memory_order_release);
        ++statApplyFailureCount;
        SetApplyStatus("Gagal bikin MonoString buat %s.", statBonusKeys[slot_index]);
        return false;
    }

    int expected_slot = slot_index;
    if (!g_PendingStatApplySlot.compare_exchange_strong(expected_slot, -1, std::memory_order_acq_rel)) {
        return false;
    }

    old_BattlerAddPassive(status, managed_key, delta, g_LastBattlerAddPassiveMethod);

    void *totals = GetPlayerTotalsPointer();
    if (totals != nullptr) {
        if (delta > 0 && old_SecureKeyValueSetGain != nullptr) {
            old_SecureKeyValueSetGain(totals, managed_key, static_cast<double>(delta), g_LastSecureKeyValueSetGainMethod);
        } else if (delta < 0 && old_SecureKeyValueSetLose != nullptr) {
            old_SecureKeyValueSetLose(totals, managed_key, static_cast<double>(-delta), g_LastSecureKeyValueSetLoseMethod);
        }
    }

    const std::string key_text(statBonusKeys[slot_index]);
    AddDeltaToObservedStatValue(key_text, static_cast<double>(delta));
    CopyStringToBuffer(key_text, lastAppliedStatKey, sizeof(lastAppliedStatKey));
    CopyStringToBuffer(key_text, lastMatchedStatKey, sizeof(lastMatchedStatKey));
    lastAppliedStatDelta = delta;
    statBonusValues[slot_index] = 0.0f;
    ++statApplySuccessCount;
    SetApplyStatus("Applied %s %+d ke player.", statBonusKeys[slot_index], delta);
    return true;
}

void RefreshObservedStatsFromPlayerTotals() {
    auto *totals = reinterpret_cast<SecureKeyValueSetDictionary *>(GetPlayerTotalsPointer());
    if (totals == nullptr || totals->_entries == nullptr) {
        return;
    }

    auto *entries_array = reinterpret_cast<Il2CppArrayHeader *>(totals->_entries);
    if (entries_array->max_length == 0 || entries_array->max_length > 4096) {
        return;
    }

    auto *entries = reinterpret_cast<SecureKeyValueSetEntry *>(reinterpret_cast<uintptr_t>(entries_array) + sizeof(Il2CppArrayHeader));
    uintptr_t entry_count = static_cast<uintptr_t>(totals->_count);
    if (entry_count == 0 || entry_count > entries_array->max_length) {
        entry_count = entries_array->max_length;
    }

    for (uintptr_t i = 0; i < entry_count; ++i) {
        SecureKeyValueSetEntry &entry = entries[i];
        if (entry.hashCode < 0 || entry.key == nullptr || entry.value == nullptr) {
            continue;
        }

        const std::string key_text = GetManagedString(entry.key);
        if (key_text.empty()) {
            continue;
        }

        const double value = ReadSealedNumberValue(entry.value);
        RememberObservedStatSourceValue(key_text, value);
    }
}

void UpdateSurfaceSize(int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }

    g_FrameWidth = width;
    g_FrameHeight = height;
    screenWidth = width;
    screenHeight = height;
    ScreenWidht = width;
    ScreenHeight = height;
    glWidth = width;
    glHeight = height;

    if (g_Initialized) {
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        io.IniFilename = nullptr;
        io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));
    }
}

void SetupFontsAndStyle() {
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    SetBlue();
    constexpr float kBaseFontSize = 38.0f;
    constexpr float kUiScale = 5.2f;

    static const ImWchar icons_ranges[] = {
        0x0020, 0x00FF,
        0x0100, 0x017F,
        0xe000, 0xf538,
        0xf7f9, 0xf8ff,
        0x01, 0xffff, 0
    };

    ImFontConfig text_cfg;
    text_cfg.SizePixels = kBaseFontSize;
    text_cfg.GlyphRanges = io.Fonts->GetGlyphRangesCyrillic();
    g_MenuRegularFont = io.Fonts->AddFontFromMemoryTTF(Roboto_Regular, sizeof(Roboto_Regular), kBaseFontSize, &text_cfg);

    ImFontConfig bold_cfg = text_cfg;
    bold_cfg.FontDataOwnedByAtlas = false;
    g_MenuBoldFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(notosans_bold_compressed_data_base85, kBaseFontSize + 2.0f, &bold_cfg, io.Fonts->GetGlyphRangesCyrillic());

    ImFontConfig icons_cfg;
    icons_cfg.GlyphMinAdvanceX = kBaseFontSize - 1.0f;
    icons_cfg.PixelSnapH = true;
    icons_cfg.MergeMode = true;
    icons_cfg.OversampleH = 2.5f;
    icons_cfg.OversampleV = 2.5f;
    icons_cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryCompressedTTF(font_awesome_data, font_awesome_size, kBaseFontSize, &icons_cfg, icons_ranges);

    ImFontConfig fa_cfg;
    fa_cfg.MergeMode = true;
    static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0x0};
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(FontAwesome6_compressed_data_base85, 30.0f, &fa_cfg, icon_ranges);

    ImGui_ImplAndroid_Init(nullptr);
    ImGui_ImplOpenGL3_Init("#version 300 es");
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(kUiScale);
    style.TouchExtraPadding = ImVec2(12.0f, 12.0f);
    style.GrabMinSize = 28.0f;
    style.ScrollbarSize = 0.0f;
    g_Initialized = true;
    LOGI(OBFUSCATE("ImGui initialized"));
}

void EnsureImGuiInitialized() {
    if (g_Initialized || g_FrameWidth <= 0 || g_FrameHeight <= 0) {
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    SetupFontsAndStyle();
    ImGui::GetIO().DisplaySize = ImVec2(static_cast<float>(g_FrameWidth), static_cast<float>(g_FrameHeight));
    SeedObservedStatCatalog();
}

void RenderImGuiFrame(bool clear_surface) {
    EnsureImGuiInitialized();
    if (!g_Initialized) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplAndroid_NewFrame(g_FrameWidth, g_FrameHeight);
    ImGui::NewFrame();

    RefreshObservedStatsFromPlayerTotals();
    BeginDraw();

    ImGui::Render();

    if (clear_surface) {
        glViewport(0, 0, g_FrameWidth, g_FrameHeight);
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ShutdownImGui() {
    if (!g_Initialized) {
        return;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplAndroid_Shutdown();
    ImGui::DestroyContext();
    g_Initialized = false;
}

void ForwardTouch(bool down, float pos_x, float pos_y) {
    if (!g_Initialized) {
        return;
    }

    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[0] = down;
    io.MousePos = ImVec2(pos_x, pos_y);

    if (down) {
        if (!g_TouchIsDown) {
            g_TouchIsDown = true;
            g_TouchMovedWhileDown = false;
            g_LastTouchX = pos_x;
            g_LastTouchY = pos_y;
            g_TouchDownStartX = pos_x;
            g_TouchDownStartY = pos_y;
            if (activeStatPickerSlot >= 0 || activeTextEditorSlot != -2) {
                g_PickerPendingTouchScrollDeltaY = 0.0f;
                g_PickerTouchScrollVelocityY = 0.0f;
            } else {
                g_MainPendingTouchScrollDeltaY = 0.0f;
                g_MainTouchScrollVelocityY = 0.0f;
            }
            return;
        }

        const float delta_x = pos_x - g_LastTouchX;
        const float delta_y = pos_y - g_LastTouchY;
        const float total_delta_x = pos_x - g_TouchDownStartX;
        const float total_delta_y = pos_y - g_TouchDownStartY;
        const float total_distance = std::sqrt((total_delta_x * total_delta_x) + (total_delta_y * total_delta_y));
        if (total_distance > 10.0f || std::fabs(delta_x) > 1.5f || std::fabs(delta_y) > 1.5f) {
            g_TouchMovedWhileDown = true;
        }
        if (g_TouchMovedWhileDown && std::fabs(delta_y) > 0.05f) {
            if (activeStatPickerSlot >= 0 || activeTextEditorSlot != -2) {
                g_PickerPendingTouchScrollDeltaY += delta_y;
            } else {
                g_MainPendingTouchScrollDeltaY += delta_y;
            }
        }
        g_LastTouchX = pos_x;
        g_LastTouchY = pos_y;
    } else {
        const bool touch_was_scroll = g_TouchMovedWhileDown;
        if (g_TouchIsDown && !g_TouchMovedWhileDown) {
            const uint64_t now_ms = GetNowMs();
            const float delta_x = pos_x - g_LastTapUpX;
            const float delta_y = pos_y - g_LastTapUpY;
            const float distance_sq = (delta_x * delta_x) + (delta_y * delta_y);
            if (g_LastTapUpTimeMs != 0 &&
                (now_ms - g_LastTapUpTimeMs) <= 420 &&
                distance_sq <= (96.0f * 96.0f)) {
                g_HasPendingDoubleTap = true;
                g_PendingDoubleTapX = pos_x;
                g_PendingDoubleTapY = pos_y;
                g_LastTapUpTimeMs = 0;
            } else {
                g_LastTapUpTimeMs = now_ms;
                g_LastTapUpX = pos_x;
                g_LastTapUpY = pos_y;
            }
        }

        g_TouchIsDown = false;
        g_TouchMovedWhileDown = false;
        g_LastTouchX = pos_x;
        g_LastTouchY = pos_y;
        g_MainPendingTouchScrollDeltaY = 0.0f;
        g_PickerPendingTouchScrollDeltaY = 0.0f;
        if (touch_was_scroll) {
            g_IgnoreNextPickerSelection = true;
        }
    }
}

std::string GetWindowRectString() {
    char result[256] = "0|0|0|0";

    if ((activeStatPickerSlot >= 0 || g_TextEditorPopupOpen) &&
        g_FrameWidth > 0 && g_FrameHeight > 0) {
        snprintf(result, sizeof(result), "%d|%d|%d|%d", 0, 0, g_FrameWidth, g_FrameHeight);
        return std::string(result);
    }

    if (g_window != nullptr) {
        snprintf(result, sizeof(result), "%d|%d|%d|%d",
                 static_cast<int>(g_window->Pos.x),
                 static_cast<int>(g_window->Pos.y),
                 static_cast<int>(g_window->Size.x),
                 static_cast<int>(g_window->Size.y));
    }
    return std::string(result);
}

void GainGoldshard_Hook(void *instance, ObscuredDoublePayload *value, MonoString *reason, void *method) {
    if (old_GainGoldshard == nullptr) {
        return;
    }

    ++gainGoldshardHitCount;
    double original = 0.0;
    double modified = 0.0;
    if (TryDecodeObscuredDouble(value, original)) {
        modified = original;
        if (goldshardMultiplierEnabled && goldshardMultiplierValue > 1 && original > 0.0) {
            modified = original * static_cast<double>(goldshardMultiplierValue);
            TryEncodeObscuredDouble(value, modified);
        }
    }

    lastGoldshardOriginalValue = original;
    lastGoldshardModifiedValue = modified;
    CopyStringToBuffer(GetManagedString(reason), lastGoldshardReason, sizeof(lastGoldshardReason));
    old_GainGoldshard(instance, value, reason, method);
}

void GainSoulstone_Hook(void *instance, int value, MonoString *reason, void *method) {
    if (old_GainSoulstone == nullptr) {
        return;
    }

    ++gainSoulstoneHitCount;
    lastSoulstoneOriginalValue = static_cast<double>(value);
    lastSoulstoneModifiedValue = static_cast<double>(value);
    CopyStringToBuffer(GetManagedString(reason), lastSoulstoneReason, sizeof(lastSoulstoneReason));

    int modified = value;
    if (soulstoneMultiplierEnabled && soulstoneMultiplierValue > 1 && value > 0) {
        const int64_t scaled = static_cast<int64_t>(value) * static_cast<int64_t>(soulstoneMultiplierValue);
        if (scaled > static_cast<int64_t>(INT32_MAX)) {
            modified = INT32_MAX;
        } else if (scaled < static_cast<int64_t>(INT32_MIN)) {
            modified = INT32_MIN;
        } else {
            modified = static_cast<int>(scaled);
        }
    }

    lastSoulstoneModifiedValue = static_cast<double>(modified);
    old_GainSoulstone(instance, modified, reason, method);
}

void UserDataServiceGainIap_Hook(void *instance, ObscuredDoublePayload *value, void *reason, void *method) {
    ++userDataServiceGainIapHitCount;

    if (old_UserDataServiceGainIap != nullptr) {
        old_UserDataServiceGainIap(instance, value, reason, method);
    }
}

void SafeCurrencyGain_Hook(void *instance, ObscuredDoublePayload *value, void *reason, void *method) {
    ++safeCurrencyGainHitCount;
    if (old_SafeCurrencyGain != nullptr) {
        old_SafeCurrencyGain(instance, value, reason, method);
    }
}

void SafeNumberGain_Hook(void *instance, ObscuredDoublePayload *value, void *method) {
    ++safeNumberGainHitCount;

    if (old_SafeNumberGain != nullptr) {
        old_SafeNumberGain(instance, value, method);
    }
}

double ObscuredDoubleInternalDecrypt_Hook(void *instance, void *method) {
    double value = 0.0;
    if (old_ObscuredDoubleInternalDecrypt != nullptr) {
        value = old_ObscuredDoubleInternalDecrypt(instance, method);
    }

    ++obscuredDoubleDecryptHitCount;
    lastDecryptOriginalValue = value;
    lastDecryptModifiedValue = value;

    return value;
}

double BattlerPVal_Hook(void *instance, MonoString *key, void *method) {
    if (method != nullptr && g_LastPValMethod == nullptr) {
        g_LastPValMethod = method;
    }
    double value = 0.0;
    if (old_BattlerPVal != nullptr) {
        value = old_BattlerPVal(instance, key, method);
    }

    ++battlerPValHitCount;
    if (!IsPlayerBattler(instance)) {
        const std::string key_text = GetManagedString(key);
        PromoteEnemyBattler(instance, "enemy-pval");
        RememberEnemyCalcValue(instance, key_text, value, false);
        return value;
    }

    ProcessPendingStatApplyIfNeeded();
    EnsureCalcStatsPrimed(instance);

    const std::string key_text = GetManagedString(key);
    RememberObservedStatValue(key_text, value, false);
    CopyStringToBuffer(key_text, lastPValKey, sizeof(lastPValKey));
    lastPValOriginalValue = value;
    lastPValModifiedValue = value;

    if (g_CalcedPValDepth > 0) {
        return value;
    }

    const double modified = ApplyStatBonusIfMatched(instance, key_text, value);
    lastPValModifiedValue = modified;
    if (modified != value) {
        ++battlerPValModifiedCount;
    }

    return modified;
}

double BattlerCalcedPVal_Hook(void *instance, MonoString *key, double gained, double multid, void *method) {
    ++g_CalcedPValDepth;
    double value = 0.0;
    if (old_BattlerCalcedPVal != nullptr) {
        value = old_BattlerCalcedPVal(instance, key, gained, multid, method);
    }
    --g_CalcedPValDepth;

    ++battlerCalcedPValHitCount;
    if (!IsPlayerBattler(instance)) {
        const std::string key_text = GetManagedString(key);
        PromoteEnemyBattler(instance, "enemy-calced");
        RememberEnemyCalcValue(instance, key_text, value, true);
        return value;
    }

    ProcessPendingStatApplyIfNeeded();
    EnsureCalcStatsPrimed(instance);

    const std::string key_text = GetManagedString(key);
    RememberObservedStatValue(key_text, value, true);
    CopyStringToBuffer(key_text, lastCalcedPValKey, sizeof(lastCalcedPValKey));
    lastCalcedPValOriginalValue = value;
    lastCalcedPValModifiedValue = value;

    const double modified = ApplyStatBonusIfMatched(instance, key_text, value);
    lastCalcedPValModifiedValue = modified;
    if (modified != value) {
        ++battlerCalcedPValModifiedCount;
    }

    return modified;
}

void BattlerAddPassive_Hook(void *instance, MonoString *key, int value, void *method) {
    ++battlerAddPassiveHitCount;
    g_LastBattlerAddPassiveMethod = method;
    if (IsPlayerStatus(instance)) {
        ProcessPendingStatApplyIfNeeded();
        const std::string key_text = GetManagedString(key);
        RememberObservedStatSourceValue(key_text, static_cast<double>(value));
        CopyStringToBuffer(key_text, lastAddPassiveKey, sizeof(lastAddPassiveKey));
        lastAddPassiveValue = value;
    }

    if (old_BattlerAddPassive != nullptr) {
        old_BattlerAddPassive(instance, key, value, method);
    }
}

void *BattlerAddTemp_Hook(void *instance, MonoString *key, int value, int act, int turn, int combat, int room, int floor, void *method) {
    ++battlerAddTempHitCount;
    if (IsPlayerStatus(instance)) {
        ProcessPendingStatApplyIfNeeded();
        const std::string key_text = GetManagedString(key);
        RememberObservedStatSourceValue(key_text, static_cast<double>(value));
        CopyStringToBuffer(key_text, lastAddTempKey, sizeof(lastAddTempKey));
        lastAddTempValue = value;
    }

    if (old_BattlerAddTemp != nullptr) {
        return old_BattlerAddTemp(instance, key, value, act, turn, combat, room, floor, method);
    }

    return nullptr;
}

void *BattlerDoAct_Hook(void *instance, void *opponent, void *m_act, void *skill_status, bool need_return, void *method) {
    ++battlerDoActHitCount;
    if (IsPlayerBattler(instance)) {
        ForcePopulateCalcStatsImpl(instance);
    }
    CaptureEnemyFromActionPair(instance, opponent, "enemy-doact");
    RefreshCalcEnemyFromMemory();

    if (old_BattlerDoAct != nullptr) {
        return old_BattlerDoAct(instance, opponent, m_act, skill_status, need_return, method);
    }
    return nullptr;
}

void BattlerDoTurnStartCommonAction_Hook(void *instance, void *opponent, void *method) {
    ++battlerTurnStartHitCount;
    if (IsPlayerBattler(instance)) {
        ForcePopulateCalcStatsImpl(instance);
    }
    CaptureEnemyFromActionPair(instance, opponent, "enemy-turnstart");
    RefreshCalcEnemyFromMemory();

    if (old_BattlerDoTurnStartCommonAction != nullptr) {
        old_BattlerDoTurnStartCommonAction(instance, opponent, method);
    }
}

void SecureKeyValueSetGain_Hook(void *instance, MonoString *key, double value, void *method) {
    ++secureKeyValueSetGainHitCount;
    g_LastSecureKeyValueSetGainMethod = method;
    if (IsPlayerTotals(instance)) {
        ProcessPendingStatApplyIfNeeded();
        const std::string key_text = GetManagedString(key);
        RememberObservedStatSourceValue(key_text, value);
        CopyStringToBuffer(key_text, lastSecureSetKey, sizeof(lastSecureSetKey));
        lastSecureSetValue = value;
    }

    if (old_SecureKeyValueSetGain != nullptr) {
        old_SecureKeyValueSetGain(instance, key, value, method);
    }
}

void SecureKeyValueSetLose_Hook(void *instance, MonoString *key, double value, void *method) {
    ++secureKeyValueSetLoseHitCount;
    g_LastSecureKeyValueSetLoseMethod = method;
    if (IsPlayerTotals(instance)) {
        ProcessPendingStatApplyIfNeeded();
        const std::string key_text = GetManagedString(key);
        RememberObservedStatSourceValue(key_text, value);
        CopyStringToBuffer(key_text, lastSecureSetKey, sizeof(lastSecureSetKey));
        lastSecureSetValue = value;
    }

    if (old_SecureKeyValueSetLose != nullptr) {
        old_SecureKeyValueSetLose(instance, key, value, method);
    }
}

void SecureKeyValueSetSet_Hook(void *instance, MonoString *key, double value, void *method) {
    ++secureKeyValueSetSetHitCount;
    if (IsPlayerTotals(instance)) {
        ProcessPendingStatApplyIfNeeded();
        const std::string key_text = GetManagedString(key);
        RememberObservedStatSourceValue(key_text, value);
        CopyStringToBuffer(key_text, lastSecureSetKey, sizeof(lastSecureSetKey));
        lastSecureSetValue = value;
    }

    if (old_SecureKeyValueSetSet != nullptr) {
        old_SecureKeyValueSetSet(instance, key, value, method);
    }
}

double SecureKeyValueSetGet_Hook(void *instance, MonoString *key, void *method) {
    double value = 0.0;
    if (old_SecureKeyValueSetGet != nullptr) {
        value = old_SecureKeyValueSetGet(instance, key, method);
    }

    ++secureKeyValueSetGetHitCount;
    if (IsPlayerTotals(instance)) {
        ProcessPendingStatApplyIfNeeded();
        const std::string key_text = GetManagedString(key);
        RememberObservedStatSourceValue(key_text, value);
        CopyStringToBuffer(key_text, lastSecureSetKey, sizeof(lastSecureSetKey));
        lastSecureSetValue = value;
        return value;
    }
    return value;
}

bool BattlerStoredDataIsPlayerSelf_Hook(void *instance, void *method) {
    bool result = false;
    if (old_BattlerStoredDataIsPlayerSelf != nullptr) {
        result = old_BattlerStoredDataIsPlayerSelf(instance, method);
    }

    ++battlerIsPlayerSelfHitCount;
    if (result && instance != nullptr) {
        if (g_LastPlayerBattler != instance) {
            ResetObservedStats();
            ResetCalcEnemySnapshot();
        }
        g_LastPlayerBattler = instance;
        hasCapturedPlayerBattler = true;
        ProcessPendingStatApplyIfNeeded();
        EnsureCalcStatsPrimed(instance);
    } else if (!result && instance != nullptr && IsEnemyBattlerCandidate(instance)) {
        PromoteEnemyBattler(instance, "enemy-selfcheck");
    }

    return result;
}

EGLBoolean HookedEglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    EGLint width = 0;
    EGLint height = 0;
    eglQuerySurface(dpy, surface, EGL_WIDTH, &width);
    eglQuerySurface(dpy, surface, EGL_HEIGHT, &height);

    UpdateSurfaceSize(width, height);
    RenderImGuiFrame(false);

    return old_eglSwapBuffers(dpy, surface);
}

void InstallGameHooks() {
    LOGI(OBFUSCATE("Waiting for libil2cpp.so"));

    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));

    g_Il2CppApiReady = EnsureIl2CppApiReady();
    LOGI(OBFUSCATE("IL2CPP API ready: %s"), g_Il2CppApiReady ? "yes" : "no");

    mod::resolver::EnsureVersionGuardReady(kGameLibraryName);
    RefreshResolverDebugState();

    uint64_t helper_address = 0;
    char generate_key_mode[32] = "pending";
    char encrypt_mode[32] = "pending";
    char set_encrypted_mode[32] = "pending";
    char decrypt_mode[32] = "pending";

    userDataServiceGainIapTargetAddress =
            reinterpret_cast<uint64_t>(mod::resolver::ResolveMethod(kGameLibraryName, hookspecs::kUserDataServiceGainIap).address);
    safeCurrencyTargetAddress =
            reinterpret_cast<uint64_t>(mod::resolver::ResolveMethod(kGameLibraryName, hookspecs::kSafeCurrencyGain).address);
    safeNumberTargetAddress =
            reinterpret_cast<uint64_t>(mod::resolver::ResolveMethod(kGameLibraryName, hookspecs::kSafeNumberGain).address);

    SealedNumber_Get = mod::resolver::ResolveFunction<double (*)(void *, void *)>(
            kGameLibraryName,
            hookspecs::kSealedNumberGet,
            helper_address,
            sealedNumberGetResolveMode,
            sizeof(sealedNumberGetResolveMode));
    ObscuredDouble_GenerateKey = mod::resolver::ResolveFunction<int64_t (*)(void *)>(
            kGameLibraryName,
            hookspecs::kObscuredDoubleGenerateKey,
            helper_address,
            generate_key_mode,
            sizeof(generate_key_mode));
    ObscuredDouble_Encrypt = mod::resolver::ResolveFunction<int64_t (*)(double, int64_t, void *)>(
            kGameLibraryName,
            hookspecs::kObscuredDoubleEncrypt,
            helper_address,
            encrypt_mode,
            sizeof(encrypt_mode));
    ObscuredDouble_SetEncrypted = mod::resolver::ResolveFunction<void (*)(void *, int64_t, int64_t, void *)>(
            kGameLibraryName,
            hookspecs::kObscuredDoubleSetEncrypted,
            helper_address,
            set_encrypted_mode,
            sizeof(set_encrypted_mode));
    ObscuredDouble_InternalDecrypt_Direct = mod::resolver::ResolveFunction<double (*)(void *, void *)>(
            kGameLibraryName,
            hookspecs::kObscuredDoubleInternalDecrypt,
            obscuredDoubleDecryptTargetAddress,
            decrypt_mode,
            sizeof(decrypt_mode));

    if (std::strcmp(generate_key_mode, encrypt_mode) == 0 &&
        std::strcmp(generate_key_mode, set_encrypted_mode) == 0 &&
        std::strcmp(generate_key_mode, decrypt_mode) == 0) {
        CopyStringToBuffer(generate_key_mode, obscuredDoubleCryptoResolveMode, sizeof(obscuredDoubleCryptoResolveMode));
    } else {
        CopyStringToBuffer("mixed", obscuredDoubleCryptoResolveMode, sizeof(obscuredDoubleCryptoResolveMode));
    }

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kBattlerIsPlayerSelf,
                               reinterpret_cast<void *>(BattlerStoredDataIsPlayerSelf_Hook),
                               &old_BattlerStoredDataIsPlayerSelf,
                               battlerIsPlayerSelfHookInstalled,
                               battlerIsPlayerSelfAddress,
                               battlerIsPlayerSelfResolveMode,
                               sizeof(battlerIsPlayerSelfResolveMode));
    battlerIsPlayerSelfReady = battlerIsPlayerSelfHookInstalled;

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kGainSoulstone,
                               reinterpret_cast<void *>(GainSoulstone_Hook),
                               &old_GainSoulstone,
                               gainSoulstoneHookInstalled,
                               gainSoulstoneTargetAddress,
                               gainSoulstoneResolveMode,
                               sizeof(gainSoulstoneResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kGainGoldshard,
                               reinterpret_cast<void *>(GainGoldshard_Hook),
                               &old_GainGoldshard,
                               gainGoldshardHookInstalled,
                               gainGoldshardTargetAddress,
                               gainGoldshardResolveMode,
                               sizeof(gainGoldshardResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kBattlerPVal,
                               reinterpret_cast<void *>(BattlerPVal_Hook),
                               &old_BattlerPVal,
                               battlerPValHookInstalled,
                               battlerPValTargetAddress,
                               battlerPValResolveMode,
                               sizeof(battlerPValResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kBattlerCalcedPVal,
                               reinterpret_cast<void *>(BattlerCalcedPVal_Hook),
                               &old_BattlerCalcedPVal,
                               battlerCalcedPValHookInstalled,
                               battlerCalcedPValTargetAddress,
                               battlerCalcedPValResolveMode,
                               sizeof(battlerCalcedPValResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kBattlerAddPassive,
                               reinterpret_cast<void *>(BattlerAddPassive_Hook),
                               &old_BattlerAddPassive,
                               battlerAddPassiveHookInstalled,
                               battlerAddPassiveTargetAddress,
                               battlerAddPassiveResolveMode,
                               sizeof(battlerAddPassiveResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kBattlerAddTemp,
                               reinterpret_cast<void *>(BattlerAddTemp_Hook),
                               &old_BattlerAddTemp,
                               battlerAddTempHookInstalled,
                               battlerAddTempTargetAddress,
                               battlerAddTempResolveMode,
                               sizeof(battlerAddTempResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kBattlerDoAct,
                               reinterpret_cast<void *>(BattlerDoAct_Hook),
                               &old_BattlerDoAct,
                               battlerDoActHookInstalled,
                               battlerDoActTargetAddress,
                               battlerDoActResolveMode,
                               sizeof(battlerDoActResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kBattlerDoTurnStartCommonAction,
                               reinterpret_cast<void *>(BattlerDoTurnStartCommonAction_Hook),
                               &old_BattlerDoTurnStartCommonAction,
                               battlerTurnStartHookInstalled,
                               battlerTurnStartTargetAddress,
                               battlerTurnStartResolveMode,
                               sizeof(battlerTurnStartResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kSecureKeyValueSetGain,
                               reinterpret_cast<void *>(SecureKeyValueSetGain_Hook),
                               &old_SecureKeyValueSetGain,
                               secureKeyValueSetGainHookInstalled,
                               secureKeyValueSetGainTargetAddress,
                               secureKeyValueSetGainResolveMode,
                               sizeof(secureKeyValueSetGainResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kSecureKeyValueSetLose,
                               reinterpret_cast<void *>(SecureKeyValueSetLose_Hook),
                               &old_SecureKeyValueSetLose,
                               secureKeyValueSetLoseHookInstalled,
                               secureKeyValueSetLoseTargetAddress,
                               secureKeyValueSetLoseResolveMode,
                               sizeof(secureKeyValueSetLoseResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kSecureKeyValueSetSet,
                               reinterpret_cast<void *>(SecureKeyValueSetSet_Hook),
                               &old_SecureKeyValueSetSet,
                               secureKeyValueSetSetHookInstalled,
                               secureKeyValueSetSetTargetAddress,
                               secureKeyValueSetSetResolveMode,
                               sizeof(secureKeyValueSetSetResolveMode));

    mod::resolver::InstallHook(kGameLibraryName,
                               hookspecs::kSecureKeyValueSetGet,
                               reinterpret_cast<void *>(SecureKeyValueSetGet_Hook),
                               &old_SecureKeyValueSetGet,
                               secureKeyValueSetGetHookInstalled,
                               secureKeyValueSetGetTargetAddress,
                               secureKeyValueSetGetResolveMode,
                               sizeof(secureKeyValueSetGetResolveMode));

    RefreshResolverDebugState();
}

void *overlay_bootstrap_thread(void *) {
    LOGI("Overlay bootstrap watcher started");

    while (!g_OverlayBootstrapCompleted.load()) {
        if (javaVM == nullptr) {
            usleep(250000);
            continue;
        }

        mod::jvm::ScopedEnvAttachment scoped_env;
        if (!scoped_env.Attach(javaVM) || scoped_env.env == nullptr) {
            usleep(250000);
            continue;
        }

        jobject activity = mod::jvm::FindBestCurrentActivity(scoped_env.env);
        if (activity != nullptr) {
            const bool success = TryBootstrapOverlay(activity, "auto");
            scoped_env.env->DeleteLocalRef(activity);
            if (success) {
                break;
            }
        }

        usleep(250000);
    }

    return nullptr;
}

extern "C" void StartOverlayBootstrapThread() {
    bool expected = false;
    if (!g_OverlayBootstrapThreadStarted.compare_exchange_strong(expected, true)) {
        return;
    }

    pthread_t bootstrap_ptid;
    pthread_create(&bootstrap_ptid, nullptr, overlay_bootstrap_thread, nullptr);
}

extern "C" void StartNativeRuntimeThreads() {
    bool expected = false;
    if (!g_NativeRuntimeStarted.compare_exchange_strong(expected, true)) {
        return;
    }

    pthread_t hook_ptid;
    pthread_create(&hook_ptid, nullptr, hack_thread, nullptr);
    StartOverlayBootstrapThread();
}

void *hack_thread(void *) {
    LOGI(OBFUSCATE("pthread created"));

    if (!g_UseOverlayRenderer) {
        auto egl = dlopen("libEGL.so", RTLD_NOW);
        if (egl != nullptr) {
            auto swap = dlsym(egl, "eglSwapBuffers");
            if (swap != nullptr) {
                hook(swap, reinterpret_cast<void *>(HookedEglSwapBuffers), reinterpret_cast<void **>(&old_eglSwapBuffers));
                LOGI(OBFUSCATE("eglSwapBuffers hooked at %p"), swap);
            } else {
                LOGE(OBFUSCATE("eglSwapBuffers symbol not found"));
            }
        } else {
            LOGE(OBFUSCATE("libEGL.so not found"));
        }
    } else {
        LOGI(OBFUSCATE("Overlay renderer enabled, skipping eglSwapBuffers hook"));
    }

    InstallGameHooks();
    return nullptr;
}

void ForcePopulateCalcStatsImpl(void* battler) {
    // SAFETY: g_LastPValMethod must be non-null before driving PVal queries ourselves.
    // IL2CPP dereferences the method pointer internally; passing nullptr causes SIGSEGV.
    // The pointer is captured the first time BattlerPVal_Hook fires naturally in battle.
    if (battler == nullptr || old_BattlerPVal == nullptr) return;
    if (!EnsureIl2CppApiReady()) return;
    if (g_LastPValMethod == nullptr) return;  // Not safe to call yet – no real method ptr captured.

    const char* keys[] = {
        "MainParam:1", "MainParam:2", "MainParam:3", "MainParam:4",
        "CritRate", "CriticalRate", "CritDmg", "CritDamage",
        "PhysicalArmor", "PArmor", "MagicArmor", "MArmor"
    };

    for (const char* key_cstr : keys) {
        MonoString *managed_key = il2cpp_string_new != nullptr ? il2cpp_string_new(key_cstr) : nullptr;
        if (managed_key) {
            double value = old_BattlerPVal(battler, managed_key, g_LastPValMethod);
            RememberObservedStatValue(key_cstr, value, false);
        }
    }
}

}  // namespace

#include "Menu.h"

extern "C" JNIEXPORT void JNICALL
Java_com_unity3d_player_UnityPlayerActivity_nativeImguiSetSurfaceSize(JNIEnv *env, jclass clazz, jint width, jint height) {
    UpdateSurfaceSize(width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_unity3d_player_UnityPlayerActivity_nativeOverlayBootstrap(JNIEnv *env, jclass clazz, jobject activity) {
    if (activity == nullptr) {
        LOGE(OBFUSCATE("nativeOverlayBootstrap called with null activity"));
        return;
    }
    if (!TryBootstrapOverlay(activity, "manual")) {
        LOGE(OBFUSCATE("nativeOverlayBootstrap failed"));
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_unity3d_player_UnityPlayerActivity_nativeImguiToggleMenu(JNIEnv *env, jclass clazz) {
    g_ShowMenu = !g_ShowMenu;
}

extern "C" JNIEXPORT void JNICALL
Java_com_unity3d_player_UnityPlayerActivity_nativeImguiTouch(JNIEnv *env, jclass clazz, jint action, jfloat pos_x, jfloat pos_y) {
    if (action == 0) {
        ForwardTouch(true, pos_x, pos_y);
    } else if (action == 1) {
        ForwardTouch(false, pos_x, pos_y);
    } else {
        ForwardTouch(ImGui::GetCurrentContext() ? ImGui::GetIO().MouseDown[0] : false, pos_x, pos_y);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_mycompany_application_GLES3JNIView_init(JNIEnv *env, jclass clazz) {
    EnsureImGuiInitialized();
}

extern "C" JNIEXPORT void JNICALL
Java_com_mycompany_application_GLES3JNIView_resize(JNIEnv *env, jobject obj, jint width, jint height) {
    UpdateSurfaceSize(width, height);
    glViewport(0, 0, width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_mycompany_application_GLES3JNIView_step(JNIEnv *env, jobject obj) {
    RenderImGuiFrame(true);
}

extern "C" JNIEXPORT void JNICALL
Java_com_mycompany_application_GLES3JNIView_imgui_1Shutdown(JNIEnv *env, jobject obj) {
    ShutdownImGui();
}

extern "C" JNIEXPORT void JNICALL
Java_com_mycompany_application_GLES3JNIView_MotionEventClick(JNIEnv *env, jobject obj, jboolean down, jfloat pos_x, jfloat pos_y) {
    ForwardTouch(down == JNI_TRUE, pos_x, pos_y);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_mycompany_application_GLES3JNIView_getWindowRect(JNIEnv *env, jobject thiz) {
    const auto rect = GetWindowRectString();
    return env->NewStringUTF(rect.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_unity3d_player_GLES3JNIView_init(JNIEnv *env, jclass clazz) {
    Java_com_mycompany_application_GLES3JNIView_init(env, clazz);
}

extern "C" JNIEXPORT void JNICALL
Java_com_unity3d_player_GLES3JNIView_resize(JNIEnv *env, jobject obj, jint width, jint height) {
    Java_com_mycompany_application_GLES3JNIView_resize(env, obj, width, height);
}

extern "C" JNIEXPORT void JNICALL
Java_com_unity3d_player_GLES3JNIView_step(JNIEnv *env, jobject obj) {
    Java_com_mycompany_application_GLES3JNIView_step(env, obj);
}

extern "C" JNIEXPORT void JNICALL
Java_com_unity3d_player_GLES3JNIView_imgui_1Shutdown(JNIEnv *env, jobject obj) {
    Java_com_mycompany_application_GLES3JNIView_imgui_1Shutdown(env, obj);
}

extern "C" JNIEXPORT void JNICALL
Java_com_unity3d_player_GLES3JNIView_MotionEventClick(JNIEnv *env, jobject obj, jboolean down, jfloat pos_x, jfloat pos_y) {
    Java_com_mycompany_application_GLES3JNIView_MotionEventClick(env, obj, down, pos_x, pos_y);
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_unity3d_player_GLES3JNIView_getWindowRect(JNIEnv *env, jobject thiz) {
    return Java_com_mycompany_application_GLES3JNIView_getWindowRect(env, thiz);
}
