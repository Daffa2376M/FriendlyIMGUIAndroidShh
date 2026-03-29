#pragma once

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>

#include "Variable.h"
#include "GameHookSpecs.h"
#include "RuntimeMethodResolver.h"

static constexpr const char *kStatsBuildTag = "stats-catalog-r14-auto-calc-safe-dynamic";

static std::string NormalizeStatKeyText(const char *raw_key) {
    if (raw_key == nullptr) {
        return std::string();
    }

    std::string key(raw_key);
    std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return key;
}

static bool HasStatModifierSuffix(const std::string &key) {
    if (key.empty()) {
        return false;
    }

    switch (key.back()) {
        case '#':
        case '+':
        case '*':
        case '@':
        case '/':
        case '-':
        case '=':
            return true;
        default:
            return false;
    }
}

static std::string HumanizeStatModifierSuffix(char suffix) {
    switch (suffix) {
        case '#': return "Total";
        case '+': return "Flat Bonus";
        case '*': return "Percent Bonus";
        case '@': return "Passive Bonus";
        case '/': return "Rate";
        case '-': return "Penalty";
        case '=': return "Override";
        default: return "";
    }
}

static std::string UppercaseToken(const std::string &token) {
    std::string out = token;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return out;
}

static std::string HumanizeMainParamToken(const std::string &token) {
    if (token == "1") return "STR";
    if (token == "2") return "INT";
    if (token == "3") return "DEX";
    if (token == "4") return "PIE";
    if (token == "12") return "STR + INT";
    if (token == "13") return "STR + DEX";
    if (token == "14") return "STR + PIE";
    if (token == "23") return "INT + DEX";
    if (token == "24") return "INT + PIE";
    if (token == "34") return "DEX + PIE";
    if (token == "123") return "STR + INT + DEX";
    if (token == "124") return "STR + INT + PIE";
    if (token == "134") return "STR + DEX + PIE";
    if (token == "234") return "INT + DEX + PIE";
    if (token == "1234") return "All Main Stats";
    if (token == "all") return "All Main Stats";
    return "";
}

static std::string HumanizeFallbackKey(const std::string &key) {
    std::string humanized = key;
    for (char &c : humanized) {
        if (c == '_' || c == '-') {
            c = ' ';
        }
    }

    bool capitalize_next = true;
    for (char &c : humanized) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            capitalize_next = true;
            continue;
        }

        if (capitalize_next) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            capitalize_next = false;
        }
    }

    return humanized;
}

static std::string HumanizeStatBaseKey(const std::string &key) {
    if (key.empty()) {
        return "Unknown";
    }

    if (key.rfind("mainparam:", 0) == 0) {
        const std::string token = key.substr(std::string("mainparam:").size());
        const std::string mapped = HumanizeMainParamToken(token);
        return mapped.empty() ? ("Main Param " + token) : mapped;
    }

    if (key.rfind("mastery:param:", 0) == 0) {
        const std::string token = key.substr(std::string("mastery:param:").size());
        const std::string mapped = HumanizeMainParamToken(token);
        return mapped.empty() ? ("Mastery " + token) : ("Mastery " + mapped);
    }

    if (key.rfind("skillpower:paramtype:", 0) == 0) {
        const std::string token = key.substr(std::string("skillpower:paramtype:").size());
        const std::string mapped = HumanizeMainParamToken(token);
        return mapped.empty() ? ("Skill Power " + token) : ("Skill Power (" + mapped + ")");
    }

    if (key.rfind("critdmg:elem:", 0) == 0) {
        return "Crit Damage [Elem " + UppercaseToken(key.substr(std::string("critdmg:elem:").size())) + "]";
    }
    if (key == "critdmg" || key.find("critdamage") != std::string::npos || key.find("criticaldamage") != std::string::npos) return "Crit Damage";
    if (key == "critrate" || key == "criticalrate") return "Crit Rate";
    if (key == "abscrit") return "Absolute Crit";
    if (key == "avoidcrit") return "Avoid Crit";
    if (key == "maxhpstack") return "Max HP Stack";
    if (key == "maxhpshield") return "Max HP / Shield";
    if (key.rfind("ignorearmor:elem:", 0) == 0) {
        return "Ignore Armor [Elem " + UppercaseToken(key.substr(std::string("ignorearmor:elem:").size())) + "]";
    }
    if (key == "ignorearmor") return "Ignore Armor";
    if (key.rfind("resail:", 0) == 0) {
        return "Ailment Resist [" + HumanizeFallbackKey(key.substr(std::string("resail:").size())) + "]";
    }
    if (key == "resail") return "Ailment Resist";
    if (key == "accu" || key.find("accuracy") != std::string::npos || key.find("hit") != std::string::npos) return "Accuracy";
    if (key == "avoid" || key.find("evasion") != std::string::npos || key.find("dodge") != std::string::npos) return "Avoid";
    if (key == "learning") return "Learning";
    if (key == "greed") return "Greed";
    if (key == "capture") return "Capture";
    if (key == "scout") return "Scout";
    if (key == "luck" || key == "luk") return "Luck";
    if (key == "parmor") return "Physical Armor";
    if (key == "marmor") return "Magic Armor";
    if (key == "varmor" || key == "warmor") return "Armor";
    if (key == "parry") return "Parry";
    if (key == "solid") return "Solid";
    if (key == "effective") return "Effectiveness";
    if (key == "additionalaction") return "Additional Action";
    if (key.rfind("allattack:direct", 0) == 0) return "All Attack Direct";
    if (key.rfind("cooldown", 0) == 0) return "Cooldown";
    if (key.rfind("consumenum", 0) == 0) return "Consume Count";
    if (key.rfind("gainmainparam:", 0) == 0) {
        const std::string tail = key.substr(std::string("gainmainparam:").size());
        return "Gain Main Param [" + tail + "]";
    }
    if (key.find("maxhp") != std::string::npos || (key.find("hp") != std::string::npos && key.find("max") != std::string::npos)) return "Max HP";
    if (key.find("currenthp") != std::string::npos || key == "hp" || (key.find("hp") != std::string::npos && key.find("current") != std::string::npos)) return "Current HP";
    if (key.find("maxshield") != std::string::npos) return "Max Shield";
    if (key.find("shield") != std::string::npos) return "Shield";
    if (key == "str" || key.find("strength") != std::string::npos) return "STR";
    if (key == "int" || key.find("intelligence") != std::string::npos) return "INT";
    if (key == "dex" || key.find("dexterity") != std::string::npos) return "DEX";
    if (key == "pie" || key.find("piety") != std::string::npos) return "PIE";
    if (key.find("atk") != std::string::npos || key.find("attack") != std::string::npos) return "Attack";
    if (key.find("def") != std::string::npos || key.find("defense") != std::string::npos) return "Defense";
    if (key.find("spd") != std::string::npos || key.find("speed") != std::string::npos || key.find("agi") != std::string::npos) return "Speed / Agility";
    if (key.find("crit") != std::string::npos || key.find("cri") != std::string::npos) return "Critical";
    if (key.find("eva") != std::string::npos || key.find("avoid") != std::string::npos || key.find("dodge") != std::string::npos) return "Evasion";
    if (key.find("vit") != std::string::npos) return "Vitality";
    if (key.find("pow") != std::string::npos || key.find("power") != std::string::npos) return "Power";
    if (key.find("fire") != std::string::npos) return "Fire";
    if (key.find("ice") != std::string::npos || key.find("cold") != std::string::npos) return "Ice / Cold";
    if (key.find("thunder") != std::string::npos || key.find("lightning") != std::string::npos) return "Thunder / Lightning";
    if (key.find("poison") != std::string::npos) return "Poison";
    if (key.find("dark") != std::string::npos) return "Dark";
    if (key.find("light") != std::string::npos || key.find("holy") != std::string::npos) return "Light / Holy";
    if (key.find("heal") != std::string::npos || key.find("recovery") != std::string::npos) return "Recovery";
    if (key.find("exp") != std::string::npos) return "Experience";
    if (key.find("level") != std::string::npos) return "Level";

    return HumanizeFallbackKey(key);
}

static std::string HumanizeStatKey(const char *raw_key) {
    const std::string normalized = NormalizeStatKeyText(raw_key);
    if (normalized.empty()) {
        return "Unknown";
    }

    std::string key = normalized;
    std::string suffix_label;
    if (HasStatModifierSuffix(key)) {
        suffix_label = HumanizeStatModifierSuffix(key.back());
        key.pop_back();
    }

    std::string label = HumanizeStatBaseKey(key);
    if (!suffix_label.empty()) {
        label += " [" + suffix_label + "]";
    }

    return label;
}

static bool IsCorePlayerStatKey(const char *raw_key) {
    const std::string key = NormalizeStatKeyText(raw_key);
    if (key.empty()) return false;

    if (key.rfind("statejob", 0) == 0) return false;
    if (key.rfind("staterace", 0) == 0) return false;
    if (key.rfind("stateborn", 0) == 0) return false;
    if (key.rfind("stateenv", 0) == 0) return false;
    if (key.rfind("stateevent", 0) == 0) return false;
    if (key.rfind("keystonestate", 0) == 0) return false;
    if (key.rfind("skill:", 0) == 0) return false;
    if (key.rfind("armor:act_", 0) == 0) return false;
    if (key.rfind("armor:ska", 0) == 0) return false;
    if (key.rfind("gear:", 0) == 0) return false;
    if (key.rfind("killtarget", 0) == 0) return false;
    if (key.rfind("remove", 0) == 0) return false;
    if (key.rfind("shorten:", 0) == 0) return false;
    if (key.rfind("expire:", 0) == 0) return false;
    if (key.rfind("usetarget", 0) == 0) return false;
    if (key.rfind("progress", 0) == 0) return false;
    if (key.rfind("useskill", 0) == 0) return false;
    if (key.rfind("killwith:", 0) == 0) return false;
    if (key.rfind("kill", 0) == 0) return false;
    if (key.find("bonusstack") != std::string::npos) return false;
    if (key.find("act_") != std::string::npos) return false;

    return true;
}

static bool HasAnyObservedValue(int stat_index) {
    return observedStatHasSourceVal[stat_index] || observedStatHasPVal[stat_index] || observedStatHasCalcedVal[stat_index];
}

static double GetObservedDisplayValue(int stat_index) {
    if (observedStatHasCalcedVal[stat_index]) return observedStatCalcedVal[stat_index];
    if (observedStatHasPVal[stat_index]) return observedStatPVal[stat_index];
    if (observedStatHasSourceVal[stat_index]) return observedStatSourceVal[stat_index];
    return 0.0;
}

static bool MatchesObservedFilter(int stat_index) {
    const char *key = observedStatKeys[stat_index];
    if (key[0] == '\0') return false;
    if (statShowCoreOnly && !IsCorePlayerStatKey(key)) return false;
    if (statShowNonZeroOnly && HasAnyObservedValue(stat_index) && GetObservedDisplayValue(stat_index) == 0.0) return false;

    if (statFilterText[0] != '\0') {
        const std::string filter = NormalizeStatKeyText(statFilterText);
        const std::string raw = NormalizeStatKeyText(key);
        const std::string human = NormalizeStatKeyText(HumanizeStatKey(key).c_str());
        if (raw.find(filter) == std::string::npos && human.find(filter) == std::string::npos) {
            return false;
        }
    }

    return true;
}

static void SectionHeaderLite(const char *label) {
    ImGui::Spacing();
    if (g_MenuBoldFont != nullptr) {
        ImGui::PushFont(g_MenuBoldFont);
    }
    ImGui::TextColored(AccentColor, "%s", label);
    if (g_MenuBoldFont != nullptr) {
        ImGui::PopFont();
    }
    ImGui::Separator();
}

static void BeginCard(const char* id, float height = 0.0f) {
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 16.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
    ImVec4 bg = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
    bg.x = ImMin(1.0f, bg.x + 0.035f);
    bg.y = ImMin(1.0f, bg.y + 0.035f);
    bg.z = ImMin(1.0f, bg.z + 0.035f);
    bg.w = 0.55f;
    ImGui::PushStyleColor(ImGuiCol_ChildBg, bg);
    ImGui::BeginChild(id, ImVec2(0, height), true, ImGuiWindowFlags_AlwaysUseWindowPadding);
}

static void EndCard() {
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

static bool ToggleSwitch(const char *label, bool *value) {
    if (value == nullptr) {
        return false;
    }

    ImGui::PushID(label);
    const float height = ImGui::GetFrameHeight() * 0.92f;
    const float width = height * 1.82f;
    const float radius = height * 0.50f;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList *draw_list = ImGui::GetWindowDrawList();

    ImGui::InvisibleButton("##toggle", ImVec2(width, height));
    const bool hovered = ImGui::IsItemHovered();
    const bool pressed = ImGui::IsItemClicked();
    if (pressed) {
        *value = !*value;
    }

    float t = *value ? 1.0f : 0.0f;
    ImGuiContext &g = *GImGui;
    if (g.LastActiveId == ImGui::GetItemID()) {
        const float anim = ImSaturate(g.LastActiveIdTimer / 0.10f);
        t = *value ? anim : (1.0f - anim);
    }

    const ImVec4 bg_off = hovered ? ImVec4(0.18f, 0.24f, 0.31f, 1.0f) : ImVec4(0.12f, 0.16f, 0.22f, 1.0f);
    const ImVec4 bg_on = hovered ? ImVec4(0.22f, 0.64f, 0.92f, 1.0f) : ImVec4(0.18f, 0.56f, 0.86f, 1.0f);
    const ImU32 bg_col = ImGui::GetColorU32(ImLerp(bg_off, bg_on, t));
    draw_list->AddRectFilled(pos, ImVec2(pos.x + width, pos.y + height), bg_col, radius);
    draw_list->AddCircleFilled(ImVec2(pos.x + radius + t * (width - radius * 2.0f), pos.y + radius),
                               radius - 2.0f, IM_COL32(245, 250, 255, 255));

    ImGui::SameLine(0.0f, 14.0f);
    if (g_MenuBoldFont != nullptr) {
        ImGui::PushFont(g_MenuBoldFont);
    }
    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted(label);
    if (g_MenuBoldFont != nullptr) {
        ImGui::PopFont();
    }
    ImGui::PopID();
    return pressed;
}

static double FindObservedStatValue(const std::vector<std::string> &candidates) {
    for (const auto &candidate : candidates) {
        for (int i = 0; i < observedStatKeyCount; ++i) {
            if (observedStatKeys[i][0] == '\0') {
                continue;
            }
            const std::string key = NormalizeStatKeyText(observedStatKeys[i]);
            if (key == candidate) {
                return GetObservedDisplayValue(i);
            }
        }
    }

    for (const auto &candidate : candidates) {
        for (int i = 0; i < observedStatKeyCount; ++i) {
            if (observedStatKeys[i][0] == '\0') {
                continue;
            }
            const std::string key = NormalizeStatKeyText(observedStatKeys[i]);
            if (key.find(candidate) != std::string::npos) {
                return GetObservedDisplayValue(i);
            }
        }
    }

    return 0.0;
}

static double GetCalcPlayerScaleValue(int scaling) {
    const double str = FindObservedStatValue({"mainparam:1#", "mainparam:1"});
    const double intel = FindObservedStatValue({"mainparam:2#", "mainparam:2"});
    const double dex = FindObservedStatValue({"mainparam:3#", "mainparam:3"});
    const double pie = FindObservedStatValue({"mainparam:4#", "mainparam:4"});

    switch (scaling) {
        case BB2CALC_SC_STR: return str;
        case BB2CALC_SC_INT: return intel;
        case BB2CALC_SC_DEX: return dex;
        case BB2CALC_SC_PIE: return pie;
        case BB2CALC_SC_STR_DEX: return (str + dex) * 0.5;
        case BB2CALC_SC_INT_PIE: return (intel + pie) * 0.5;
        case BB2CALC_SC_FLAT: return 0.0;
        default: return 0.0;
    }
}

static double GetCalcPlayerCritRate() {
    const double value = FindObservedStatValue({"critrate#", "critrate", "criticalrate"});
    if (value <= 1.0) {
        return value;
    }
    return value / 100.0;
}

static double GetCalcPlayerCritDmg() {
    const double value = FindObservedStatValue({"critdmg#", "critdmg", "critdamage"});
    if (value <= 0.0) {
        return 1.5;
    }
    if (value <= 5.0) {
        return value;
    }
    return value / 100.0;
}

static const char *GetCalcScaleLabel(int scaling) {
    switch (scaling) {
        case BB2CALC_SC_STR: return "STR";
        case BB2CALC_SC_INT: return "INT";
        case BB2CALC_SC_DEX: return "DEX";
        case BB2CALC_SC_PIE: return "PIE";
        case BB2CALC_SC_STR_DEX: return "STR + DEX";
        case BB2CALC_SC_INT_PIE: return "INT + PIE";
        case BB2CALC_SC_FLAT: return "FLAT";
        default: return "UNKNOWN";
    }
}

static const char *GetCalcDmgTypeLabel(int dmg_type) {
    switch (dmg_type) {
        case BB2CALC_DMG_PHYS: return "Physical";
        case BB2CALC_DMG_MAGIC: return "Magic";
        case BB2CALC_DMG_TRUE: return "True";
        case BB2CALC_DMG_HEAL: return "Heal";
        case BB2CALC_DMG_BUFF: return "Buff";
        default: return "Unknown";
    }
}

static const char *GetCalcElementLabel(int element) {
    switch (element) {
        case BB2CALC_ELEM_FIRE: return "Fire";
        case BB2CALC_ELEM_ICE: return "Ice";
        case BB2CALC_ELEM_THUNDER: return "Thunder";
        case BB2CALC_ELEM_POISON: return "Poison";
        case BB2CALC_ELEM_DARK: return "Dark";
        case BB2CALC_ELEM_HOLY: return "Holy";
        case BB2CALC_ELEM_NONE:
        default: return "None";
    }
}

struct CalcResolverCache {
    using BattlerCurrentSkillFn = void *(*)(void *, void *);
    using BattlerUsableSkillsFn = void *(*)(void *, void *);
    using UsableSkillCurrentNameFn = MonoString *(*)(void *, void *, void *);
    using UsableSkillDamageTypesShortFn = MonoString *(*)(void *, void *, void *);
    using UsableSkillCanUseFn = bool (*)(void *, void *);
    using UsableSkillCantUseReasonFn = MonoString *(*)(void *, void *);
    using UsableSkillCurrentCooldownFn = int (*)(void *, void *, void *);
    using UsableSkillCurrentRemainFn = int (*)(void *, void *, bool, void *);
    using UsableSkillPValFn = double (*)(void *, MonoString *, void *);
    using UsableSkillUserPValFn = int (*)(void *, void *, MonoString *, int, void *);

    bool attempted = false;
    int retryCooldownFrames = 0;
    BattlerCurrentSkillFn currentSkill = nullptr;
    BattlerUsableSkillsFn usableSkills = nullptr;
    UsableSkillCurrentNameFn currentName = nullptr;
    UsableSkillDamageTypesShortFn damageTypesShort = nullptr;
    UsableSkillCanUseFn canUse = nullptr;
    UsableSkillCantUseReasonFn cantUseReason = nullptr;
    UsableSkillCurrentCooldownFn currentCooldown = nullptr;
    UsableSkillCurrentRemainFn currentRemain = nullptr;
    UsableSkillPValFn pVal = nullptr;
    UsableSkillUserPValFn userPVal = nullptr;
    bool hasAnyResolver() const {
        return currentSkill != nullptr || usableSkills != nullptr || currentName != nullptr ||
               damageTypesShort != nullptr || canUse != nullptr || cantUseReason != nullptr ||
               currentCooldown != nullptr || currentRemain != nullptr || pVal != nullptr || userPVal != nullptr;
    }
};

struct Il2CppListObjectLike {
    void *klass;
    void *monitor;
    void *_items;
    int32_t _size;
    int32_t _version;
    void *_syncRoot;
};

static Il2CppListObjectLike *GetBattlerBattleSkillsList(void *battler) {
    if (battler == nullptr) {
        return nullptr;
    }
    return reinterpret_cast<Il2CppListObjectLike *>(
            *reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(battler) + mod::offsets::kBattlerFieldBattleSkills));
}

static CalcResolverCache &GetCalcResolverCache() {
    static CalcResolverCache cache;
    return cache;
}

static MonoString *GetCachedManagedKey(const char *key) {
    static std::unordered_map<std::string, MonoString *> cache;
    if (key == nullptr || key[0] == '\0' || !EnsureIl2CppApiReady()) {
        return nullptr;
    }

    const std::string stable_key(key);
    const auto found = cache.find(stable_key);
    if (found != cache.end()) {
        return found->second;
    }

    MonoString *managed_key = il2cpp_string_new(key);
    if (managed_key == nullptr) {
        return nullptr;
    }
    cache.emplace(stable_key, managed_key);
    return managed_key;
}

static bool EnsureCalcRuntimeResolvers() {
    auto &cache = GetCalcResolverCache();

    // Fast path: already resolved and at least one resolver is available.
    if (cache.attempted && cache.hasAnyResolver()) {
        return true;
    }

    // Previously attempted but got nothing — enforce cooldown before retrying.
    if (cache.attempted && !cache.hasAnyResolver()) {
        if (cache.retryCooldownFrames > 0) {
            --cache.retryCooldownFrames;
            return false;
        }
        // Cooldown expired: fall through and retry below.
    }

    // Reset all pointers before (re-)attempting resolution.
    cache.currentSkill = nullptr;
    cache.usableSkills = nullptr;
    cache.currentName = nullptr;
    cache.damageTypesShort = nullptr;
    cache.canUse = nullptr;
    cache.cantUseReason = nullptr;
    cache.currentCooldown = nullptr;
    cache.currentRemain = nullptr;
    cache.pVal = nullptr;
    cache.userPVal = nullptr;
    cache.attempted = true;

    uint64_t ignored_address = 0;
    char ignored_mode[32] = "";
    cache.currentSkill = mod::resolver::ResolveFunction<CalcResolverCache::BattlerCurrentSkillFn>(
            kGameLibraryName, mod::hookspecs::kBattlerCurrentSkill, ignored_address, ignored_mode, sizeof(ignored_mode));
    cache.usableSkills = mod::resolver::ResolveFunction<CalcResolverCache::BattlerUsableSkillsFn>(
            kGameLibraryName, mod::hookspecs::kBattlerUsableSkills, ignored_address, ignored_mode, sizeof(ignored_mode));
    cache.currentName = mod::resolver::ResolveFunction<CalcResolverCache::UsableSkillCurrentNameFn>(
            kGameLibraryName, mod::hookspecs::kUsableSkillCurrentName, ignored_address, ignored_mode, sizeof(ignored_mode));
    cache.damageTypesShort = mod::resolver::ResolveFunction<CalcResolverCache::UsableSkillDamageTypesShortFn>(
            kGameLibraryName, mod::hookspecs::kUsableSkillDamageTypesShort, ignored_address, ignored_mode, sizeof(ignored_mode));
    cache.canUse = mod::resolver::ResolveFunction<CalcResolverCache::UsableSkillCanUseFn>(
            kGameLibraryName, mod::hookspecs::kUsableSkillCanUse, ignored_address, ignored_mode, sizeof(ignored_mode));
    cache.cantUseReason = mod::resolver::ResolveFunction<CalcResolverCache::UsableSkillCantUseReasonFn>(
            kGameLibraryName, mod::hookspecs::kUsableSkillCantUseReason, ignored_address, ignored_mode, sizeof(ignored_mode));
    cache.currentCooldown = mod::resolver::ResolveFunction<CalcResolverCache::UsableSkillCurrentCooldownFn>(
            kGameLibraryName, mod::hookspecs::kUsableSkillCurrentCooldown, ignored_address, ignored_mode, sizeof(ignored_mode));
    cache.currentRemain = mod::resolver::ResolveFunction<CalcResolverCache::UsableSkillCurrentRemainFn>(
            kGameLibraryName, mod::hookspecs::kUsableSkillCurrentRemain, ignored_address, ignored_mode, sizeof(ignored_mode));
    cache.pVal = mod::resolver::ResolveFunction<CalcResolverCache::UsableSkillPValFn>(
            kGameLibraryName, mod::hookspecs::kUsableSkillPVal, ignored_address, ignored_mode, sizeof(ignored_mode));
    cache.userPVal = mod::resolver::ResolveFunction<CalcResolverCache::UsableSkillUserPValFn>(
            kGameLibraryName, mod::hookspecs::kUsableSkillUserPVal, ignored_address, ignored_mode, sizeof(ignored_mode));

    // If still nothing resolved, throttle the next attempt to ~120 frames (~2 sec).
    if (!cache.hasAnyResolver()) {
        cache.retryCooldownFrames = 120;
    }
    return cache.hasAnyResolver();
}

static void *GetManagedArrayObjectAt(void *array_object, int index) {
    auto *header = reinterpret_cast<mod::types::Il2CppArrayHeader *>(array_object);
    if (header == nullptr || index < 0 || static_cast<uintptr_t>(index) >= header->max_length) {
        return nullptr;
    }
    auto **vector = reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(header) + sizeof(mod::types::Il2CppArrayHeader));
    return vector[index];
}

static double ReadUsableSkillPVal(void *skill, const char *key) {
    auto &cache = GetCalcResolverCache();
    if (skill == nullptr || key == nullptr || key[0] == '\0' || cache.pVal == nullptr) {
        return 0.0;
    }
    MonoString *managed_key = GetCachedManagedKey(key);
    if (managed_key == nullptr) {
        return 0.0;
    }
    return cache.pVal(skill, managed_key, nullptr);
}

static bool ShouldUseDynamicSkillIntValue(const char *key) {
    if (key == nullptr) {
        return false;
    }
    return std::strcmp(key, "Damage") == 0 ||
           std::strcmp(key, "Power") == 0 ||
           std::strcmp(key, "HitCount") == 0 ||
           std::strcmp(key, "AttackNum") == 0 ||
           std::strcmp(key, "Count") == 0 ||
           std::strcmp(key, "ActNum") == 0 ||
           std::strcmp(key, "Heal") == 0 ||
           std::strcmp(key, "HealHP") == 0;
}

static double ReadUsableSkillDynamicValue(void *skill, const char *key, int sentinel_value = -31337) {
    auto &cache = GetCalcResolverCache();
    if (skill == nullptr || key == nullptr || key[0] == '\0' || !ShouldUseDynamicSkillIntValue(key) ||
        cache.userPVal == nullptr || g_LastPlayerBattler == nullptr) {
        return ReadUsableSkillPVal(skill, key);
    }
    MonoString *managed_key = GetCachedManagedKey(key);
    if (managed_key == nullptr) {
        return ReadUsableSkillPVal(skill, key);
    }

    const int value = cache.userPVal(skill, g_LastPlayerBattler, managed_key, sentinel_value, nullptr);
    if (value != sentinel_value) {
        return static_cast<double>(value);
    }
    return ReadUsableSkillPVal(skill, key);
}

static bool StringContainsAny(const std::string &text, std::initializer_list<const char *> needles) {
    const std::string normalized = NormalizeStatKeyText(text.c_str());
    for (const char *needle : needles) {
        if (needle != nullptr && normalized.find(needle) != std::string::npos) {
            return true;
        }
    }
    return false;
}

static void InferAutoSkillSnapshotFromPVal(void *skill, BB2AutoSkillSnapshot &snapshot) {
    struct ScaleCandidate {
        const char *label;
        int scaling;
        const char *keys[4];
    };

    static const ScaleCandidate kScaleCandidates[] = {
            {"STR", BB2CALC_SC_STR, {"MainParam:1", "SkillPower:ParamType:1", "Power:STR", nullptr}},
            {"INT", BB2CALC_SC_INT, {"MainParam:2", "SkillPower:ParamType:2", "Power:INT", nullptr}},
            {"DEX", BB2CALC_SC_DEX, {"MainParam:3", "SkillPower:ParamType:3", "Power:DEX", nullptr}},
            {"PIE", BB2CALC_SC_PIE, {"MainParam:4", "SkillPower:ParamType:4", "Power:PIE", nullptr}},
            {"STR + DEX", BB2CALC_SC_STR_DEX, {"MainParam:13", "MainParam:31", nullptr, nullptr}},
            {"INT + PIE", BB2CALC_SC_INT_PIE, {"MainParam:24", "MainParam:42", nullptr, nullptr}},
    };

    double best_coeff = 0.0;
    std::snprintf(snapshot.sourceStatLabel, sizeof(snapshot.sourceStatLabel), "%s", "FLAT");
    snapshot.scaling = BB2CALC_SC_FLAT;
    snapshot.sourceStatValue = 0.0;

    for (const auto &candidate : kScaleCandidates) {
        for (const char *key : candidate.keys) {
            if (key == nullptr) {
                continue;
            }
            const double coeff = ReadUsableSkillPVal(skill, key);
            if (std::fabs(coeff) <= std::fabs(best_coeff)) {
                continue;
            }
            best_coeff = coeff;
            snapshot.scaling = candidate.scaling;
            std::snprintf(snapshot.sourceStatLabel, sizeof(snapshot.sourceStatLabel), "%s", candidate.label);
        }
    }

    snapshot.powerCoeff = static_cast<float>(best_coeff);
    snapshot.flatBonus = static_cast<float>(ReadUsableSkillDynamicValue(skill, "Damage"));
    if (std::fabs(snapshot.flatBonus) < 0.001f) {
        snapshot.flatBonus = static_cast<float>(ReadUsableSkillDynamicValue(skill, "Power"));
    }

    snapshot.hitCount = static_cast<int>(std::lround(std::max({
            1.0,
            ReadUsableSkillDynamicValue(skill, "HitCount"),
            ReadUsableSkillDynamicValue(skill, "AttackNum"),
            ReadUsableSkillDynamicValue(skill, "Count"),
            ReadUsableSkillDynamicValue(skill, "ActNum")
    })));

    snapshot.armorPierce = static_cast<float>(std::clamp(std::max({
            0.0,
            ReadUsableSkillPVal(skill, "ArmorPierce"),
            ReadUsableSkillPVal(skill, "IgnoreArmor"),
            ReadUsableSkillPVal(skill, "IgnoreArmor:Elem:P")
    }), 0.0, 1.0));
    snapshot.ignoreArmor = snapshot.armorPierce >= 0.99f || ReadUsableSkillPVal(skill, "IgnoreArmor") > 0.99;
    snapshot.canCrit = ReadUsableSkillPVal(skill, "CritDmg") > 0.0;

    const std::string name = snapshot.name;
    const std::string damage_text = snapshot.damageTypes;
    if (StringContainsAny(name, {"heal", "recovery", "recover", "cure"}) ||
        StringContainsAny(damage_text, {"heal", "recovery", "recover", "cure"}) ||
        ReadUsableSkillDynamicValue(skill, "Heal") > 0.0 ||
        ReadUsableSkillDynamicValue(skill, "HealHP") > 0.0) {
        snapshot.dmgType = BB2CALC_DMG_HEAL;
        snapshot.flatBonus = static_cast<float>(std::max({static_cast<double>(snapshot.flatBonus), ReadUsableSkillDynamicValue(skill, "Heal"), ReadUsableSkillDynamicValue(skill, "HealHP")}));
    } else if (best_coeff == 0.0 && std::fabs(snapshot.flatBonus) < 0.001f) {
        snapshot.dmgType = BB2CALC_DMG_BUFF;
    } else if (StringContainsAny(damage_text, {"magic", "spell", "dark", "holy", "fire", "ice", "thunder", "poison"})) {
        snapshot.dmgType = BB2CALC_DMG_MAGIC;
    } else if (StringContainsAny(damage_text, {"true"})) {
        snapshot.dmgType = BB2CALC_DMG_TRUE;
    } else {
        snapshot.dmgType = BB2CALC_DMG_PHYS;
    }

    if (StringContainsAny(name, {"fire"}) || StringContainsAny(damage_text, {"fire"})) snapshot.element = BB2CALC_ELEM_FIRE;
    else if (StringContainsAny(name, {"ice", "cold"}) || StringContainsAny(damage_text, {"ice", "cold"})) snapshot.element = BB2CALC_ELEM_ICE;
    else if (StringContainsAny(name, {"thunder", "lightning"}) || StringContainsAny(damage_text, {"thunder", "lightning"})) snapshot.element = BB2CALC_ELEM_THUNDER;
    else if (StringContainsAny(name, {"poison"}) || StringContainsAny(damage_text, {"poison"})) snapshot.element = BB2CALC_ELEM_POISON;
    else if (StringContainsAny(name, {"dark"}) || StringContainsAny(damage_text, {"dark"})) snapshot.element = BB2CALC_ELEM_DARK;
    else if (StringContainsAny(name, {"holy", "light"}) || StringContainsAny(damage_text, {"holy", "light"})) snapshot.element = BB2CALC_ELEM_HOLY;
    else snapshot.element = BB2CALC_ELEM_NONE;

    snapshot.sourceStatValue = GetCalcPlayerScaleValue(snapshot.scaling) * calcPlayerStatMult;
    std::snprintf(snapshot.inferredFrom, sizeof(snapshot.inferredFrom),
                  "%s | coeff %.2f | hits %d",
                  snapshot.sourceStatLabel,
                  snapshot.powerCoeff,
                  snapshot.hitCount);
}

static void RefreshAutoCalcSkillSnapshots() {
    static uint64_t last_refresh_ms = 0;
    const uint64_t now_ms = GetNowMs();
    if (now_ms - last_refresh_ms < 250) {
        return;
    }
    last_refresh_ms = now_ms;

    for (auto &snapshot : calcSkillSnapshots) {
        snapshot = {};
    }

    if (g_LastPlayerBattler == nullptr) {
        return;
    }
    const bool has_resolvers = EnsureCalcRuntimeResolvers();

    auto &cache = GetCalcResolverCache();
    void *selected_skill = cache.currentSkill != nullptr ? cache.currentSkill(g_LastPlayerBattler, nullptr) : nullptr;
    auto *skill_list = cache.usableSkills != nullptr
                       ? reinterpret_cast<Il2CppListObjectLike *>(cache.usableSkills(g_LastPlayerBattler, nullptr))
                       : nullptr;
    const bool usable_valid = skill_list != nullptr && skill_list->_items != nullptr && skill_list->_size > 0 && skill_list->_size <= 16;
    auto *battle_skill_list = GetBattlerBattleSkillsList(g_LastPlayerBattler);
    const bool battle_valid = battle_skill_list != nullptr && battle_skill_list->_items != nullptr &&
                              battle_skill_list->_size > 0 && battle_skill_list->_size <= 16;

    if ((!usable_valid || skill_list->_size < kCalcSkillSlotCount) && battle_valid) {
        skill_list = battle_skill_list;
    }

    if ((!has_resolvers && !battle_valid) ||
        skill_list == nullptr || skill_list->_items == nullptr || skill_list->_size <= 0 || skill_list->_size > 16) {
        return;
    }

    const int count = std::min(kCalcSkillSlotCount, skill_list->_size);
    for (int i = 0; i < count; ++i) {
        void *skill = GetManagedArrayObjectAt(skill_list->_items, i);
        if (skill == nullptr) {
            continue;
        }

        auto &snapshot = calcSkillSnapshots[i];
        snapshot.active = true;
        snapshot.selected = (skill == selected_skill);
        snapshot.canUse = cache.canUse != nullptr ? cache.canUse(skill, nullptr) : false;
        snapshot.cooldown = cache.currentCooldown != nullptr ? cache.currentCooldown(skill, g_LastPlayerBattler, nullptr) : 0;
        snapshot.remain = cache.currentRemain != nullptr ? cache.currentRemain(skill, g_LastPlayerBattler, false, nullptr) : 0;
        snapshot.isReady = cache.canUse != nullptr ? (snapshot.canUse && snapshot.cooldown <= 0) : (snapshot.cooldown <= 0);

        const std::string name = GetManagedString(cache.currentName != nullptr ? cache.currentName(skill, g_LastPlayerBattler, nullptr) : nullptr);
        const std::string damage_types = GetManagedString(cache.damageTypesShort != nullptr ? cache.damageTypesShort(skill, g_LastPlayerBattler, nullptr) : nullptr);
        const std::string cant_use_reason = GetManagedString(cache.cantUseReason != nullptr ? cache.cantUseReason(skill, nullptr) : nullptr);
        if (!name.empty()) {
            CopyStringToBuffer(name, snapshot.name, sizeof(snapshot.name));
        } else {
            std::snprintf(snapshot.name, sizeof(snapshot.name), "Skill %d", i + 1);
        }
        CopyStringToBuffer(damage_types, snapshot.damageTypes, sizeof(snapshot.damageTypes));
        CopyStringToBuffer(cant_use_reason, snapshot.cantUseReason, sizeof(snapshot.cantUseReason));

        InferAutoSkillSnapshotFromPVal(skill, snapshot);
        if (cache.pVal == nullptr) {
            snapshot.dmgType = StringContainsAny(name, {"heal", "recover", "recovery", "cure"}) ? BB2CALC_DMG_HEAL : BB2CALC_DMG_BUFF;
            std::snprintf(snapshot.inferredFrom, sizeof(snapshot.inferredFrom), "%s",
                          cache.currentName != nullptr ? "battle-skill fallback" : "raw battle-skill fallback");
        }
    }
}

static BB2CalcResult CalcDamagePreview(const BB2AutoSkillSnapshot &slot) {
    BB2CalcResult result{};
    result.isHeal = slot.dmgType == BB2CALC_DMG_HEAL;
    result.isBuff = slot.dmgType == BB2CALC_DMG_BUFF;
    const double critRate = std::clamp(GetCalcPlayerCritRate(), 0.0, 1.0);
    const double critDmg = std::max(1.0, GetCalcPlayerCritDmg());
    result.sourceValue = slot.sourceStatValue;
    std::snprintf(result.sourceLabel, sizeof(result.sourceLabel), "%s", slot.sourceStatLabel[0] ? slot.sourceStatLabel : GetCalcScaleLabel(slot.scaling));
    result.rawFromScaling = static_cast<float>(result.sourceValue * slot.powerCoeff);
    result.rawFlat = slot.flatBonus;
    result.raw = result.rawFromScaling + result.rawFlat;

    float afterArmor = result.raw;
    if (slot.dmgType != BB2CALC_DMG_TRUE && slot.dmgType != BB2CALC_DMG_HEAL && slot.dmgType != BB2CALC_DMG_BUFF && !slot.ignoreArmor) {
        double armor = (slot.dmgType == BB2CALC_DMG_MAGIC) ? calcEnemyMArmor : calcEnemyPArmor;
        armor *= calcEnemyArmorMult;
        armor *= (1.0 - std::clamp(calcTargetExpose, 0.0, 1.0));
        armor *= (1.0 - std::clamp(static_cast<double>(slot.armorPierce), 0.0, 1.0));
        result.armorUsed = static_cast<float>(std::max(0.0, armor));
        const double minimum = static_cast<double>(result.raw) * 0.05;
        result.armorMinClamp = static_cast<float>(minimum);
        afterArmor = static_cast<float>(std::max(static_cast<double>(result.raw) - armor, minimum));
    }
    result.afterArmor = afterArmor;
    result.armorReduced = std::max(0.0f, result.raw - result.afterArmor);

    float afterResist = afterArmor;
    if (slot.element != BB2CALC_ELEM_NONE && slot.element < BB2CALC_ELEM_COUNT) {
        const double resist = calcEnemyResist[slot.element];
        result.resistMultiplier = static_cast<float>(std::max(0.0, 1.0 - resist));
        if (calcFloorAmpElement == slot.element) {
            result.floorElemMultiplier = static_cast<float>(std::max(0.0, calcFloorElemAmp));
        }
        if (resist >= 1.0) {
            result.immune = true;
            afterResist = 0.0f;
        } else {
            result.weak = resist < -0.01;
            afterResist = static_cast<float>(static_cast<double>(afterArmor) * result.resistMultiplier * result.floorElemMultiplier);
        }
    }
    result.afterResist = afterResist;
    result.targetTakenMultiplier = static_cast<float>(std::max(0.0, calcTargetTakenMult));
    result.magicTakenMultiplier = slot.dmgType == BB2CALC_DMG_MAGIC ? static_cast<float>(std::max(0.0, calcMagicTakenMult)) : 1.0f;
    result.hitChance = static_cast<float>(std::clamp(calcHitChance, 0.0, 1.0));
    result.perHitFinal = result.afterResist * result.targetTakenMultiplier * result.magicTakenMultiplier;

    if (result.isHeal) {
        result.totalNormal = result.perHitFinal * std::max(slot.hitCount, 1);
        result.healTotal = result.totalNormal;
        result.expected = result.healTotal;
    } else if (result.isBuff) {
        result.expected = 0.0f;
    } else {
        result.totalNormal = result.perHitFinal * std::max(slot.hitCount, 1);
        result.totalCrit = slot.canCrit ? static_cast<float>(result.totalNormal * critDmg) : result.totalNormal;
        result.critRate = slot.canCrit ? static_cast<float>(critRate) : 0.0f;
        result.critMultiplier = slot.canCrit ? static_cast<float>(critDmg) : 1.0f;
        result.expectedNonCritContribution = static_cast<float>((1.0 - result.critRate) * result.totalNormal * result.hitChance);
        result.expectedCritContribution = static_cast<float>(result.critRate * result.totalCrit * result.hitChance);
        result.expected = result.expectedNonCritContribution + result.expectedCritContribution;

        float remaining = result.expected;
        const float shield = static_cast<float>(std::max(0.0, calcEnemyShield));
        if (shield > 0.0f) {
            result.shieldDmg = std::min(remaining, shield);
            remaining -= result.shieldDmg;
            result.shieldBreak = result.shieldDmg >= shield;
        }
        result.hpDmg = std::max(0.0f, remaining);
        result.lethal = result.hpDmg >= static_cast<float>(std::max(0.0, calcEnemyCurrentHp));
    }

    if (result.isHeal) {
        std::snprintf(result.breakdown,
                      sizeof(result.breakdown),
                      "1) Source %s = %.0f\n"
                      "2) Heal raw = (%.0f x %.2f) + %.0f = %.0f\n"
                      "3) Hits = %d\n"
                      "4) Total heal = %.0f",
                      result.sourceLabel,
                      result.sourceValue,
                      result.sourceValue,
                      slot.powerCoeff,
                      result.rawFlat,
                      result.raw,
                      std::max(slot.hitCount, 1),
                      result.healTotal);
    } else if (result.isBuff) {
        std::snprintf(result.breakdown,
                      sizeof(result.breakdown),
                      "Skill ini kebaca sebagai buff / utility.\n"
                      "Damage type text: %s\n"
                      "Inferensi: %s",
                      slot.damageTypes[0] ? slot.damageTypes : "-",
                      slot.inferredFrom[0] ? slot.inferredFrom : "-");
    } else {
        std::snprintf(
                result.breakdown,
                sizeof(result.breakdown),
                "1) Source %s = %.0f\n"
                "2) Raw = (%.0f x %.2f) + %.0f = %.0f\n"
                "3) Armor eff = %.0f  | clamp min = %.0f  | after armor = %.0f\n"
                "4) Resist x%.2f  | floor amp x%.2f  | after resist = %.0f\n"
                "5) Taken mult x%.2f  | magic mult x%.2f  | per hit = %.0f\n"
                "6) Hits = %d  | total normal = %.0f\n"
                "7) Crit rate %.0f%%  | crit mult x%.2f\n"
                "8) Hit chance %.0f%%  | EV(non-crit %.0f + crit %.0f) = %.0f\n"
                "9) Shield %.0f  | HP %.0f%s%s%s",
                result.sourceLabel,
                result.sourceValue,
                result.sourceValue,
                slot.powerCoeff,
                result.rawFlat,
                result.raw,
                result.armorUsed,
                result.armorMinClamp,
                result.afterArmor,
                result.resistMultiplier,
                result.floorElemMultiplier,
                result.afterResist,
                result.targetTakenMultiplier,
                result.magicTakenMultiplier,
                result.perHitFinal,
                std::max(slot.hitCount, 1),
                result.totalNormal,
                result.critRate * 100.0f,
                result.critMultiplier,
                result.hitChance * 100.0f,
                result.expectedNonCritContribution,
                result.expectedCritContribution,
                result.expected,
                result.shieldDmg,
                result.hpDmg,
                result.shieldBreak ? "  [BREAK]" : "",
                result.lethal ? "  [LETHAL]" : "",
                result.immune ? "  [IMMUNE]" : (result.weak ? "  [WEAK]" : ""));
    }
    return result;
}

static void RenderCalcTab() {
    SectionHeaderLite("Damage Preview");
    RefreshCalcEnemyFromMemory();
    RefreshAutoCalcSkillSnapshots();

    if (!calcEnemyCaptured) {
        ImGui::TextDisabled("Musuh belum terdeteksi. Silakan masuk battle / target enemy.");
    } else {
        ImGui::Text("Target HP: %.0f / %.0f | Shield: %.0f", calcEnemyCurrentHp, calcEnemyMaxHp, calcEnemyShield);
    }
    ImGui::Spacing();

    for (int i = 0; i < kCalcSkillSlotCount; ++i) {
        ImGui::PushID(i);
        auto &snapshot = calcSkillSnapshots[i];
        if (!snapshot.active) {
            ImGui::PopID();
            continue;
        }

        calcResults[i] = CalcDamagePreview(snapshot);
        const auto &r = calcResults[i];

        char card_id[32];
        std::snprintf(card_id, sizeof(card_id), "SkillCard_%d", i);
        // Tinggi fix 105 biar kartu seragam
        BeginCard(card_id, 105.0f);

        if (g_MenuBoldFont) ImGui::PushFont(g_MenuBoldFont);
        if (snapshot.isReady) {
            ImGui::TextColored(ImVec4(0.2f, 0.85f, 0.2f, 1.0f), "[RDY]");
        } else {
            ImGui::TextColored(ImVec4(0.85f, 0.2f, 0.2f, 1.0f), "[CD:%d]", snapshot.cooldown);
        }
        ImGui::SameLine();
        ImGui::TextColored(snapshot.selected ? AccentColor : ImVec4(1,1,1,1), "%s", snapshot.name[0] ? snapshot.name : "(Unknown)");
        if (g_MenuBoldFont) ImGui::PopFont();

        if (!snapshot.canUse && snapshot.cantUseReason[0]) {
            ImGui::TextColored(ImVec4(0.95f, 0.45f, 0.45f, 1.0f), "%s", snapshot.cantUseReason);
        } else {
            ImGui::Spacing();
        }

        if (r.isHeal) {
            ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.5f, 1.0f), "Heal Estimate: %.0f HP", r.healTotal);
        } else if (r.isBuff) {
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Utility / Buff Skill");
        } else {
            ImGui::Text("Total Estimate: "); ImGui::SameLine();
            if (g_MenuBoldFont) ImGui::PushFont(g_MenuBoldFont);
            ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.2f, 1.0f), "%.0f DMG", r.expected);
            if (g_MenuBoldFont) ImGui::PopFont();
            
            if (r.shieldBreak) {
                ImGui::SameLine(); ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.2f, 1.0f), "[BREAK]");
            }
            if (r.lethal) {
                ImGui::SameLine(); ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[LETHAL]");
            }
            if (r.immune) {
                ImGui::SameLine(); ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[IMMUNE]");
            } else if (r.weak) {
                ImGui::SameLine(); ImGui::TextColored(ImVec4(0.8f, 0.4f, 1.0f, 1.0f), "[WEAK]");
            }
            
            ImGui::TextDisabled("HP: %.0f | Shield: %.0f | Hits: %d", r.hpDmg, r.shieldDmg, std::max(snapshot.hitCount, 1));
        }

        EndCard();
        ImGui::Spacing();
        ImGui::PopID();
    }
}

static const std::vector<int> &GatherSortedObservedStatIndices() {
    static std::vector<int> cached_indices;
    static int cached_count = -1;
    static bool cached_core_only = false;
    static bool cached_non_zero_only = false;
    static std::string cached_filter;

    const std::string current_filter = NormalizeStatKeyText(statFilterText);
    const bool needs_rebuild = cached_count != observedStatKeyCount ||
                               cached_core_only != statShowCoreOnly ||
                               cached_non_zero_only != statShowNonZeroOnly ||
                               cached_filter != current_filter;
    if (!needs_rebuild) {
        return cached_indices;
    }

    cached_indices.clear();
    cached_indices.reserve(observedStatKeyCount);

    for (int stat_index = 0; stat_index < observedStatKeyCount; ++stat_index) {
        if (MatchesObservedFilter(stat_index)) {
            cached_indices.push_back(stat_index);
        }
    }

    std::sort(cached_indices.begin(), cached_indices.end(), [](int left, int right) {
        const std::string left_label = NormalizeStatKeyText(HumanizeStatKey(observedStatKeys[left]).c_str());
        const std::string right_label = NormalizeStatKeyText(HumanizeStatKey(observedStatKeys[right]).c_str());
        if (left_label != right_label) {
            return left_label < right_label;
        }

        const std::string left_key = NormalizeStatKeyText(observedStatKeys[left]);
        const std::string right_key = NormalizeStatKeyText(observedStatKeys[right]);
        if (left_key != right_key) {
            return left_key < right_key;
        }

        return left < right;
    });

    cached_count = observedStatKeyCount;
    cached_core_only = statShowCoreOnly;
    cached_non_zero_only = statShowNonZeroOnly;
    cached_filter = current_filter;
    return cached_indices;
}

static char *GetActiveTextEditorBuffer() {
    if (activeTextEditorSlot == -1) {
        return statFilterText;
    }
    if (activeTextEditorSlot >= 0 && activeTextEditorSlot < kStatSlotCount) {
        return statBonusKeys[activeTextEditorSlot];
    }
    return nullptr;
}

static size_t GetActiveTextEditorBufferSize() {
    if (activeTextEditorSlot == -1) {
        return IM_ARRAYSIZE(statFilterText);
    }
    if (activeTextEditorSlot >= 0 && activeTextEditorSlot < kStatSlotCount) {
        return IM_ARRAYSIZE(statBonusKeys[activeTextEditorSlot]);
    }
    return 0;
}

static void OpenTextEditorPopup(int target_slot, const char *title) {
    activeTextEditorSlot = target_slot;
    std::snprintf(activeTextEditorTitle, sizeof(activeTextEditorTitle), "%s", title != nullptr ? title : "Edit Text");
    ImGui::OpenPopup("Touch Text Editor");
}

static void AppendTextEditorCharacter(char character) {
    char *buffer = GetActiveTextEditorBuffer();
    const size_t buffer_size = GetActiveTextEditorBufferSize();
    if (buffer == nullptr || buffer_size < 2) {
        return;
    }

    const size_t length = std::strlen(buffer);
    if (length + 1 >= buffer_size) {
        return;
    }

    buffer[length] = character;
    buffer[length + 1] = '\0';
}

static void BackspaceTextEditorCharacter() {
    char *buffer = GetActiveTextEditorBuffer();
    if (buffer == nullptr) {
        return;
    }

    const size_t length = std::strlen(buffer);
    if (length == 0) {
        return;
    }

    buffer[length - 1] = '\0';
}

static void ClearTextEditorBuffer() {
    char *buffer = GetActiveTextEditorBuffer();
    if (buffer == nullptr) {
        return;
    }

    buffer[0] = '\0';
}

static void RenderTouchTextEditorPopup() {
    g_TextEditorPopupOpen = ImGui::IsPopupOpen("Touch Text Editor");
    if (ImGui::BeginPopupModal("Touch Text Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        g_TextEditorPopupOpen = true;
        char *buffer = GetActiveTextEditorBuffer();
        const char *preview = (buffer != nullptr && buffer[0] != '\0') ? buffer : "(kosong)";
        ImGui::Text("%s", activeTextEditorTitle);
        ImGui::Separator();
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.10f, 0.12f, 0.15f, 1.0f));
        ImGui::BeginChild("touch_text_preview", ImVec2(760, 84), true, ImGuiWindowFlags_NoScrollbar);
        ImGui::TextWrapped("%s", preview);
        ImGui::EndChild();
        ImGui::PopStyleColor();

        static const char *keyboard_rows[] = {
                "ABCDEFGHIJ",
                "KLMNOPQRST",
                "UVWXYZ0123",
                "456789:#@",
                "+*/_-."
        };

        for (const char *row : keyboard_rows) {
            const int row_length = static_cast<int>(std::strlen(row));
            for (int i = 0; i < row_length; ++i) {
                char label[8];
                std::snprintf(label, sizeof(label), "%c", row[i]);
                if (ImGui::Button(label, ImVec2(64, 52))) {
                    AppendTextEditorCharacter(row[i]);
                }
                if (i + 1 < row_length) {
                    ImGui::SameLine();
                }
            }
        }

        if (ImGui::Button("SPACE", ImVec2(140, 52))) {
            AppendTextEditorCharacter(' ');
        }
        ImGui::SameLine();
        if (ImGui::Button("BACK", ImVec2(140, 52))) {
            BackspaceTextEditorCharacter();
        }
        ImGui::SameLine();
        if (ImGui::Button("CLEAR", ImVec2(140, 52))) {
            ClearTextEditorBuffer();
        }
        ImGui::SameLine();
        if (ImGui::Button("DONE", ImVec2(140, 52))) {
            ImGui::CloseCurrentPopup();
            activeTextEditorSlot = -2;
        }

        ImGui::EndPopup();
        return;
    }

    if (!g_TextEditorPopupOpen && activeTextEditorSlot != -2) {
        activeTextEditorSlot = -2;
    }
}

static void ApplyTouchScrollToCurrentWindow(bool use_picker_state = false, float speed = 1.35f) {
    ImGuiWindow *window = ImGui::GetCurrentWindow();
    if (window == nullptr) {
        return;
    }

    float &pending_delta = use_picker_state ? g_PickerPendingTouchScrollDeltaY : g_MainPendingTouchScrollDeltaY;
    float &scroll_velocity = use_picker_state ? g_PickerTouchScrollVelocityY : g_MainTouchScrollVelocityY;

    const bool hovered = ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max, true);
    if (g_TouchIsDown) {
        if (!hovered) {
            return;
        }

        if (std::fabs(pending_delta) < 0.08f) {
            return;
        }

        const float direct_delta = -(pending_delta * speed);
        window->Scroll.y = ImClamp(window->Scroll.y + direct_delta, 0.0f, window->ScrollMax.y);
        scroll_velocity = (scroll_velocity * 0.35f) + (direct_delta * 0.65f);
        pending_delta = 0.0f;
        return;
    }

    if (std::fabs(scroll_velocity) < 0.18f) {
        scroll_velocity = 0.0f;
        return;
    }

    window->Scroll.y = ImClamp(window->Scroll.y + scroll_velocity, 0.0f, window->ScrollMax.y);

    scroll_velocity *= 0.90f;
}

void BeginDraw() {
    ImGuiIO &io = ImGui::GetIO();
    static float menu_alpha = 0.0f;
    const float alpha_step = g_ShowMenu ? 0.12f : 0.18f;
    menu_alpha = ImClamp(menu_alpha + (g_ShowMenu ? alpha_step : -alpha_step), 0.0f, 1.0f);

    const ImGuiViewport *main_viewport = ImGui::GetMainViewport();
    if (!g_ShowMenu) {
        // Dragable & Auto-resize
        ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 24, main_viewport->WorkPos.y + 80), ImGuiCond_FirstUseEver);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImMax(menu_alpha, 0.85f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 16.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.09f, 0.12f, 0.90f));

        const bool mini_visible = ImGui::Begin("StrikeOps Mini", nullptr,
                                               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
                                               ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar);
        if (mini_visible) {
            g_window = ImGui::GetCurrentWindow();
            
            // --- DAMAGE PREDICTOR HUD ---
            RefreshCalcEnemyFromMemory();
            RefreshAutoCalcSkillSnapshots();

            if (g_MenuBoldFont) ImGui::PushFont(g_MenuBoldFont);
            ImGui::TextColored(AccentColor, "BB2 PREDICTOR");
            if (g_MenuBoldFont) ImGui::PopFont();
            ImGui::Separator();
            
            if (!calcEnemyCaptured) {
                ImGui::TextDisabled("Musuh tidak terdeteksi.");
            } else {
                ImGui::Text("Target: %.0f HP | Shield: %.0f", calcEnemyCurrentHp, calcEnemyShield);
                ImGui::Spacing();
                
                for (int i = 0; i < kCalcSkillSlotCount; ++i) {
                    auto &snapshot = calcSkillSnapshots[i];
                    if (!snapshot.active || !snapshot.isReady) continue;

                    calcResults[i] = CalcDamagePreview(snapshot);
                    const auto &r = calcResults[i];

                    ImGui::TextColored(ImVec4(1, 1, 1, 1), "%s", snapshot.name[0] ? snapshot.name : "Skill");
                    ImGui::SameLine();
                    
                    if (r.isHeal) {
                        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.5f, 1.0f), "-> +%.0f HP", r.healTotal);
                    } else if (r.isBuff) {
                        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "-> Utility");
                    } else {
                        ImGui::TextColored(ImVec4(1.0f, 0.65f, 0.2f, 1.0f), "-> %.0f DMG", r.expected);
                        if (r.shieldBreak) ImGui::SameLine(), ImGui::TextColored(ImVec4(0.9f, 0.8f, 0.2f, 1.0f), "[BREAK]");
                        if (r.lethal) ImGui::SameLine(), ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "[LETHAL]");
                    }
                }
            }
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            
            if (ImGui::Button("OPEN MOD MENU (Drag Me!)", ImVec2(ImMax(ImGui::GetContentRegionAvail().x, 320.0f), 54))) {
                g_ShowMenu = true;
            }
        }
        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
        return;
    }

    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 90, main_viewport->WorkPos.y + 180), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(
            ImClamp(main_viewport->WorkSize.x * 0.78f, 820.0f, 1040.0f),
            ImClamp(main_viewport->WorkSize.y * 0.76f, 620.0f, 920.0f)), ImGuiCond_FirstUseEver);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImMax(menu_alpha, 0.25f));

    const bool main_visible = ImGui::Begin("StrikeOps x Buriedbornes2", nullptr,
                                           ImGuiWindowFlags_NoCollapse);
    if (main_visible) {
        g_window = ImGui::GetCurrentWindow();
        ImRect expanded_title_rect(g_window->Pos,
                                   ImVec2(g_window->Pos.x + g_window->Size.x,
                                          g_window->Pos.y + 116.0f));
        if (g_HasPendingDoubleTap &&
            expanded_title_rect.Contains(ImVec2(g_PendingDoubleTapX, g_PendingDoubleTapY))) {
            g_ShowMenu = false;
            g_HasPendingDoubleTap = false;
        }

        // --- Smooth Transition Logic ---
        static int pending_Hell = Hell;
        static float content_alpha = 1.0f;

        // --- HEADER TITLE ---
        if (g_MenuBoldFont != nullptr) {
            ImGui::PushFont(g_MenuBoldFont);
        }
        ImGui::PushStyleColor(ImGuiCol_Text, AccentColor);
        ImGui::Text("MOD MENU PRO [BB2 x StrikeOps]");
        ImGui::PopStyleColor();
        if (g_MenuBoldFont != nullptr) {
            ImGui::PopFont();
        }
        ImGui::SameLine();
        ImGui::TextColored(SoftTextColor, " FPS: %.1f | Build: %s", io.Framerate, kStatsBuildTag);
        ImGui::TextColored(SoftTextColor, "Double tap area teratas panel untuk menyembunyikan.");
        ImGui::Separator();

        // --- NAVIGATION DROPDOWN ---
        const char* tabNames[] = { "1. REWARDS", "2. DEBUG", "3. STATS", "4. ABOUT" };
        const int tabIndices[] = { 1, 2, 3, 5 }; // Hell values. Calc (4) is hidden.
        
        int current_dropdown_idx = 0;
        for (int i = 0; i < 4; ++i) {
            if (pending_Hell == tabIndices[i]) {
                current_dropdown_idx = i;
                break;
            }
        }
        
        ImGui::PushItemWidth(-1);
        if (ImGui::BeginCombo("##MenuNavCombo", tabNames[current_dropdown_idx], ImGuiComboFlags_HeightLarge)) {
            for (int i = 0; i < 4; ++i) {
                const bool is_selected = (current_dropdown_idx == i);
                if (ImGui::Selectable(tabNames[i], is_selected)) {
                    pending_Hell = tabIndices[i];
                }
                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::PopItemWidth();
        ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();

        // --- TRANSITION ALPHA ---
        if (pending_Hell != Hell) {
            content_alpha -= io.DeltaTime * 8.0f; // Fast Fade Out
            if (content_alpha <= 0.0f) {
                Hell = pending_Hell;
                content_alpha = 0.0f;
            }
        } else {
            content_alpha = ImMin(content_alpha + io.DeltaTime * 8.0f, 1.0f); // Fade In
        }

        // --- CONTENT LAYOUT ---
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, content_alpha * ImMax(menu_alpha, 0.25f));
        ImGui::BeginGroup();
        ImGui::BeginChild("scroll_area", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);
        if (activeStatPickerSlot < 0 && activeTextEditorSlot == -2) {
            ApplyTouchScrollToCurrentWindow();
        }

        if (Hell == 1) {
            BeginCard("InfoCard", 86);
            ImGui::TextWrapped("Target reward sekarang dipisah: Soulstone gameplay pakai GainSoulstone(int value, string reason), "
                               "dan Goldshard lobby/premium pakai GainGoldshard(ObscuredDouble value, string reason).");
            EndCard();

            ImGui::Spacing();

            BeginCard("CurrencyCard");
            SectionHeaderLite("Currency Multipliers");
            if (ImGui::BeginTable("reward_table", 2, ImGuiTableFlags_SizingStretchSame)) {
                ImGui::TableNextColumn();
                ToggleSwitch("Enable Soulstone Multiplier", &soulstoneMultiplierEnabled);
                ImGui::TableNextColumn();
                ImGui::SliderInt("Soulstone Multiplier", &soulstoneMultiplierValue, 1, 20);
                
                ImGui::TableNextColumn(); ImGui::Spacing(); ImGui::TableNextColumn(); ImGui::Spacing();
                
                ImGui::TableNextColumn();
                ToggleSwitch("Enable Goldshard Multiplier", &goldshardMultiplierEnabled);
                ImGui::TableNextColumn();
                ImGui::SliderInt("Goldshard Multiplier", &goldshardMultiplierValue, 1, 20);
                ImGui::EndTable();
            }
            ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
            ImGui::TextWrapped("Soulstone dikali langsung dari parameter int di GainSoulstone. "
                               "Goldshard melalui dekripsi aman ObscuredDouble jadi nggak berisiko server ban.");
            EndCard();
        }

        if (Hell == 2) {
            ToggleSwitch("Show debug stats", &showDebugStats);
            ImGui::Separator();
            if (showDebugStats) {
                ImGui::Text("Resolver guard: %s", resolverVersionGuardMatched ? "known version / fallback allowed" : "new version / fallback blocked");
                ImGui::TextWrapped("Resolver fingerprint: %s", resolverFingerprintReady ? resolverFingerprintText : "pending");
                ImGui::Text("Resolver counts by-name/fallback/blocked: %llu / %llu / %llu",
                            static_cast<unsigned long long>(resolverByNameCount),
                            static_cast<unsigned long long>(resolverFallbackCount),
                            static_cast<unsigned long long>(resolverBlockedFallbackCount));
                ImGui::Separator();
                ImGui::Text("GainSoulstone hook: %s [%s]", gainSoulstoneHookInstalled ? "installed" : "missing", gainSoulstoneResolveMode);
                ImGui::Text("GainGoldshard hook: %s [%s]", gainGoldshardHookInstalled ? "installed" : "missing", gainGoldshardResolveMode);
                ImGui::Text("Battler PVal hook: %s [%s]", battlerPValHookInstalled ? "installed" : "missing", battlerPValResolveMode);
                ImGui::Text("Battler CalcedPVal hook: %s [%s]", battlerCalcedPValHookInstalled ? "installed" : "missing", battlerCalcedPValResolveMode);
                ImGui::Text("Battler AddPassive hook: %s [%s]", battlerAddPassiveHookInstalled ? "installed" : "missing", battlerAddPassiveResolveMode);
                ImGui::Text("Battler AddTemp hook: %s [%s]", battlerAddTempHookInstalled ? "installed" : "missing", battlerAddTempResolveMode);
                ImGui::Text("Battler IsPlayerSelf hook: %s [%s]", battlerIsPlayerSelfHookInstalled ? "installed" : "missing", battlerIsPlayerSelfResolveMode);
                ImGui::Text("SecureKeyValueSet Get hook: %s [%s]", secureKeyValueSetGetHookInstalled ? "installed" : "missing", secureKeyValueSetGetResolveMode);
                ImGui::Text("SecureKeyValueSet Set hook: %s [%s]", secureKeyValueSetSetHookInstalled ? "installed" : "missing", secureKeyValueSetSetResolveMode);
                ImGui::Text("SecureKeyValueSet Gain hook: %s [%s]", secureKeyValueSetGainHookInstalled ? "installed" : "missing", secureKeyValueSetGainResolveMode);
                ImGui::Text("SecureKeyValueSet Lose hook: %s [%s]", secureKeyValueSetLoseHookInstalled ? "installed" : "missing", secureKeyValueSetLoseResolveMode);
                ImGui::Text("SealedNumber.Get resolve: %s", sealedNumberGetResolveMode);
                ImGui::Text("ObscuredDouble helpers: %s", obscuredDoubleCryptoResolveMode);
                ImGui::Text("IsPlayerSelf ready: %s", battlerIsPlayerSelfReady ? "yes" : "no");
                ImGui::Text("Player captured: %s", hasCapturedPlayerBattler ? "yes" : "no");
                ImGui::Separator();
                ImGui::Text("GainSoulstone hits: %llu", static_cast<unsigned long long>(gainSoulstoneHitCount));
                ImGui::Text("GainGoldshard hits: %llu", static_cast<unsigned long long>(gainGoldshardHitCount));
                ImGui::Text("PVal hits: %llu", static_cast<unsigned long long>(battlerPValHitCount));
                ImGui::Text("PVal modified: %llu", static_cast<unsigned long long>(battlerPValModifiedCount));
                ImGui::Text("CalcedPVal hits: %llu", static_cast<unsigned long long>(battlerCalcedPValHitCount));
                ImGui::Text("CalcedPVal modified: %llu", static_cast<unsigned long long>(battlerCalcedPValModifiedCount));
                ImGui::Text("AddPassive hits: %llu", static_cast<unsigned long long>(battlerAddPassiveHitCount));
                ImGui::Text("AddTemp hits: %llu", static_cast<unsigned long long>(battlerAddTempHitCount));
                ImGui::Text("IsPlayerSelf hits: %llu", static_cast<unsigned long long>(battlerIsPlayerSelfHitCount));
                ImGui::Text("SecureKeyValueSet Get hits: %llu", static_cast<unsigned long long>(secureKeyValueSetGetHitCount));
                ImGui::Text("SecureKeyValueSet Set hits: %llu", static_cast<unsigned long long>(secureKeyValueSetSetHitCount));
                ImGui::Text("SecureKeyValueSet Gain hits: %llu", static_cast<unsigned long long>(secureKeyValueSetGainHitCount));
                ImGui::Text("SecureKeyValueSet Lose hits: %llu", static_cast<unsigned long long>(secureKeyValueSetLoseHitCount));
                ImGui::Text("Soulstone multiplier: %d (%s)", soulstoneMultiplierValue, soulstoneMultiplierEnabled ? "on" : "off");
                ImGui::Text("Goldshard multiplier: %d (%s)", goldshardMultiplierValue, goldshardMultiplierEnabled ? "on" : "off");
                ImGui::Text("Last Soulstone reward: %.0f -> %.0f", lastSoulstoneOriginalValue, lastSoulstoneModifiedValue);
                ImGui::Text("Last Soulstone reason: %s", lastSoulstoneReason[0] ? lastSoulstoneReason : "-");
                ImGui::Text("Last Goldshard reward: %.2f -> %.2f", lastGoldshardOriginalValue, lastGoldshardModifiedValue);
                ImGui::Text("Last Goldshard reason: %s", lastGoldshardReason[0] ? lastGoldshardReason : "-");
                ImGui::Text("Last PVal key: %s", lastPValKey[0] ? lastPValKey : "-");
                ImGui::Text("Last PVal original: %.4f", lastPValOriginalValue);
                ImGui::Text("Last PVal modified: %.4f", lastPValModifiedValue);
                ImGui::Text("Last Calced key: %s", lastCalcedPValKey[0] ? lastCalcedPValKey : "-");
                ImGui::Text("Last Calced original: %.4f", lastCalcedPValOriginalValue);
                ImGui::Text("Last Calced modified: %.4f", lastCalcedPValModifiedValue);
                ImGui::Text("Last AddPassive: %s = %d", lastAddPassiveKey[0] ? lastAddPassiveKey : "-", lastAddPassiveValue);
                ImGui::Text("Last AddTemp: %s = %d", lastAddTempKey[0] ? lastAddTempKey : "-", lastAddTempValue);
                ImGui::Text("Last SecureSet: %s = %.2f", lastSecureSetKey[0] ? lastSecureSetKey : "-", lastSecureSetValue);
                ImGui::Text("Last matched stat key: %s", lastMatchedStatKey[0] ? lastMatchedStatKey : "-");
                ImGui::Separator();
                ImGui::Text("GainSoulstone addr: 0x%llX", static_cast<unsigned long long>(gainSoulstoneTargetAddress));
                ImGui::Text("GainGoldshard addr: 0x%llX", static_cast<unsigned long long>(gainGoldshardTargetAddress));
                ImGui::Text("PVal addr: 0x%llX", static_cast<unsigned long long>(battlerPValTargetAddress));
                ImGui::Text("CalcedPVal addr: 0x%llX", static_cast<unsigned long long>(battlerCalcedPValTargetAddress));
                ImGui::Text("AddPassive addr: 0x%llX", static_cast<unsigned long long>(battlerAddPassiveTargetAddress));
                ImGui::Text("AddTemp addr: 0x%llX", static_cast<unsigned long long>(battlerAddTempTargetAddress));
                ImGui::Text("SecureSet Get addr: 0x%llX", static_cast<unsigned long long>(secureKeyValueSetGetTargetAddress));
                ImGui::Text("SecureSet Set addr: 0x%llX", static_cast<unsigned long long>(secureKeyValueSetSetTargetAddress));
                ImGui::Text("SecureSet Gain addr: 0x%llX", static_cast<unsigned long long>(secureKeyValueSetGainTargetAddress));
                ImGui::Text("SecureSet Lose addr: 0x%llX", static_cast<unsigned long long>(secureKeyValueSetLoseTargetAddress));
                ImGui::Text("IsPlayerSelf addr: 0x%llX", static_cast<unsigned long long>(battlerIsPlayerSelfAddress));
            }
        }

        if (Hell == 3) {
            if (observedStatKeyCount == 0) {
                SeedObservedStatCatalog();
            }
            ImGui::TextWrapped("Slot stat sekarang pakai model konfirmasi. Pilih key, isi delta, lalu "
                               "tekan tombol ADD biar nilainya dikirim ke player status.");
            ImGui::TextColored(AccentColor, "Build: %s", kStatsBuildTag);
            ImGui::TextWrapped("List stat sekarang sudah di-seed dari katalog known key hasil dump runtime, jadi dari awal harusnya sudah ada banyak opsi tanpa nunggu trigger.");
            ImGui::TextWrapped("Hint: MainParam:1/2/3/4 = STR/INT/DEX/PIE, dan CritDmg = Crit Damage. "
                               "Suffix umum: # total, + flat, * percent, @ passive.");
            ImGui::Spacing();
            ToggleSwitch("Hide internal keys", &statShowCoreOnly);
            ImGui::SameLine();
            ToggleSwitch("Hide zero values", &statShowNonZeroOnly);
            ImGui::Text("Filter: %s", statFilterText[0] ? statFilterText : "(kosong)");
            ImGui::SameLine();
            if (ImGui::Button("FILTER", ImVec2(0, 48))) {
                OpenTextEditorPopup(-1, "Filter Stat");
            }
            ImGui::SameLine();
            if (ImGui::Button("CLEAR", ImVec2(0, 48))) {
                statFilterText[0] = '\0';
            }
            ImGui::Text("Observed stats: %d", observedStatKeyCount);
            ImGui::Text("Last observed stat: %s", lastObservedStatKey[0] ? lastObservedStatKey : "-");
            ImGui::Text("Apply status: %s", lastApplyStatus);
            ImGui::Text("Apply req/success/fail: %llu / %llu / %llu",
                        static_cast<unsigned long long>(statApplyRequestCount),
                        static_cast<unsigned long long>(statApplySuccessCount),
                        static_cast<unsigned long long>(statApplyFailureCount));
            ImGui::Text("Last applied: %s %+d",
                        lastAppliedStatKey[0] ? lastAppliedStatKey : "-",
                        lastAppliedStatDelta);
            ImGui::Separator();

            for (int i = 0; i < kStatSlotCount; ++i) {
                ImGui::PushID(i);
                const int selected_index = selectedObservedStatIndices[i];
                const char *preview = "Choose observed stat";
                std::string preview_label;
                if (selected_index >= 0 && selected_index < observedStatKeyCount && observedStatKeys[selected_index][0]) {
                    preview_label = HumanizeStatKey(observedStatKeys[selected_index]) + " [" + observedStatKeys[selected_index] + "]";
                    preview = preview_label.c_str();
                } else if (statBonusKeys[i][0]) {
                    preview_label = HumanizeStatKey(statBonusKeys[i]) + " [" + statBonusKeys[i] + "]";
                    preview = preview_label.c_str();
                }

                ImGui::TextWrapped("Selected: %s", preview);
                if (ImGui::Button("CHOOSE STAT", ImVec2(-1, 64))) {
                    g_PickerPendingTouchScrollDeltaY = 0.0f;
                    g_PickerTouchScrollVelocityY = 0.0f;
                    g_IgnoreNextPickerSelection = false;
                    activeStatPickerSlot = i;
                }

                ImGui::TextWrapped("Key: %s", statBonusKeys[i][0] ? statBonusKeys[i] : "(belum dipilih)");
                if (ImGui::Button("CLEAR STAT", ImVec2(0, 48))) {
                    selectedObservedStatIndices[i] = -1;
                    statBonusKeys[i][0] = '\0';
                }

                ImGui::SetNextItemWidth(190.f);
                ImGui::DragFloat("Delta", &statBonusValues[i], 1.0f, -99999.0f, 99999.0f, "%.0f");

                ImGui::PushButtonRepeat(true);
                if (ImGui::Button("-1000")) statBonusValues[i] -= 1000.f; ImGui::SameLine();
                if (ImGui::Button("-100")) statBonusValues[i] -= 100.f; ImGui::SameLine();
                if (ImGui::Button("-10")) statBonusValues[i] -= 10.f; ImGui::SameLine();
                if (ImGui::Button("-1")) statBonusValues[i] -= 1.f;
                
                if (ImGui::Button("+1")) statBonusValues[i] += 1.f; ImGui::SameLine();
                if (ImGui::Button("+10")) statBonusValues[i] += 10.f; ImGui::SameLine();
                if (ImGui::Button("+100")) statBonusValues[i] += 100.f; ImGui::SameLine();
                if (ImGui::Button("+1000")) statBonusValues[i] += 1000.f; ImGui::SameLine();
                if (ImGui::Button("0")) statBonusValues[i] = 0.f;
                ImGui::PopButtonRepeat();

                const bool can_apply = statBonusKeys[i][0] != '\0' && static_cast<int>(statBonusValues[i]) != 0;
                if (!can_apply) {
                    ImGui::BeginDisabled();
                }
                if (ImGui::Button("ADD", ImVec2(0, 48))) {
                    QueueStatApplyRequest(i);
                }
                if (!can_apply) {
                    ImGui::EndDisabled();
                }

                ImGui::PopID();
            }

            if (activeStatPickerSlot < 0) {
                const std::vector<int> &sorted_stat_indices = GatherSortedObservedStatIndices();
                if (ImGui::Button("Clear Stat Slots", ImVec2(0, 48))) {
                    for (int i = 0; i < kStatSlotCount; ++i) {
                        statBonusKeys[i][0] = '\0';
                        statBonusValues[i] = 0.f;
                        selectedObservedStatIndices[i] = -1;
                    }
                }

                ImGui::Spacing();
                ImGui::Text("Last runtime PVal key: %s", lastPValKey[0] ? lastPValKey : "-");
                ImGui::Text("Last runtime Calced key: %s", lastCalcedPValKey[0] ? lastCalcedPValKey : "-");
                ImGui::Text("Last matched: %s", lastMatchedStatKey[0] ? lastMatchedStatKey : "-");
                ImGui::Separator();

                if (ImGui::BeginTable("observed_stats_table", 5,
                                      ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp)) {
                    ImGui::TableSetupColumn("Label");
                    ImGui::TableSetupColumn("Key");
                    ImGui::TableSetupColumn("Source");
                    ImGui::TableSetupColumn("Base");
                    ImGui::TableSetupColumn("Calced");
                    ImGui::TableHeadersRow();

                    ImGuiListClipper clipper;
                    clipper.Begin(static_cast<int>(sorted_stat_indices.size()));
                    while (clipper.Step()) {
                        for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row) {
                            const int stat_index = sorted_stat_indices[row];
                            ImGui::TableNextRow();

                            ImGui::TableSetColumnIndex(0);
                            ImGui::TextUnformatted(HumanizeStatKey(observedStatKeys[stat_index]).c_str());

                            ImGui::TableSetColumnIndex(1);
                            ImGui::TextUnformatted(observedStatKeys[stat_index]);

                            ImGui::TableSetColumnIndex(2);
                            if (observedStatHasSourceVal[stat_index]) {
                                ImGui::Text("%.0f", observedStatSourceVal[stat_index]);
                            } else {
                                ImGui::TextUnformatted("-");
                            }

                            ImGui::TableSetColumnIndex(3);
                            if (observedStatHasPVal[stat_index]) {
                                ImGui::Text("%.2f", observedStatPVal[stat_index]);
                            } else {
                                ImGui::TextUnformatted("-");
                            }

                            ImGui::TableSetColumnIndex(4);
                            if (observedStatHasCalcedVal[stat_index]) {
                                ImGui::Text("%.2f", observedStatCalcedVal[stat_index]);
                            } else {
                                ImGui::TextUnformatted("-");
                            }
                        }
                    }

                    ImGui::EndTable();
                }
            }
        }

        // (Tab CALC / Hell == 4 sudah dipindahkan menjadi Live Overlay di Mini Menu)

        if (Hell == 5) {
            BeginCard("AboutCard");
            SectionHeaderLite("About Project");
            ImGui::TextWrapped("UI diambil dari basis StrikeOps yang kamu suka, tapi logic hook "
                               "dipangkas buat fokus ke Buriedbornes 2.");
            ImGui::Spacing();
            ImGui::BulletText("Target lib: libil2cpp.so");
            ImGui::BulletText("Method: BB2UserStoredData$$GainSoulstone");
            ImGui::BulletText("Method: BB2UserStoredData$$GainGoldshard");
            ImGui::BulletText("Soulstone dan Goldshard sekarang dipisah jadi dua toggle sendiri.");
            EndCard();
        }

        ImGui::EndChild(); // scroll_area
        ImGui::EndGroup(); // content group
        ImGui::PopStyleVar(); // pop alpha transition

        if (Hell == 3 && activeStatPickerSlot >= 0) {
            const std::vector<int> &picker_sorted_indices = GatherSortedObservedStatIndices();
            ImGui::SetNextWindowPos(main_viewport->Pos, ImGuiCond_Always);
            ImGui::SetNextWindowSize(main_viewport->Size, ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(0.72f);
            if (ImGui::Begin("Stat Picker Overlay", nullptr,
                             ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
                ImGui::SetCursorPos(ImVec2(40, 40));
                ImGui::BeginChild("stat_picker_panel", ImVec2(main_viewport->Size.x - 80, main_viewport->Size.y - 80),
                                  true, ImGuiWindowFlags_NoScrollbar);
                ImGui::Text("Pilih stat untuk slot %d", activeStatPickerSlot + 1);
                ImGui::TextWrapped("Drag daftar ini buat scroll, lalu tap stat yang mau dipakai.");
                ImGui::Separator();
                if (ImGui::Button("TUTUP", ImVec2(180, 56))) {
                    activeStatPickerSlot = -1;
                }
                ImGui::Spacing();
                ImGui::BeginChild("stat_picker_list", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar);
                ApplyTouchScrollToCurrentWindow(true, 1.10f);
                ImGuiListClipper clipper;
                clipper.Begin(static_cast<int>(picker_sorted_indices.size()));
                bool picker_selection_committed = false;
                while (!picker_selection_committed && clipper.Step()) {
                    for (int row = clipper.DisplayStart; row < clipper.DisplayEnd && !picker_selection_committed; ++row) {
                        const int stat_index = picker_sorted_indices[row];
                        const std::string alias = HumanizeStatKey(observedStatKeys[stat_index]);
                        char option_label[256];
                        if (observedStatHasCalcedVal[stat_index] && observedStatHasSourceVal[stat_index]) {
                            std::snprintf(option_label, sizeof(option_label), "%s [%s] | src %.0f | calced %.2f",
                                          alias.c_str(), observedStatKeys[stat_index], observedStatSourceVal[stat_index], observedStatCalcedVal[stat_index]);
                        } else if (observedStatHasCalcedVal[stat_index]) {
                            std::snprintf(option_label, sizeof(option_label), "%s [%s] | calced %.2f",
                                          alias.c_str(), observedStatKeys[stat_index], observedStatCalcedVal[stat_index]);
                        } else if (observedStatHasSourceVal[stat_index]) {
                            std::snprintf(option_label, sizeof(option_label), "%s [%s] | src %.0f",
                                          alias.c_str(), observedStatKeys[stat_index], observedStatSourceVal[stat_index]);
                        } else if (observedStatHasPVal[stat_index]) {
                            std::snprintf(option_label, sizeof(option_label), "%s [%s] | base %.2f",
                                          alias.c_str(), observedStatKeys[stat_index], observedStatPVal[stat_index]);
                        } else {
                            std::snprintf(option_label, sizeof(option_label), "%s [%s]", alias.c_str(), observedStatKeys[stat_index]);
                        }

                        if (ImGui::Button(option_label, ImVec2(-1, 62))) {
                            if (g_IgnoreNextPickerSelection) {
                                g_IgnoreNextPickerSelection = false;
                                continue;
                            }
                            selectedObservedStatIndices[activeStatPickerSlot] = stat_index;
                            std::snprintf(statBonusKeys[activeStatPickerSlot], sizeof(statBonusKeys[activeStatPickerSlot]), "%s", observedStatKeys[stat_index]);
                            activeStatPickerSlot = -1;
                            picker_selection_committed = true;
                        }
                    }
                }
                ImGui::EndChild();
                ImGui::EndChild();
            }
            ImGui::End();
        }

        RenderTouchTextEditorPopup();

    }
    ImGui::End();
    ImGui::PopStyleVar();
}
