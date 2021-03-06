#include "stdafx.h"

#include <algorithm>


#include <GWCA/Packets/StoC.h>

#include <GWCA/Context/GameContext.h>
#include <GWCA/Context/CharContext.h>

#include <GWCA/GameEntities/Player.h>
#include <GWCA/GameEntities/Item.h>

#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/StoCMgr.h>
#include <GWCA/Managers/CtoSMgr.h>
#include <GWCA/Managers/AgentMgr.h>
#include <GWCA/Managers/MapMgr.h>
#include <GWCA/Managers/GameThreadMgr.h>
#include <GWCA/Managers/ItemMgr.h>
#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/UIMgr.h>

#include <GuiUtils.h>
#include <Logger.h>
#include <ImGuiAddons.h>
#include <Modules/Obfuscator.h>


#ifndef GAME_SMSG_MERCENARY_INFO
#define GAME_SMSG_MERCENARY_INFO 115
#endif
#include <Psapi.h>


namespace {
    /*IWbemServices* pSvc = 0;
    IWbemLocator* pLoc = 0;
    HRESULT CoInitializeEx_result = -1;*/
    MSG msg;
    HWND streaming_window_handle = 0;
    std::default_random_engine dre = std::default_random_engine((uint32_t)time(0));
    HWINEVENTHOOK hook = 0;
    bool running = false;

    const wchar_t* getPlayerName() {
        GW::GameContext* g = GW::GameContext::instance();
        return g ? g->character->player_name : nullptr;
    }

    std::vector<std::vector<const wchar_t*>> replace_tokens_by_channel;

    std::vector<const wchar_t*> obfuscated_name_pool = {
        L"Abbot Ramoth",
        L"Acolyte Of Balthazar",
        L"Acolyte Of Dwayna",
        L"Acolyte Of Grenth",
        L"Acolyte Of Lyssa",
        L"Acolyte Of Melandru",
        L"Admiral Chiggen",
        L"Admiral Kantoh",
        L"Admiral Kaya",
        L"Ajun Xi Deft Blade",
        L"Am Fah Courier",
        L"Am Fah Leader",
        L"Apep Unending Night",
        L"Argo",
        L"Arius Dark Apostle",
        L"Ashlyn Spiderfriend",
        L"Auri The Skull Wand",
        L"Aurora",
        L"Bairn The Sinless",
        L"Bearn The Implacable",
        L"Bonetti",
        L"Bosun Mohrti",
        L"Braima The Callous",
        L"Brogan The Punisher",
        L"Calamitous",
        L"Captain Alsin",
        L"Captain Bei Chi",
        L"Captain Blood Farid",
        L"Captain Chichor",
        L"Captain Denduru",
        L"Captain Kavaka",
        L"Captain Kuruk",
        L"Captain Lumanda",
        L"Captain Mhedi",
        L"Captain Mwende",
        L"Captain Nebo",
        L"Captain Shehnahr",
        L"Carnak The Hungry",
        L"Cerris",
        L"Chae Plan",
        L"Chazek Plague Herder",
        L"Cho Spirit Empath",
        L"Chung The Attuned",
        L"Colonel Chaklin",
        L"Colonel Custo",
        L"Colonel Kajo",
        L"Commander Bahreht",
        L"Commander Chui Kantu",
        L"Commander Kubeh",
        L"Commander Noss",
        L"Commander Sadi-Belai",
        L"Commander Sehden",
        L"Commander Wahli",
        L"Commander Werishakul",
        L"Confessor Isaiah",
        L"Corbin The Upright",
        L"Corporal Argon",
        L"Corporal Luluh",
        L"Corporal Suli",
        L"Corsair Commander",
        L"Countess Nadya",
        L"Cultist Milthuran",
        L"Cultist Rajazan",
        L"Cursed Salihm",
        L"Cuthbert The Chaste",
        L"Daeman",
        L"Daisuke Crimson Edge",
        L"Danthor The Adamant",
        L"Darwym The Spiteful",
        L"Degaz The Cynical",
        L"Dim Sii",
        L"Dimsur Cheefai",
        L"Drinkmaster Tahnu",
        L"Edgar The Iron Fist",
        L"Edred The Bruiser",
        L"Elvina The Pious",
        L"En Fa The Awakened",
        L"Ensign Charehli",
        L"Ensign Jahan",
        L"Ensign Lumi",
        L"Eri Heart Of Fire",
        L"Erulai The Inimical",
        L"Fo Mahn",
        L"Galrath",
        L"Ganshu The Scribe",
        L"Gao Han Of The Rings",
        L"Garr The Merciful",
        L"General Kahyet",
        L"Geoffer Pain Bringer",
        L"Gilroy The Stoic",
        L"Gowan Chobak",
        L"Heifan Kanko",
        L"Imuk The Pungent",
        L"Initiate Jeng Sunjoo",
        L"Initiate Shen Wojong",
        L"Inquisitor Bauer",
        L"Inquisitor Lashona",
        L"Inquisitor Lovisa",
        L"Insatiable Vakar",
        L"Ironfist",
        L"Irwyn The Severe",
        L"Jacqui The Reaver",
        L"Jang Wen",
        L"Jiao Kuai The Swift",
        L"Jin The Skull Bow",
        L"Jin The Purifier",
        L"Joh The Hostile",
        L"Julen The Devout",
        L"Justiciar Amilyn",
        L"Justiciar Hablion",
        L"Justiciar Kasandra",
        L"Justiciar Kimii",
        L"Justiciar Marron",
        L"Justiciar Sevaan",
        L"Kahli, The Stiched",
        L"Kai Shi Jo",
        L"Kathryn The Cold",
        L"Kayali The Brave",
        L"Kenric The Believer",
        L"Kenshi Steelhand",
        L"Koon Jizang",
        L"Lai Graceful Blade",
        L"Lale The Vindictive",
        L"Lars The Obeisant",
        L"Leijun Ano",
        L"Lerita The Lewd",
        L"Lev The Condemned",
        L"Li Ho Yan",
        L"Liam Shanglui",
        L"Lian Dragons Petal",
        L"Lieutenant Kayin",
        L"Lieutenant Mahrik",
        L"Lieutenant Nali",
        L"Lieutenant Shagu",
        L"Lieutenant Silmok",
        L"Lieutenant Vanahk",
        L"Lorelle Jade Cutter",
        L"Lou Of The Knives",
        L"Major Jeahr",
        L"Manton The Indulgent",
        L"Markis",
        L"Marnta Doomspeaker",
        L"Maxine Coldstone",
        L"Meijun Vengeful Eye",
        L"Merki The Reaver",
        L"Midshipman Bennis",
        L"Midshipman Beraidun",
        L"Midshipman Morolah",
        L"Mina Shatter Storm",
        L"Minea The Obscene",
        L"Ming The Judgment",
        L"Oswald The Amiable",
        L"Overseer Boktek",
        L"Overseer Haubeh",
        L"Pah Pei",
        L"Pei The Skull Blade",
        L"Pleoh The Ugly",
        L"Quufu",
        L"Ramm The Benevolent",
        L"Rei Bi",
        L"Reiko",
        L"Reisen The Phoenix",
        L"Rho Ki",
        L"Rien The Martyr",
        L"Riseh The Harmless",
        L"Royen Beastkeeper",
        L"Samira Dhulnarim",
        L"Seaguard Eli",
        L"Seaguard Gita",
        L"Seaguard Hala",
        L"Seiran Of The Cards",
        L"Selenas The Blunt",
        L"Selwin The Fervent",
        L"Sergeant Behnwa",
        L"Shen The Magistrate",
        L"Sheng Pai",
        L"Shensang Jinzao",
        L"Shiro Tagachi",
        L"Sun The Quivering",
        L"Suuga Rei",
        L"Suunshi Haisang",
        L"Tachi Forvent",
        L"Tai Soon",
        L"Talous The Mad",
        L"Taskmaster Suli",
        L"Taskmaster Vanahk",
        L"Teral The Punisher",
        L"The Dark Blade",
        L"Torr The Relentless",
        L"Tuila The Club",
        L"Uris Tong Of Ash",
        L"Valis The Rampant",
        L"Ven The Conservator",
        L"Vess The Disputant",
        L"Waeng",
        L"Watari The Infinite",
        L"Wing Three Blade",
        L"Xi Lin Of The Flames",
        L"Xien",
        L"Yayoi Of The Orders",
        L"Yinnai Qi",
        L"Zaln The Jaded",
        L"Zu Jin The Quick"
    };
}
void CALLBACK Obfuscator::OnWindowEvent(HWINEVENTHOOK _hook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime) {
     if (!running)
         return;
     UNREFERENCED_PARAMETER(_hook);
     UNREFERENCED_PARAMETER(dwEventThread);
     UNREFERENCED_PARAMETER(dwmsEventTime);
     switch (event) {
     case EVENT_OBJECT_DESTROY:
         if (hwnd != streaming_window_handle)
             return;
         streaming_window_handle = 0;
         Log::Info("Streaming mode deactivated");
         break;
     case EVENT_SYSTEM_FOREGROUND: {
         if (streaming_window_handle)
             return;
         TCHAR window_class_name[MAX_PATH] = { 0 };
         DWORD dwProcId = 0;

         GetWindowThreadProcessId(hwnd, &dwProcId);

         HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcId);
         if (!hProc) {
             Log::Log("Failed to OpenProcess, %d", GetLastError());
             return;
         }
         DWORD res = GetModuleFileNameExA((HMODULE)hProc, NULL, window_class_name, MAX_PATH);
         CloseHandle(hProc);
         if (!res) {
             Log::Log("Failed to GetModuleFileNameExA, %d", GetLastError());
             return;
         }
         if (strstr(window_class_name, "obs64.exe")) {
             streaming_window_handle = hwnd;
             Log::Info("Streaming mode activated");
         }
         Log::Log("%p, %p, %p, %p - %s", event, hwnd, idObject, idChild, window_class_name);
     } break;
     }
}
void Obfuscator::OnSendChat(GW::HookStatus* status, GW::Chat::Channel channel, wchar_t* message) {
    if (channel != GW::Chat::Channel::CHANNEL_WHISPER)
        return;
    if (Instance().status != Enabled)
        return;
    static bool processing = false;
    if (processing)
        return;
    wchar_t* whisper_separator = wcschr(message, ',');
    if (!whisper_separator)
        return;
    size_t len = (whisper_separator - message);
    wchar_t unobfuscated[32] = { 0 };
    wcsncpy(unobfuscated, message, len);
    Instance().UnobfuscateName(unobfuscated, unobfuscated, _countof(unobfuscated));
    wchar_t new_message[138] = { 0 };
    swprintf(new_message, _countof(new_message), L"%s%s", unobfuscated, whisper_separator);
    status->blocked = true;
    // NB: Block and send a copy with the new message content; current wchar_t* may not have enough allocated memory to just replace the content.
    processing = true;
    GW::Chat::SendChat(unobfuscated, &whisper_separator[1]);
    processing = false;
}
wchar_t* Obfuscator::ObfuscateMessage(GW::Chat::Channel channel, wchar_t* message, bool obfuscate) {
    UNREFERENCED_PARAMETER(channel);
    static wchar_t new_message[1024];
    if (channel == GW::Chat::Channel::CHANNEL_GWCA2) {
        wchar_t* link_start = wcsstr(message, L"<a=1>");
        if (link_start) {
            link_start += 5;
            wchar_t* link_end = wcsstr(link_start, L"</a>");
            wchar_t new_name[32] = { 0 };
            wcsncpy(new_name, link_start, link_end - link_start);
            return message;
        }
    }

    auto& replace_tokens = replace_tokens_by_channel[channel];
    for (size_t i = 0; i < replace_tokens.size(); i++) {
        size_t token_len = wcslen(replace_tokens[i]);
        if (wcsncmp(message, replace_tokens[i], token_len) != 0)
            continue;
        wchar_t* whisper_separator = wcschr(&message[token_len], '\x1');
        if (!whisper_separator)
            break;
        size_t len = (whisper_separator - message - token_len);
        // NB: Static for ease, but be sure copy this away before accessing this function again as needed
        wchar_t new_name[32] = { 0 };
        wcsncpy(new_name, &message[token_len], len);
        if (obfuscate) {
            ObfuscateName(new_name, new_name, _countof(new_name));
        }
        else {
            UnobfuscateName(new_name, new_name, _countof(new_name));
        }
        int written = 0;
        if (message == new_message) {
            // Avoid recursive calls to this function causing borked up pointers.
            wchar_t* latter_message = new wchar_t[wcslen(whisper_separator) + 1];
            wcscpy(latter_message, whisper_separator);
            written = swprintf(new_message, _countof(new_message) - 1, L"%s%s%s", replace_tokens[i], new_name, latter_message);
        }
        else {
            written = swprintf(new_message, _countof(new_message) - 1, L"%s%s%s", replace_tokens[i], new_name, whisper_separator);
        }
        new_message[written] = 0;
        return new_message;
    }
    return message;
}
void Obfuscator::OnPrintChat(GW::HookStatus* , GW::Chat::Channel channel, wchar_t** message_ptr, FILETIME, int) {
    if (Instance().status != Enabled)
        return;
    *message_ptr = Instance().ObfuscateMessage(channel, *message_ptr);
}
void Obfuscator::OnPreUIMessage(GW::HookStatus*, uint32_t msg_id, void* wParam, void* ) {
    if (Instance().status != Enabled)
        return;
    switch (msg_id) {
    case GW::UI::UIMessage::kWriteToChatLog: {
        struct PlayerChatMessage {
            GW::Chat::Channel channel;
            wchar_t* message;
            uint32_t player_number;
        } *packet_actual = (PlayerChatMessage*) wParam;
        packet_actual->message = Instance().UnobfuscateMessage(packet_actual->channel, packet_actual->message);
        // Unobfuscate player name for incoming UI messages; we obfuscate it when the message is printed on-screen
        UNREFERENCED_PARAMETER(packet_actual);
    } break;
    }
}
void Obfuscator::OnPostUIMessage(GW::HookStatus*, uint32_t , void* , void* ) {
    // TODO: Re-obfuscate player name.
}
void Obfuscator::OnStoCPacket(GW::HookStatus*, GW::Packet::StoC::PacketBase* packet) {
    if (packet->header == GAME_SMSG_TRANSFER_GAME_SERVER_INFO) {
        Instance().Reset();
        if (Instance().status == Pending)
        Instance().status = Enabled;
        return;
    }
    if (Instance().status != Enabled)
        return;
    switch (packet->header) {
    // Hide Player name on spawn
    case GAME_SMSG_AGENT_CREATE_PLAYER: {
        struct Packet {
            uint32_t chaff[7];
            wchar_t name[32];
        } *packet_actual = (Packet*)packet;
        Instance().ObfuscateName(packet_actual->name, packet_actual->name, _countof(packet_actual->name) - 1);
        return;
    } break;
    // Hide Mercenary Hero name
    case GAME_SMSG_MERCENARY_INFO: {
        struct Packet {
            uint32_t chaff[20];
            wchar_t name[32];
        } *packet_actual = (Packet*)packet;
        Instance().ObfuscateName(packet_actual->name, packet_actual->name, _countof(packet_actual->name) - 1);
    } break;
    // Hide Mercenary Hero name after being added to party or in explorable area
    case GAME_SMSG_AGENT_UPDATE_NPC_NAME: {
        struct Packet {
            uint32_t chaff[2];
            wchar_t name[32];
        } *packet_actual = (Packet*)packet;
        if (wcsncmp(L"\x108\x107", packet_actual->name, 2) != 0)
            return; // Not a mercenary name
        wchar_t original_name[20];
        size_t len = 0;
        for (size_t i = 2; i < _countof(original_name) - 1; i++) {
            if (!packet_actual->name[i] || packet_actual->name[i] == 0x1)
                break;
            original_name[len] = packet_actual->name[i];
            len++;
        }
        original_name[len] = 0;
        Instance().ObfuscateName(original_name, &packet_actual->name[2], _countof(packet_actual->name) - 4);
        len = wcslen(packet_actual->name);
        packet_actual->name[len] = 0x1;
        packet_actual->name[len+1] = 0;
    } break;
    // Hide "Customised for <player_name>". Packet header is poorly named, this is actually something like GAME_SMSG_ITEM_CUSTOMISED_NAME
    case GAME_SMSG_ITEM_UPDATE_NAME: {
        struct Packet {
            uint32_t chaff[2];
            wchar_t name[32];
        } *packet_actual = (Packet*)packet;
        Instance().ObfuscateName(packet_actual->name, packet_actual->name, _countof(packet_actual->name));
    } break;
    // Hide guild member info
    case GAME_SMSG_GUILD_PLAYER_INFO: {
        struct Packet {
            uint32_t header;
            wchar_t account_name[20];
            wchar_t player_name[20];
        } *packet_actual = (Packet*)packet;
        // Only bother to obfuscate the current player's name
        if(wcscmp(packet_actual->player_name, getPlayerName()) == 0)
            Instance().ObfuscateName(packet_actual->player_name, packet_actual->player_name, _countof(packet_actual->player_name));
    } break;
    }
}
wchar_t* Obfuscator::ObfuscateName(const wchar_t* _original_name, wchar_t* out, int length) {
    std::wstring original_name = GuiUtils::SanitizePlayerName(_original_name);
    auto found = obfuscated_by_original.find(original_name);
    if (found != obfuscated_by_original.end())
        return wcsncpy(out, found->second.c_str(), length);
    const wchar_t* res = obfuscated_name_pool[pool_index];
    pool_index++;
    if (pool_index >= obfuscated_name_pool.size())
        pool_index = 0;
    swprintf(out, length, L"%s", res);
    for(size_t cnt = 0;cnt < 100;cnt++) {
        found = obfuscated_by_obfuscation.find(out);
        if (found == obfuscated_by_obfuscation.end()) {
            obfuscated_by_obfuscation.emplace(out, original_name.c_str());
            obfuscated_by_original.emplace(original_name.c_str(), out);
            break;
        }
        ASSERT(swprintf(out, length, L"%.16s %d", res, cnt) != -1);
    }
    ASSERT(out[0]);
    return out;
}
wchar_t* Obfuscator::UnobfuscateName(const wchar_t* _obfuscated_name, wchar_t* out, int length) {
    std::wstring obfuscated_name = GuiUtils::SanitizePlayerName(_obfuscated_name);
    auto found = obfuscated_by_obfuscation.find(obfuscated_name);
    if (found != obfuscated_by_obfuscation.end())
        return wcsncpy(out, found->second.c_str(), length);
    return out == _obfuscated_name ? out : wcsncpy(out, _obfuscated_name, length);
}
Obfuscator::~Obfuscator() {
    Reset();
    Terminate();
}
void Obfuscator::Terminate() {
    /*if (pSvc) {
        pSvc->Release();
        pSvc = 0;
    }
    if (pLoc) {
        pLoc->Release();
        pLoc = 0;
    }
    if (!SUCCEEDED(CoInitializeEx_result)) {
        CoUninitialize();
        CoInitializeEx_result = -1;
    }*/
    running = false;
    if (hook) {
        ASSERT(UnhookWinEvent(hook));
        CoUninitialize();
        hook = 0;
    }
}
void Obfuscator::Initialize() {
    ToolboxModule::Initialize();
    Reset();

    replace_tokens_by_channel.resize(GW::Chat::Channel::CHANNEL_COUNT);
    replace_tokens_by_channel[GW::Chat::Channel::CHANNEL_ALLIANCE] = { L"\x76b\x10a\x913\x107" };
    replace_tokens_by_channel[GW::Chat::Channel::CHANNEL_GUILD] = { L"\x76b\x107" };
    replace_tokens_by_channel[GW::Chat::Channel::CHANNEL_ALLIES] = { L"\x76b\x10a\x108\x107" };
    replace_tokens_by_channel[GW::Chat::Channel::CHANNEL_ALL] = { L"\x76b\x10a\x108\x107" };
    replace_tokens_by_channel[GW::Chat::Channel::CHANNEL_GROUP] = { L"\x76b\x10a\x108\x107" };
    replace_tokens_by_channel[GW::Chat::Channel::CHANNEL_WHISPER] = {
        L"\x076d\x0107",            // Incoming whisper
    };
    replace_tokens_by_channel[GW::Chat::Channel::CHANNEL_GLOBAL] = {
        L"\x76e\x101\x100\x107",    // Outgoing whisper
    };
    GW::StoC::RegisterPacketCallback(&stoc_hook, GAME_SMSG_AGENT_CREATE_PLAYER, OnStoCPacket);
    GW::StoC::RegisterPacketCallback(&stoc_hook, GAME_SMSG_MERCENARY_INFO, OnStoCPacket);
    GW::StoC::RegisterPacketCallback(&stoc_hook, GAME_SMSG_AGENT_UPDATE_NPC_NAME, OnStoCPacket);
    GW::StoC::RegisterPacketCallback(&stoc_hook, GAME_SMSG_ITEM_UPDATE_NAME, OnStoCPacket);
    GW::StoC::RegisterPacketCallback(&stoc_hook, GAME_SMSG_TRANSFER_GAME_SERVER_INFO, OnStoCPacket);
    GW::StoC::RegisterPacketCallback(&stoc_hook, GAME_SMSG_GUILD_PLAYER_INFO, OnStoCPacket);

    GW::UI::RegisterUIMessageCallback(&stoc_hook, OnPreUIMessage, -0x9000);
    GW::UI::RegisterUIMessageCallback(&stoc_hook, OnPostUIMessage, 0x8000);

    GW::Chat::RegisterSendChatCallback(&ctos_hook, OnSendChat);

    GW::Chat::RegisterPrintChatCallback(&ctos_hook, OnPrintChat);

    CoInitialize(NULL);
    hook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_OBJECT_DESTROY, NULL, OnWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    running = true;
}
void Obfuscator::Update(float) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
void Obfuscator::Reset() {
    std::shuffle(std::begin(obfuscated_name_pool), std::end(obfuscated_name_pool), dre);
    pool_index = 0;
    obfuscated_by_obfuscation.clear();
    obfuscated_by_original.clear();
}
void Obfuscator::DrawSettingInternal() {
    static bool enabled = status != Disabled;
    if (ImGui::Checkbox("Obfuscate player names", &enabled)) {
        status = enabled ? Pending : Disabled;
        Log::Info("Setting will be applied on next map change");
    }
    ImGui::ShowHelp("Replace player names in-game with aliases.\nThis change is applied on next map change.");
}
