#include "stdafx.h"

#include <GWCA/Constants/Constants.h>

#include <GWCA/Context/WorldContext.h>
#include <GWCA/GameEntities/Agent.h>
#include <GWCA/GameEntities/Skill.h>

#include <GWCA/Managers/AgentMgr.h>
#include <GWCA/Managers/GameThreadMgr.h>
#include <GWCA/Managers/MapMgr.h>
#include <GWCA/Managers/PartyMgr.h>

#include <Color.h>
#include <Utils/GuiUtils.h>
#include <Windows/EnemyWindow.h>

#include <Timer.h>
#include <GWCA/Managers/SkillbarMgr.h>

using namespace GW::Agents;

namespace {

    bool hide_when_nothing = true;
    bool show_enemies_counter = true;
    float enemies_threshhold = 1.0f;
    float range = 1248.0f;
    float triangleSpacing = 22.0f;
    int triangleYOffset = 3;
    float triangleSizeMultiplier = 1.0f;

    struct enemyinfo {
        enemyinfo(const GW::AgentID agent_id) : agent_id(agent_id) {}
        GW::AgentID agent_id;
        clock_t last_casted = 0;
        uint16_t last_skill;
        float distance = FLT_MAX;
    };

    GW::AgentLiving* GetAgentLivingByID(const uint32_t agent_id)
    {
        const auto a = GW::Agents::GetAgentByID(agent_id);
        return a ? a->GetAsAgentLiving() : nullptr;
    }

    void drawStatusTriangle(int triangleCount, ImVec2 position, ImU32 triangleColor, bool upsidedown)
    {
        ImVec2 point1, point2, point3;

        if (upsidedown) {
            point1 = ImVec2(position.x - (triangleCount * triangleSpacing), position.y + triangleYOffset);
            point2 = ImVec2(position.x + (20 * triangleSizeMultiplier) - (triangleCount * triangleSpacing), position.y + triangleYOffset);
            point3 = ImVec2(position.x + (10 * triangleSizeMultiplier) - (triangleCount * triangleSpacing), position.y + triangleYOffset + (10 * triangleSizeMultiplier));       
        }
        else {
            point1 = ImVec2(position.x - (triangleCount * triangleSpacing), position.y + triangleYOffset + (10 * triangleSizeMultiplier));
            point2 = ImVec2(position.x + (20 * triangleSizeMultiplier) - (triangleCount * triangleSpacing), position.y + triangleYOffset + (10 * triangleSizeMultiplier));
            point3 = ImVec2(position.x + (10 * triangleSizeMultiplier) - (triangleCount * triangleSpacing), position.y + triangleYOffset);
        }

        ImGui::GetWindowDrawList()->AddTriangleFilled(point1, point2, point3, triangleColor);
    }


    uint32_t GetAgentMaxHP(const GW::AgentLiving* agent)
    {
        if (!agent) {
            return 0; // Invalid agent
        }
        if (agent->max_hp) {
            return agent->max_hp;
        }
        return 0;
    }

    int GetHealthRegenPips(const GW::AgentLiving* agent)
    {
        const auto max_hp = GetAgentMaxHP(agent);
        if (!(max_hp && agent->hp_pips != .0f)) {
            return 0; 
        }
        const float health_regen_per_second = max_hp * agent->hp_pips;
        const float pips = std::ceil(health_regen_per_second / 2.f); // 1 pip = 2 health per second
        return static_cast<int>(pips);
    }

    bool OrderEnemyInfo(const enemyinfo& a, const enemyinfo& b)
    {
        return a.distance < b.distance;
    }

    void DrawDuping(const char* label, const std::vector<enemyinfo>& vec)
    {

        if (vec.empty()) {
            return;
        }

        ImGui::Spacing();
        if (ImGui::BeginTable(label, 4)) {
            ImGui::TableSetupColumn("Selection", ImGuiTableColumnFlags_WidthFixed, 0, 0);
            ImGui::TableSetupColumn("HP", ImGuiTableColumnFlags_WidthStretch, -1, 0);
            ImGui::TableSetupColumn("Regen", ImGuiTableColumnFlags_WidthFixed, 70, 1);
            ImGui::TableSetupColumn("Last Casted", ImGuiTableColumnFlags_WidthFixed, 40, 2);

            const GW::Agent* target = GW::Agents::GetTarget();

            for (const auto& dupe_info : vec) {
                const auto living = GetAgentLivingByID(dupe_info.agent_id);
                if (!living || dupe_info.distance > range * range) {
                    continue;
                }
                const auto selected = target && target->agent_id == living->agent_id;

                std::wstring agent_name_enc;
                GW::Agents::AsyncGetAgentName(living, agent_name_enc);
                if (agent_name_enc.empty()) {
                    continue;
                }

                const std::string agent_name_str = GuiUtils::WStringToString(agent_name_enc);
                std::string skillname;

                ImGui::PushID(dupe_info.agent_id);
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);

                if (ImGui::Selectable("", selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2(0, 23))) {
                    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                        GW::GameThread::Enqueue([living] {
                            GW::Agents::ChangeTarget(living);
                        });
                    }
                }

                // Progress bar
                ImGui::TableSetColumnIndex(1);
                ImVec2 progressBarPos = ImGui::GetCursorScreenPos();
                ImVec2 Pos1 = ImVec2(progressBarPos.x + ImGui::GetContentRegionAvail().x * 0.025f, progressBarPos.y + 3);
                ImVec2 Pos2 = ImVec2(progressBarPos.x + ImGui::GetContentRegionAvail().x - 25, progressBarPos.y + 3);
                ImU32 HexedColor = IM_COL32(253, 113, 255, 255);
                ImU32 ConditionedColor = IM_COL32(160, 117, 85, 255);
                ImU32 EnchantedColor = IM_COL32(224, 253, 94, 255);

                int triangles = 0;

                if (living->skill != 0) {
                    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.165f, 0.196f, 0.294f, 1.0f));
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.6117647058823529f, 0.0f, 0.0f, 1.0f));
                }
                ImGui::ProgressBar(living->hp, ImVec2(-1, 0), "");
                ImGui::PopStyleColor();

                if (dupe_info.last_skill != 0) {
                    const GW::Skill* skill_data = GW::SkillbarMgr::GetSkillConstantData(static_cast<GW::Constants::SkillID>(dupe_info.last_skill));
                    ASSERT(skill_data);
                    auto enc_skillname = new GuiUtils::EncString(skill_data->name);
                    skillname = enc_skillname->string();
                }

                if (TIMER_DIFF(dupe_info.last_casted) < 3000) {
                    ImGui::GetWindowDrawList()->AddText(Pos1, IM_COL32(253, 255, 255, 255), (agent_name_str + " - " + skillname).c_str());
                }
                else {
                    ImGui::GetWindowDrawList()->AddText(Pos1, IM_COL32(253, 255, 255, 255), (agent_name_str).c_str());
                }

                if (living->GetIsEnchanted()) {
                    drawStatusTriangle(triangles, Pos2, EnchantedColor, false);
                    triangles++;
                }
                
                if (living->GetIsHexed()) {
                    drawStatusTriangle(triangles, Pos2, HexedColor, true);
                    triangles++;
                }

                if (living->GetIsConditioned()) {
                    drawStatusTriangle(triangles, Pos2, ConditionedColor, true);
                }

                //Health pips
                ImGui::TableSetColumnIndex(2);
                const auto pips = GetHealthRegenPips(living);
                if (pips > 0 && pips < 11) {
                    ImGui::Text("%.*s", pips > 0 && pips < 11 ? pips : 0, ">>>>>>>>>>");
                }

                //Last Casted Skill
                ImGui::TableSetColumnIndex(3);
                if (dupe_info.last_casted != 0) {
                    const auto seconds_ago = static_cast<int>((TIMER_DIFF(dupe_info.last_casted) / CLOCKS_PER_SEC));
                    const auto [quot, rem] = std::div(seconds_ago, 60);
                    ImGui::Text("%d:%02d", quot, rem);
                }
                else {
                    ImGui::Text("-");
                }
                ImGui::PopID();
            }
            ImGui::EndTable();
        }
    }

    std::vector<enemyinfo> enemies{};
    std::set<GW::AgentID> all_enemies;

    void clearenemies()
    {
        enemies.clear();
        all_enemies.clear();
    }

} // namespace

void EnemyWindow::Terminate()
{
    ToolboxWindow::Terminate();
    clearenemies();
}

void EnemyWindow::Draw(IDirect3DDevice9*)
{
    if (!visible) {
        return;
    }

    const bool is_in_outpost = GW::Map::GetInstanceType() == GW::Constants::InstanceType::Outpost;
    if (is_in_outpost) {
        clearenemies();
    }

    int enemy_count = 0;
    all_enemies.clear();

    const GW::AgentArray* agents = GW::Agents::GetAgentArray();
    const GW::Agent* player = agents ? GW::Agents::GetPlayer() : nullptr;

    if (player) {
        std::vector<enemyinfo>* enemyinfoarray = nullptr;
        std::set<GW::AgentID>* all_agents_of_type = nullptr;
        for (auto* agent : *agents) {
            const GW::AgentLiving* living = agent ? agent->GetAsAgentLiving() : nullptr;

            if (!living || living->allegiance != GW::Constants::Allegiance::Enemy || !living->GetIsAlive()) {
                continue;
            }

            switch (living->player_number) {
                case 2338:
                case 2325:
                    continue;
                default:
                    break;
            }

            all_agents_of_type = &all_enemies;
            enemyinfoarray = &enemies;
            enemy_count++;

            if (living->hp <= enemies_threshhold) {
                all_agents_of_type->insert(living->agent_id);

                const bool is_casting = living->skill != static_cast<uint16_t>(GW::Constants::SkillID::No_Skill);

                auto found_enemy = std::ranges::find_if(*enemyinfoarray, [living](const enemyinfo& info) {
                    return info.agent_id == living->agent_id;
                });
                if (found_enemy == enemyinfoarray->end()) {
                    enemyinfoarray->push_back(living->agent_id);
                    found_enemy = enemyinfoarray->end() - 1;
                }
                if (is_casting) {
                    found_enemy->last_casted = TIMER_INIT();
                    found_enemy->last_skill = living->skill;

                }

                found_enemy->distance = GW::GetSquareDistance(player->pos, living->pos);
            }
        }

        std::erase_if(enemies, [](auto& info) {
            return !all_enemies.contains(info.agent_id);
        });

        std::ranges::sort(enemies, &OrderEnemyInfo);

    }

    ImGui::SetNextWindowCenter(ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(Name(), GetVisiblePtr(), GetWinFlags())) {

        DrawDuping("enemies", enemies);
    }
    ImGui::End();
}

void EnemyWindow::DrawSettingsInternal()
{
    ImGui::DragFloat("Range", &range, 50, 0, 5000);
    ImGui::Separator();
    ImGui::Text("Enemy Counters:");
    ImGui::StartSpacedElements(275.f);
    ImGui::NextSpacedElement();
    ImGui::Checkbox("Show enemies", &show_enemies_counter);
    ImGui::Separator();
    ImGui::Text("HP thresholds:");
    ImGui::ShowHelp("Threshold HP below which enemy duping info is displayed");
    ImGui::DragFloat("Percent", &enemies_threshhold, 0.01f, 0, 1);
    ImGui::Separator();
    ImGui::Text("Status triange size multiplier:");
    ImGui::ShowHelp("Hex, condition, enchanted");
    ImGui::DragFloat("Multiplier", &triangleSizeMultiplier, 0.25f, 0, 5);
    ImGui::Separator();
    ImGui::Text("Status triange spacing");
    ImGui::DragFloat("Spacing", &triangleSpacing, 0.01, 0, 100);
}

void EnemyWindow::LoadSettings(ToolboxIni* ini)
{
    ToolboxWindow::LoadSettings(ini);
    LOAD_BOOL(show_enemies_counter);
    LOAD_FLOAT(enemies_threshhold);
    LOAD_FLOAT(range);
    LOAD_FLOAT(triangleSizeMultiplier);
    LOAD_FLOAT(triangleSpacing);
}

void EnemyWindow::SaveSettings(ToolboxIni* ini)
{
    ToolboxWindow::SaveSettings(ini);
    SAVE_BOOL(show_enemies_counter);
    SAVE_FLOAT(enemies_threshhold);
    SAVE_FLOAT(range);
    SAVE_FLOAT(triangleSizeMultiplier);
    SAVE_FLOAT(triangleSpacing);

}
