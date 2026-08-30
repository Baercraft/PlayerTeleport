#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "ScriptMgr.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"
#include "Chat.h"
#include "QueryResult.h"
#include "DatabaseEnv.h"
#include <unordered_map>
#include <string>

static bool g_ModuleEnabled = true;
static uint32 g_VipPriceGold = 10000;

struct SavedLocation
{
    uint32 mapId;
    float x;
    float y;
    float z;
    float o;
};

static std::unordered_map<ObjectGuid, SavedLocation> g_PreviousLocations;

enum SecretIslandGossip
{
    GOSSIP_ACTION_GO_SECRET   = 1,
    GOSSIP_ACTION_RETURN      = 2,
    GOSSIP_ACTION_CLOSE       = 3,
    GOSSIP_ACTION_BUY_VIP     = 4,
    GOSSIP_ACTION_GO_VIP      = 5,

    GOSSIP_ACTION_SECRET_BASE = 100
};

static void SavePlayerPosition(Player* player)
{
    SavedLocation loc;
    loc.mapId = player->GetMapId();
    loc.x = player->GetPositionX();
    loc.y = player->GetPositionY();
    loc.z = player->GetPositionZ();
    loc.o = player->GetOrientation();
    g_PreviousLocations[player->GetGUID()] = loc;
}

// Hilfsfunktion: Prüft, ob der Account VIP-Status besitzt
static bool IsAccountVip(uint32 accountId)
{
    QueryResult isVip = CharacterDatabase.Query("SELECT AccountId FROM vip WHERE active = 1 AND AccountId = {}", accountId);
    return isVip != nullptr;
}

class MallTeleportWorldScript : public WorldScript
{
public:
    MallTeleportWorldScript() : WorldScript("MallTeleportWorldScript") { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        g_ModuleEnabled = sConfigMgr->GetOption<bool>("MallTeleport.Enable", true);
        g_VipPriceGold  = sConfigMgr->GetOption<uint32>("VIP.PriceInGold", 10000);
    }
};

// ----------------------------------------------------------------
// NPC 1: Flussgeist (991105)
// ----------------------------------------------------------------
class npc_secret_island_teleporter : public CreatureScript
{
public:
    npc_secret_island_teleporter() : CreatureScript("npc_secret_island_teleporter") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!g_ModuleEnabled)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Das Teleport-Modul ist zurzeit deaktiviert.");
            return true;
        }

        ClearGossipMenuFor(player);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Bring mich zur Geheimen Festung!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_GO_SECRET);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Ich wollte doch nichts.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_CLOSE);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (!g_ModuleEnabled)
            return true;

        if (action == GOSSIP_ACTION_GO_SECRET)
        {
            if (player->IsInCombat())
            {
                CloseGossipMenuFor(player);
                ChatHandler(player->GetSession()).SendSysMessage("Du kannst dich im Kampf nicht teleportieren!");
                return true;
            }

            QueryResult result = WorldDatabase.Query("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM `game_tele` WHERE `name` = 'Geheim1'");
            if (result)
            {
                Field* fields = result->Fetch();
                uint32 mapId   = fields[0].Get<uint32>();
                float posX     = fields[1].Get<float>();
                float posY     = fields[2].Get<float>();
                float posZ     = fields[3].Get<float>();
                float orient   = fields[4].Get<float>();

                SavePlayerPosition(player);
                player->TeleportTo(mapId, posX, posY, posZ, orient);
            }
            else
            {
                ChatHandler(player->GetSession()).SendSysMessage("Dieser Teleportpunkt existiert noch nicht in der Datenbank!");
            }
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

// ----------------------------------------------------------------
// NPC 2: Rückkehr-Portal (991106)
// ----------------------------------------------------------------
class npc_secret_island_return : public CreatureScript
{
public:
    npc_secret_island_return() : CreatureScript("npc_secret_island_return") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!g_ModuleEnabled)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Das Teleport-Modul ist zurzeit deaktiviert.");
            return true;
        }

        ClearGossipMenuFor(player);

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Bring mich wieder zurueck!", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_RETURN);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Ich wollte doch nichts.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_CLOSE);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (!g_ModuleEnabled)
            return true;

        if (action == GOSSIP_ACTION_RETURN)
        {
            if (player->IsInCombat())
            {
                CloseGossipMenuFor(player);
                ChatHandler(player->GetSession()).SendSysMessage("Du kannst dich im Kampf nicht teleportieren!");
                return true;
            }

            auto it = g_PreviousLocations.find(player->GetGUID());
            if (it != g_PreviousLocations.end())
            {
                player->TeleportTo(it->second.mapId, it->second.x, it->second.y, it->second.z, it->second.o);
                g_PreviousLocations.erase(it);
            }
            else
            {
                if (player->GetTeamId() == TEAM_ALLIANCE)
                    player->TeleportTo(0, -8833.37f, 628.62f, 94.01f, 1.0f);
                else
                    player->TeleportTo(1, 1629.85f, -4373.59f, 31.55f, 3.6f);
            }
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

// ----------------------------------------------------------------
// NPC 3: Geheimniskraemer (991107) -> MIT VIP-Prüfung!
// ----------------------------------------------------------------
class npc_mystic_teleporter : public CreatureScript
{
public:
    npc_mystic_teleporter() : CreatureScript("npc_mystic_teleporter") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!g_ModuleEnabled)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Das Teleport-Modul ist zurzeit deaktiviert.");
            return true;
        }

        ClearGossipMenuFor(player);

        uint32 accountId = player->GetSession()->GetAccountId();

        if (!IsAccountVip(accountId))
        {
            ChatHandler(player->GetSession()).SendSysMessage("Ich spreche nur mit VIPs! Besorge dir erst den VIP-Rang bei Karl.");
            CloseGossipMenuFor(player);
            return true;
        }

        ChatHandler(player->GetSession()).SendSysMessage("Seid gegruesst... Ich kenne zehn Uralte, verborgene Orte. Wohin soll die Reise gehen?");

        for (uint8 i = 1; i <= 10; ++i)
        {
            std::string label = "Reise nach secret" + (i < 10 ? std::string("0") : std::string("")) + std::to_string(i);
            AddGossipItemFor(player, GOSSIP_ICON_TABARD, label, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_SECRET_BASE + i);
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Ich wollte doch nichts.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_CLOSE);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (!g_ModuleEnabled)
            return true;

        uint32 accountId = player->GetSession()->GetAccountId();
        if (!IsAccountVip(accountId))
            return true;

        if (action >= GOSSIP_ACTION_SECRET_BASE + 1 && action <= GOSSIP_ACTION_SECRET_BASE + 10)
        {
            if (player->IsInCombat())
            {
                CloseGossipMenuFor(player);
                ChatHandler(player->GetSession()).SendSysMessage("Du kannst dich im Kampf nicht teleportieren!");
                return true;
            }

            uint8 secretIndex = static_cast<uint8>(action - GOSSIP_ACTION_SECRET_BASE);
            std::string teleName = "secret" + (secretIndex < 10 ? std::string("0") : std::string("")) + std::to_string(secretIndex);

            QueryResult result = WorldDatabase.Query("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM `game_tele` WHERE `name` = '{}'", teleName);
            if (result)
            {
                Field* fields = result->Fetch();
                uint32 mapId   = fields[0].Get<uint32>();
                float posX     = fields[1].Get<float>();
                float posY     = fields[2].Get<float>();
                float posZ     = fields[3].Get<float>();
                float orient   = fields[4].Get<float>();

                SavePlayerPosition(player);
                player->TeleportTo(mapId, posX, posY, posZ, orient);
            }
            else
            {
                ChatHandler(player->GetSession()).SendSysMessage("Dieser Teleportpunkt existiert noch nicht in der Datenbank!");
            }
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

// ----------------------------------------------------------------
// NPC 4: Karl (991108) -> Verkaufen & Teleportieren
// ----------------------------------------------------------------
class npc_vip_master : public CreatureScript
{
public:
    npc_vip_master() : CreatureScript("npc_vip_master") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!g_ModuleEnabled)
        {
            ChatHandler(player->GetSession()).SendSysMessage("Das Teleport-Modul ist zurzeit deaktiviert.");
            return true;
        }

        ClearGossipMenuFor(player);

        uint32 accountId = player->GetSession()->GetAccountId();

        if (IsAccountVip(accountId))
        {
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, "Bring mich zum VIP Bereich", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_GO_VIP);
        }
        else
        {
            std::string msg = "Hallo! Fuer " + std::to_string(g_VipPriceGold) + " Gold schalte ich deinen gesamten Account als VIP frei!";
            ChatHandler(player->GetSession()).SendSysMessage(msg.c_str());

            std::string label = "VIP Rang kaufen (" + std::to_string(g_VipPriceGold) + " Gold)";
            std::string confirm = "Moechtest du wirklich den VIP-Rang fuer " + std::to_string(g_VipPriceGold) + " Gold freischalten?";

            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, label, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_BUY_VIP, confirm, 0, false);
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Ich wollte doch nichts.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_CLOSE);

        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (!g_ModuleEnabled)
            return true;

        if (action == GOSSIP_ACTION_BUY_VIP)
        {
            uint32 accountId = player->GetSession()->GetAccountId();
            uint32 vipPriceCopper = g_VipPriceGold * 10000;

            if (!player->HasEnoughMoney(static_cast<uint32>(vipPriceCopper)))
            {
                std::string errorMsg = "Du hast nicht genuegend Gold! Der VIP-Rang kostet " + std::to_string(g_VipPriceGold) + " Gold.";
                ChatHandler(player->GetSession()).SendSysMessage(errorMsg.c_str());
                CloseGossipMenuFor(player);
                return true;
            }

            player->ModifyMoney(-static_cast<int32>(vipPriceCopper));
            CharacterDatabase.Execute("REPLACE INTO vip (AccountId, active) VALUES ({}, 1)", accountId);

            player->CastSpell(player, 63711, true);
            
            std::string successMsg = "|cff00ff00Glueckwunsch! Dein Account hat jetzt VIP-Status!|r";
            ChatHandler(player->GetSession()).SendSysMessage(successMsg.c_str());
        }
        else if (action == GOSSIP_ACTION_GO_VIP)
        {
            if (player->IsInCombat())
            {
                CloseGossipMenuFor(player);
                ChatHandler(player->GetSession()).SendSysMessage("Du kannst dich im Kampf nicht teleportieren!");
                return true;
            }

            QueryResult result = WorldDatabase.Query("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM `game_tele` WHERE `name` = 'VIP'");
            if (result)
            {
                Field* fields = result->Fetch();
                uint32 mapId   = fields[0].Get<uint32>();
                float posX     = fields[1].Get<float>();
                float posY     = fields[2].Get<float>();
                float posZ     = fields[3].Get<float>();
                float orient   = fields[4].Get<float>();

                SavePlayerPosition(player);
                player->TeleportTo(mapId, posX, posY, posZ, orient);
            }
            else
            {
                ChatHandler(player->GetSession()).SendSysMessage("Der Teleportpunkt 'VIP' existiert noch nicht in der Datenbank!");
            }
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

// ----------------------------------------------------------------
// Wichtig: Namensanpassung für das Modul-Loader-Script!
// ----------------------------------------------------------------
void AddMallTeleportScripts()
{
    new MallTeleportWorldScript();
    new npc_secret_island_teleporter();
    new npc_secret_island_return();
    new npc_mystic_teleporter();
    new npc_vip_master();
}