#pragma once

#define LIB OBFUSCATE("libil2cpp.so")
#define DHOOK(offseet, ptr, orig) DobbyHook((void*)getAbsoluteAddress("libil2cpp.so", offseet), (void*)ptr, (void**)&orig)
#define gAA getAbsoluteAddress

int glHeight;
int glWidth;
static int Hell = 1;

constexpr int kStatSlotCount = 4;
constexpr int kStatKeyBufferSize = 96;
constexpr int kObservedStatCapacity = 640;
constexpr int kCalcSkillSlotCount = 5;

bool soulstoneMultiplierEnabled = false;
int soulstoneMultiplierValue = 5;
bool goldshardMultiplierEnabled = false;
int goldshardMultiplierValue = 5;
float statBonusValues[kStatSlotCount] = {0.f, 0.f, 0.f, 0.f};
char statBonusKeys[kStatSlotCount][kStatKeyBufferSize] = {"", "", "", ""};
int selectedObservedStatIndices[kStatSlotCount] = {-1, -1, -1, -1};
char observedStatKeys[kObservedStatCapacity][kStatKeyBufferSize] = {{0}};
double observedStatSourceVal[kObservedStatCapacity] = {0.0};
double observedStatPVal[kObservedStatCapacity] = {0.0};
double observedStatCalcedVal[kObservedStatCapacity] = {0.0};
bool observedStatHasSourceVal[kObservedStatCapacity] = {false};
bool observedStatHasPVal[kObservedStatCapacity] = {false};
bool observedStatHasCalcedVal[kObservedStatCapacity] = {false};
int observedStatKeyCount = 0;
bool showDebugStats = true;
bool statShowCoreOnly = true;
bool statShowNonZeroOnly = false;
char statFilterText[kStatKeyBufferSize] = "";
int activeStatPickerSlot = -1;
int activeTextEditorSlot = -2;
char activeTextEditorTitle[64] = "Edit Text";
char lastApplyStatus[160] = "Belum ada apply stat.";
char lastAppliedStatKey[kStatKeyBufferSize] = "";
int lastAppliedStatDelta = 0;
uint64_t statApplyRequestCount = 0;
uint64_t statApplySuccessCount = 0;
uint64_t statApplyFailureCount = 0;

bool gainSoulstoneHookInstalled = false;
bool gainGoldshardHookInstalled = false;
bool userDataServiceGainIapHookInstalled = false;
bool safeCurrencyHookInstalled = false;
bool safeNumberHookInstalled = false;
bool obscuredDoubleDecryptHookInstalled = false;
bool battlerPValHookInstalled = false;
bool battlerCalcedPValHookInstalled = false;
bool battlerAddPassiveHookInstalled = false;
bool battlerAddTempHookInstalled = false;
bool battlerIsPlayerSelfHookInstalled = false;
bool battlerDoActHookInstalled = false;
bool battlerTurnStartHookInstalled = false;
bool secureKeyValueSetGetHookInstalled = false;
bool secureKeyValueSetSetHookInstalled = false;
bool secureKeyValueSetGainHookInstalled = false;
bool secureKeyValueSetLoseHookInstalled = false;
bool battlerIsPlayerSelfReady = false;
bool resolverVersionGuardMatched = false;
bool resolverFingerprintReady = false;

uint64_t gainSoulstoneHitCount = 0;
uint64_t gainGoldshardHitCount = 0;
uint64_t userDataServiceGainIapHitCount = 0;
uint64_t safeCurrencyGainHitCount = 0;
uint64_t safeNumberGainHitCount = 0;
uint64_t obscuredDoubleDecryptHitCount = 0;
uint64_t obscuredDoubleDecryptModifiedCount = 0;
uint64_t battlerPValHitCount = 0;
uint64_t battlerPValModifiedCount = 0;
uint64_t battlerCalcedPValHitCount = 0;
uint64_t battlerCalcedPValModifiedCount = 0;
uint64_t battlerAddPassiveHitCount = 0;
uint64_t battlerAddTempHitCount = 0;
uint64_t battlerIsPlayerSelfHitCount = 0;
uint64_t battlerDoActHitCount = 0;
uint64_t battlerTurnStartHitCount = 0;
uint64_t secureKeyValueSetGetHitCount = 0;
uint64_t secureKeyValueSetSetHitCount = 0;
uint64_t secureKeyValueSetGainHitCount = 0;
uint64_t secureKeyValueSetLoseHitCount = 0;
uint64_t resolverByNameCount = 0;
uint64_t resolverFallbackCount = 0;
uint64_t resolverBlockedFallbackCount = 0;

uint64_t gainSoulstoneTargetAddress = 0;
uint64_t gainGoldshardTargetAddress = 0;
uint64_t userDataServiceGainIapTargetAddress = 0;
uint64_t safeCurrencyTargetAddress = 0;
uint64_t safeNumberTargetAddress = 0;
uint64_t obscuredDoubleDecryptTargetAddress = 0;
uint64_t battlerPValTargetAddress = 0;
uint64_t battlerCalcedPValTargetAddress = 0;
uint64_t battlerAddPassiveTargetAddress = 0;
uint64_t battlerAddTempTargetAddress = 0;
uint64_t battlerDoActTargetAddress = 0;
uint64_t battlerTurnStartTargetAddress = 0;
uint64_t secureKeyValueSetGetTargetAddress = 0;
uint64_t secureKeyValueSetSetTargetAddress = 0;
uint64_t secureKeyValueSetGainTargetAddress = 0;
uint64_t secureKeyValueSetLoseTargetAddress = 0;
uint64_t battlerIsPlayerSelfAddress = 0;

double lastSoulstoneOriginalValue = 0.0;
double lastSoulstoneModifiedValue = 0.0;
double lastGoldshardOriginalValue = 0.0;
double lastGoldshardModifiedValue = 0.0;
double lastDecryptOriginalValue = 0.0;
double lastDecryptModifiedValue = 0.0;
double lastPValOriginalValue = 0.0;
double lastPValModifiedValue = 0.0;
double lastCalcedPValOriginalValue = 0.0;
double lastCalcedPValModifiedValue = 0.0;

char lastPValKey[kStatKeyBufferSize] = "";
char lastCalcedPValKey[kStatKeyBufferSize] = "";
char lastAddPassiveKey[kStatKeyBufferSize] = "";
char lastAddTempKey[kStatKeyBufferSize] = "";
char lastSecureSetKey[kStatKeyBufferSize] = "";
char lastMatchedStatKey[kStatKeyBufferSize] = "";
char lastObservedStatKey[kStatKeyBufferSize] = "";
char lastSoulstoneReason[160] = "";
char lastGoldshardReason[160] = "";
int lastAddPassiveValue = 0;
int lastAddTempValue = 0;
double lastSecureSetValue = 0.0;
bool hasCapturedPlayerBattler = false;
char resolverFingerprintText[160] = "pending";
char gainSoulstoneResolveMode[32] = "pending";
char gainGoldshardResolveMode[32] = "pending";
char battlerPValResolveMode[32] = "pending";
char battlerCalcedPValResolveMode[32] = "pending";
char battlerAddPassiveResolveMode[32] = "pending";
char battlerAddTempResolveMode[32] = "pending";
char battlerIsPlayerSelfResolveMode[32] = "pending";
char battlerDoActResolveMode[32] = "pending";
char battlerTurnStartResolveMode[32] = "pending";
char secureKeyValueSetGetResolveMode[32] = "pending";
char secureKeyValueSetSetResolveMode[32] = "pending";
char secureKeyValueSetGainResolveMode[32] = "pending";
char secureKeyValueSetLoseResolveMode[32] = "pending";
char sealedNumberGetResolveMode[32] = "pending";
char obscuredDoubleCryptoResolveMode[32] = "pending";

ImVec4 AccentColor = ImVec4(0.31f, 0.77f, 0.97f, 1.0f);
ImVec4 SoftTextColor = ImVec4(0.87f, 0.92f, 0.97f, 1.0f);
ImFont *g_MenuRegularFont = nullptr;
ImFont *g_MenuBoldFont = nullptr;

enum BB2CalcElem {
    BB2CALC_ELEM_NONE = 0,
    BB2CALC_ELEM_FIRE,
    BB2CALC_ELEM_ICE,
    BB2CALC_ELEM_THUNDER,
    BB2CALC_ELEM_POISON,
    BB2CALC_ELEM_DARK,
    BB2CALC_ELEM_HOLY,
    BB2CALC_ELEM_COUNT
};

enum BB2CalcDmgType {
    BB2CALC_DMG_PHYS = 0,
    BB2CALC_DMG_MAGIC,
    BB2CALC_DMG_TRUE,
    BB2CALC_DMG_HEAL,
    BB2CALC_DMG_BUFF
};

enum BB2CalcScale {
    BB2CALC_SC_STR = 0,
    BB2CALC_SC_INT,
    BB2CALC_SC_DEX,
    BB2CALC_SC_PIE,
    BB2CALC_SC_STR_DEX,
    BB2CALC_SC_INT_PIE,
    BB2CALC_SC_FLAT
};

struct BB2AutoSkillSnapshot {
    bool active = false;
    bool selected = false;
    bool canUse = false;
    bool isReady = false;
    int scaling = BB2CALC_SC_FLAT;
    int dmgType = BB2CALC_DMG_BUFF;
    int element = BB2CALC_ELEM_NONE;
    float powerCoeff = 0.0f;
    float flatBonus = 0.0f;
    int hitCount = 1;
    bool ignoreArmor = false;
    float armorPierce = 0.0f;
    bool canCrit = true;
    int cooldown = 0;
    int remain = 0;
    double sourceStatValue = 0.0;
    char name[96] = "";
    char sourceStatLabel[32] = "";
    char damageTypes[128] = "";
    char cantUseReason[128] = "";
    char inferredFrom[128] = "";
};

struct BB2CalcResult {
    double sourceValue = 0.0;
    char sourceLabel[32] = "";
    float rawFromScaling = 0.0f;
    float rawFlat = 0.0f;
    float raw = 0.0f;
    float armorUsed = 0.0f;
    float armorMinClamp = 0.0f;
    float armorReduced = 0.0f;
    float afterArmor = 0.0f;
    float resistMultiplier = 1.0f;
    float floorElemMultiplier = 1.0f;
    float afterResist = 0.0f;
    float targetTakenMultiplier = 1.0f;
    float magicTakenMultiplier = 1.0f;
    float hitChance = 1.0f;
    float perHitFinal = 0.0f;
    float totalNormal = 0.0f;
    float totalCrit = 0.0f;
    float critRate = 0.0f;
    float critMultiplier = 1.5f;
    float expectedNonCritContribution = 0.0f;
    float expectedCritContribution = 0.0f;
    float expected = 0.0f;
    float shieldDmg = 0.0f;
    float hpDmg = 0.0f;
    float healTotal = 0.0f;
    bool isHeal = false;
    bool isBuff = false;
    bool weak = false;
    bool immune = false;
    bool shieldBreak = false;
    bool lethal = false;
    char breakdown[768] = "";
};

double calcEnemyCurrentHp = 1000.0;
double calcEnemyMaxHp = 1000.0;
double calcEnemyShield = 0.0;
double calcEnemyPArmor = 0.0;
double calcEnemyMArmor = 0.0;
double calcEnemyResist[BB2CALC_ELEM_COUNT] = {0.0};
double calcPlayerStatMult = 1.0;
double calcEnemyArmorMult = 1.0;
double calcTargetExpose = 0.0;
double calcTargetTakenMult = 1.0;
double calcMagicTakenMult = 1.0;
double calcHitChance = 1.0;
int calcFloorAmpElement = BB2CALC_ELEM_NONE;
double calcFloorElemAmp = 1.0;
bool calcEnemyCaptured = false;
char calcEnemySource[64] = "waiting";
char calcEnemyLastKey[kStatKeyBufferSize] = "";
uint64_t calcEnemySeenCount = 0;

BB2AutoSkillSnapshot calcSkillSnapshots[kCalcSkillSlotCount] = {};
BB2CalcResult calcResults[kCalcSkillSlotCount] = {};
