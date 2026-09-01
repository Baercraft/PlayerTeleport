#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "ScriptMgr.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"
#include "Chat.h"
#include "QueryResult.h"
#include "DatabaseEnv.h"
#include "WorldSession.h"
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

enum class MallText
{
    ModuleDisabled,
    GoSecretFortress,
    Cancel,
    InCombat,
    TeleportMissing,
    Return,
    VipOnly,
    MysticGreeting,
    TravelTo,
    GoVipArea,
    VipOfferPrefix,
    VipOfferSuffix,
    BuyVipPrefix,
    BuyVipSuffix,
    ConfirmVipPrefix,
    ConfirmVipSuffix,
    NotEnoughGoldPrefix,
    NotEnoughGoldSuffix,
    VipSuccess,
    VipTeleportMissing
};

// Localized text is selected from the language of the connected WoW client.
// Unsupported locales fall back to English.
static char const* GetMallText(Player const* player, MallText text)
{
    LocaleConstant locale = LOCALE_enUS;
    if (player && player->GetSession())
        locale = player->GetSession()->GetSessionDbcLocale();

    switch (locale)
    {
        case LOCALE_deDE:
            switch (text)
            {
                case MallText::ModuleDisabled:        return "Das Teleport-Modul ist zurzeit deaktiviert.";
                case MallText::GoSecretFortress:      return "Bring mich zur Geheimen Festung!";
                case MallText::Cancel:                return "Ich wollte doch nichts.";
                case MallText::InCombat:              return "Du kannst dich im Kampf nicht teleportieren!";
                case MallText::TeleportMissing:       return "Dieser Teleportpunkt existiert noch nicht in der Datenbank!";
                case MallText::Return:                return "Bring mich wieder zurück!";
                case MallText::VipOnly:               return "Ich spreche nur mit VIPs! Besorge dir erst den VIP-Rang bei Karl.";
                case MallText::MysticGreeting:        return "Seid gegrüßt... Ich kenne zehn uralte, verborgene Orte. Wohin soll die Reise gehen?";
                case MallText::TravelTo:              return "Reise nach ";
                case MallText::GoVipArea:             return "Bring mich zum VIP-Bereich";
                case MallText::VipOfferPrefix:        return "Hallo! Für ";
                case MallText::VipOfferSuffix:        return " Gold schalte ich deinen gesamten Account als VIP frei!";
                case MallText::BuyVipPrefix:          return "VIP-Rang kaufen (";
                case MallText::BuyVipSuffix:          return " Gold)";
                case MallText::ConfirmVipPrefix:      return "Möchtest du wirklich den VIP-Rang für ";
                case MallText::ConfirmVipSuffix:      return " Gold freischalten?";
                case MallText::NotEnoughGoldPrefix:   return "Du hast nicht genügend Gold! Der VIP-Rang kostet ";
                case MallText::NotEnoughGoldSuffix:   return " Gold.";
                case MallText::VipSuccess:            return "|cff00ff00Glückwunsch! Dein Account hat jetzt VIP-Status!|r";
                case MallText::VipTeleportMissing:    return "Der Teleportpunkt 'VIP' existiert noch nicht in der Datenbank!";
            }
            break;

        case LOCALE_esES:
        case LOCALE_esMX:
            switch (text)
            {
                case MallText::ModuleDisabled:        return "El módulo de teletransporte está desactivado actualmente.";
                case MallText::GoSecretFortress:      return "¡Llévame a la Fortaleza Secreta!";
                case MallText::Cancel:                return "No importa, gracias.";
                case MallText::InCombat:              return "¡No puedes teletransportarte durante el combate!";
                case MallText::TeleportMissing:       return "¡Este punto de teletransporte todavía no existe en la base de datos!";
                case MallText::Return:                return "¡Llévame de vuelta!";
                case MallText::VipOnly:               return "¡Solo hablo con VIP! Consigue primero el rango VIP con Karl.";
                case MallText::MysticGreeting:        return "Saludos... Conozco diez lugares antiguos y ocultos. ¿A dónde quieres viajar?";
                case MallText::TravelTo:              return "Viajar a ";
                case MallText::GoVipArea:             return "Llévame a la zona VIP";
                case MallText::VipOfferPrefix:        return "¡Hola! Por ";
                case MallText::VipOfferSuffix:        return " de oro activaré el estado VIP para toda tu cuenta.";
                case MallText::BuyVipPrefix:          return "Comprar rango VIP (";
                case MallText::BuyVipSuffix:          return " de oro)";
                case MallText::ConfirmVipPrefix:      return "¿Seguro que quieres desbloquear el rango VIP por ";
                case MallText::ConfirmVipSuffix:      return " de oro?";
                case MallText::NotEnoughGoldPrefix:   return "¡No tienes suficiente oro! El rango VIP cuesta ";
                case MallText::NotEnoughGoldSuffix:   return " de oro.";
                case MallText::VipSuccess:            return "|cff00ff00¡Felicidades! ¡Tu cuenta ahora tiene estado VIP!|r";
                case MallText::VipTeleportMissing:    return "¡El punto de teletransporte 'VIP' todavía no existe en la base de datos!";
            }
            break;

        case LOCALE_frFR:
            switch (text)
            {
                case MallText::ModuleDisabled:        return "Le module de téléportation est actuellement désactivé.";
                case MallText::GoSecretFortress:      return "Emmène-moi à la Forteresse Secrète !";
                case MallText::Cancel:                return "Finalement, ce n'est rien.";
                case MallText::InCombat:              return "Vous ne pouvez pas vous téléporter pendant un combat !";
                case MallText::TeleportMissing:       return "Ce point de téléportation n'existe pas encore dans la base de données !";
                case MallText::Return:                return "Ramène-moi !";
                case MallText::VipOnly:               return "Je ne parle qu'aux VIP ! Obtenez d'abord le rang VIP auprès de Karl.";
                case MallText::MysticGreeting:        return "Salutations... Je connais dix lieux anciens et cachés. Où souhaitez-vous aller ?";
                case MallText::TravelTo:              return "Voyager vers ";
                case MallText::GoVipArea:             return "Emmène-moi dans la zone VIP";
                case MallText::VipOfferPrefix:        return "Bonjour ! Pour ";
                case MallText::VipOfferSuffix:        return " pièces d'or, j'activerai le statut VIP pour tout votre compte !";
                case MallText::BuyVipPrefix:          return "Acheter le rang VIP (";
                case MallText::BuyVipSuffix:          return " pièces d'or)";
                case MallText::ConfirmVipPrefix:      return "Voulez-vous vraiment débloquer le rang VIP pour ";
                case MallText::ConfirmVipSuffix:      return " pièces d'or ?";
                case MallText::NotEnoughGoldPrefix:   return "Vous n'avez pas assez d'or ! Le rang VIP coûte ";
                case MallText::NotEnoughGoldSuffix:   return " pièces d'or.";
                case MallText::VipSuccess:            return "|cff00ff00Félicitations ! Votre compte possède maintenant le statut VIP !|r";
                case MallText::VipTeleportMissing:    return "Le point de téléportation 'VIP' n'existe pas encore dans la base de données !";
            }
            break;

        case LOCALE_ruRU:
            switch (text)
            {
                case MallText::ModuleDisabled:        return "Модуль телепортации сейчас отключен.";
                case MallText::GoSecretFortress:      return "Отправь меня в Тайную крепость!";
                case MallText::Cancel:                return "Нет, ничего не нужно.";
                case MallText::InCombat:              return "Нельзя телепортироваться во время боя!";
                case MallText::TeleportMissing:       return "Эта точка телепортации еще не существует в базе данных!";
                case MallText::Return:                return "Верни меня обратно!";
                case MallText::VipOnly:               return "Я разговариваю только с VIP! Сначала получи VIP-статус у Карла.";
                case MallText::MysticGreeting:        return "Приветствую... Я знаю десять древних скрытых мест. Куда отправимся?";
                case MallText::TravelTo:              return "Путешествие в ";
                case MallText::GoVipArea:             return "Отправь меня в VIP-зону";
                case MallText::VipOfferPrefix:        return "Привет! За ";
                case MallText::VipOfferSuffix:        return " золота я открою VIP-статус для всей учетной записи!";
                case MallText::BuyVipPrefix:          return "Купить VIP-статус (";
                case MallText::BuyVipSuffix:          return " золота)";
                case MallText::ConfirmVipPrefix:      return "Вы действительно хотите открыть VIP-статус за ";
                case MallText::ConfirmVipSuffix:      return " золота?";
                case MallText::NotEnoughGoldPrefix:   return "Недостаточно золота! VIP-статус стоит ";
                case MallText::NotEnoughGoldSuffix:   return " золота.";
                case MallText::VipSuccess:            return "|cff00ff00Поздравляем! Ваша учетная запись теперь имеет VIP-статус!|r";
                case MallText::VipTeleportMissing:    return "Точка телепортации 'VIP' еще не существует в базе данных!";
            }
            break;

        case LOCALE_zhCN:
            switch (text)
            {
                case MallText::ModuleDisabled:        return "传送模块目前已禁用。";
                case MallText::GoSecretFortress:      return "带我去秘密要塞！";
                case MallText::Cancel:                return "算了，我不需要。";
                case MallText::InCombat:              return "战斗中无法传送！";
                case MallText::TeleportMissing:       return "数据库中还没有这个传送点！";
                case MallText::Return:                return "带我回去！";
                case MallText::VipOnly:               return "我只和VIP交谈！请先从Karl那里获得VIP资格。";
                case MallText::MysticGreeting:        return "欢迎……我知道十个古老而隐秘的地点。你想去哪里？";
                case MallText::TravelTo:              return "前往 ";
                case MallText::GoVipArea:             return "带我去VIP区域";
                case MallText::VipOfferPrefix:        return "你好！只需 ";
                case MallText::VipOfferSuffix:        return " 金币，我就能为你的整个账号开通VIP！";
                case MallText::BuyVipPrefix:          return "购买VIP资格（";
                case MallText::BuyVipSuffix:          return " 金币）";
                case MallText::ConfirmVipPrefix:      return "确定要花费 ";
                case MallText::ConfirmVipSuffix:      return " 金币开通VIP资格吗？";
                case MallText::NotEnoughGoldPrefix:   return "你的金币不足！VIP资格需要 ";
                case MallText::NotEnoughGoldSuffix:   return " 金币。";
                case MallText::VipSuccess:            return "|cff00ff00恭喜！你的账号现在已获得VIP资格！|r";
                case MallText::VipTeleportMissing:    return "数据库中还没有'VIP'传送点！";
            }
            break;

        case LOCALE_zhTW:
            switch (text)
            {
                case MallText::ModuleDisabled:        return "傳送模組目前已停用。";
                case MallText::GoSecretFortress:      return "帶我去秘密要塞！";
                case MallText::Cancel:                return "算了，我不需要。";
                case MallText::InCombat:              return "戰鬥中無法傳送！";
                case MallText::TeleportMissing:       return "資料庫中還沒有這個傳送點！";
                case MallText::Return:                return "帶我回去！";
                case MallText::VipOnly:               return "我只和VIP交談！請先從Karl那裡取得VIP資格。";
                case MallText::MysticGreeting:        return "歡迎……我知道十個古老而隱秘的地點。你想去哪裡？";
                case MallText::TravelTo:              return "前往 ";
                case MallText::GoVipArea:             return "帶我去VIP區域";
                case MallText::VipOfferPrefix:        return "你好！只需 ";
                case MallText::VipOfferSuffix:        return " 金幣，我就能為你的整個帳號開通VIP！";
                case MallText::BuyVipPrefix:          return "購買VIP資格（";
                case MallText::BuyVipSuffix:          return " 金幣）";
                case MallText::ConfirmVipPrefix:      return "確定要花費 ";
                case MallText::ConfirmVipSuffix:      return " 金幣開通VIP資格嗎？";
                case MallText::NotEnoughGoldPrefix:   return "你的金幣不足！VIP資格需要 ";
                case MallText::NotEnoughGoldSuffix:   return " 金幣。";
                case MallText::VipSuccess:            return "|cff00ff00恭喜！你的帳號現在已取得VIP資格！|r";
                case MallText::VipTeleportMissing:    return "資料庫中還沒有'VIP'傳送點！";
            }
            break;

        case LOCALE_enUS:
        default:
            switch (text)
            {
                case MallText::ModuleDisabled:        return "The teleport module is currently disabled.";
                case MallText::GoSecretFortress:      return "Take me to the Secret Fortress!";
                case MallText::Cancel:                return "Never mind.";
                case MallText::InCombat:              return "You cannot teleport while in combat!";
                case MallText::TeleportMissing:       return "This teleport location does not exist in the database yet!";
                case MallText::Return:                return "Take me back!";
                case MallText::VipOnly:               return "I only speak to VIPs! Get VIP status from Karl first.";
                case MallText::MysticGreeting:        return "Greetings... I know ten ancient, hidden places. Where would you like to travel?";
                case MallText::TravelTo:              return "Travel to ";
                case MallText::GoVipArea:             return "Take me to the VIP area";
                case MallText::VipOfferPrefix:        return "Hello! For ";
                case MallText::VipOfferSuffix:        return " gold I will unlock VIP status for your entire account!";
                case MallText::BuyVipPrefix:          return "Buy VIP rank (";
                case MallText::BuyVipSuffix:          return " gold)";
                case MallText::ConfirmVipPrefix:      return "Do you really want to unlock VIP rank for ";
                case MallText::ConfirmVipSuffix:      return " gold?";
                case MallText::NotEnoughGoldPrefix:   return "You do not have enough gold! VIP rank costs ";
                case MallText::NotEnoughGoldSuffix:   return " gold.";
                case MallText::VipSuccess:            return "|cff00ff00Congratulations! Your account now has VIP status!|r";
                case MallText::VipTeleportMissing:    return "The 'VIP' teleport location does not exist in the database yet!";
            }
            break;
    }

    return "";
}

static void SendMallMessage(Player* player, MallText text)
{
    ChatHandler(player->GetSession()).SendSysMessage(GetMallText(player, text));
}

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

class npc_secret_island_teleporter : public CreatureScript
{
public:
    npc_secret_island_teleporter() : CreatureScript("npc_secret_island_teleporter") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!g_ModuleEnabled)
        {
            SendMallMessage(player, MallText::ModuleDisabled);
            return true;
        }

        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GetMallText(player, MallText::GoSecretFortress), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_GO_SECRET);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GetMallText(player, MallText::Cancel), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_CLOSE);
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
                SendMallMessage(player, MallText::InCombat);
                return true;
            }

            QueryResult result = WorldDatabase.Query("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM `game_tele` WHERE `name` = 'Geheim1'");
            if (result)
            {
                Field* fields = result->Fetch();
                SavePlayerPosition(player);
                player->TeleportTo(fields[0].Get<uint32>(), fields[1].Get<float>(), fields[2].Get<float>(), fields[3].Get<float>(), fields[4].Get<float>());
            }
            else
                SendMallMessage(player, MallText::TeleportMissing);
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

class npc_secret_island_return : public CreatureScript
{
public:
    npc_secret_island_return() : CreatureScript("npc_secret_island_return") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!g_ModuleEnabled)
        {
            SendMallMessage(player, MallText::ModuleDisabled);
            return true;
        }

        ClearGossipMenuFor(player);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GetMallText(player, MallText::Return), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_RETURN);
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GetMallText(player, MallText::Cancel), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_CLOSE);
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
                SendMallMessage(player, MallText::InCombat);
                return true;
            }

            auto it = g_PreviousLocations.find(player->GetGUID());
            if (it != g_PreviousLocations.end())
            {
                player->TeleportTo(it->second.mapId, it->second.x, it->second.y, it->second.z, it->second.o);
                g_PreviousLocations.erase(it);
            }
            else if (player->GetTeamId() == TEAM_ALLIANCE)
                player->TeleportTo(0, -8833.37f, 628.62f, 94.01f, 1.0f);
            else
                player->TeleportTo(1, 1629.85f, -4373.59f, 31.55f, 3.6f);
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

class npc_mystic_teleporter : public CreatureScript
{
public:
    npc_mystic_teleporter() : CreatureScript("npc_mystic_teleporter") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!g_ModuleEnabled)
        {
            SendMallMessage(player, MallText::ModuleDisabled);
            return true;
        }

        ClearGossipMenuFor(player);
        uint32 accountId = player->GetSession()->GetAccountId();

        if (!IsAccountVip(accountId))
        {
            SendMallMessage(player, MallText::VipOnly);
            CloseGossipMenuFor(player);
            return true;
        }

        SendMallMessage(player, MallText::MysticGreeting);

        for (uint8 i = 1; i <= 10; ++i)
        {
            std::string label = GetMallText(player, MallText::TravelTo);
            label += "secret";
            label += (i < 10 ? "0" : "");
            label += std::to_string(i);
            AddGossipItemFor(player, GOSSIP_ICON_TABARD, label, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_SECRET_BASE + i);
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GetMallText(player, MallText::Cancel), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_CLOSE);
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
                SendMallMessage(player, MallText::InCombat);
                return true;
            }

            uint8 secretIndex = static_cast<uint8>(action - GOSSIP_ACTION_SECRET_BASE);
            std::string teleName = "secret" + (secretIndex < 10 ? std::string("0") : std::string("")) + std::to_string(secretIndex);
            QueryResult result = WorldDatabase.Query("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM `game_tele` WHERE `name` = '{}'", teleName);

            if (result)
            {
                Field* fields = result->Fetch();
                SavePlayerPosition(player);
                player->TeleportTo(fields[0].Get<uint32>(), fields[1].Get<float>(), fields[2].Get<float>(), fields[3].Get<float>(), fields[4].Get<float>());
            }
            else
                SendMallMessage(player, MallText::TeleportMissing);
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

class npc_vip_master : public CreatureScript
{
public:
    npc_vip_master() : CreatureScript("npc_vip_master") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        if (!g_ModuleEnabled)
        {
            SendMallMessage(player, MallText::ModuleDisabled);
            return true;
        }

        ClearGossipMenuFor(player);
        uint32 accountId = player->GetSession()->GetAccountId();

        if (IsAccountVip(accountId))
            AddGossipItemFor(player, GOSSIP_ICON_TAXI, GetMallText(player, MallText::GoVipArea), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_GO_VIP);
        else
        {
            std::string msg = GetMallText(player, MallText::VipOfferPrefix);
            msg += std::to_string(g_VipPriceGold);
            msg += GetMallText(player, MallText::VipOfferSuffix);
            ChatHandler(player->GetSession()).SendSysMessage(msg.c_str());

            std::string label = GetMallText(player, MallText::BuyVipPrefix);
            label += std::to_string(g_VipPriceGold);
            label += GetMallText(player, MallText::BuyVipSuffix);

            std::string confirm = GetMallText(player, MallText::ConfirmVipPrefix);
            confirm += std::to_string(g_VipPriceGold);
            confirm += GetMallText(player, MallText::ConfirmVipSuffix);

            AddGossipItemFor(player, GOSSIP_ICON_MONEY_BAG, label, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_BUY_VIP, confirm, 0, false);
        }

        AddGossipItemFor(player, GOSSIP_ICON_CHAT, GetMallText(player, MallText::Cancel), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_CLOSE);
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

            if (!player->HasEnoughMoney(vipPriceCopper))
            {
                std::string errorMsg = GetMallText(player, MallText::NotEnoughGoldPrefix);
                errorMsg += std::to_string(g_VipPriceGold);
                errorMsg += GetMallText(player, MallText::NotEnoughGoldSuffix);
                ChatHandler(player->GetSession()).SendSysMessage(errorMsg.c_str());
                CloseGossipMenuFor(player);
                return true;
            }

            player->ModifyMoney(-static_cast<int32>(vipPriceCopper));
            CharacterDatabase.Execute("REPLACE INTO vip (AccountId, active) VALUES ({}, 1)", accountId);
            player->CastSpell(player, 63711, true);
            SendMallMessage(player, MallText::VipSuccess);
        }
        else if (action == GOSSIP_ACTION_GO_VIP)
        {
            if (player->IsInCombat())
            {
                CloseGossipMenuFor(player);
                SendMallMessage(player, MallText::InCombat);
                return true;
            }

            QueryResult result = WorldDatabase.Query("SELECT `map`, `position_x`, `position_y`, `position_z`, `orientation` FROM `game_tele` WHERE `name` = 'VIP'");
            if (result)
            {
                Field* fields = result->Fetch();
                SavePlayerPosition(player);
                player->TeleportTo(fields[0].Get<uint32>(), fields[1].Get<float>(), fields[2].Get<float>(), fields[3].Get<float>(), fields[4].Get<float>());
            }
            else
                SendMallMessage(player, MallText::VipTeleportMissing);
        }

        CloseGossipMenuFor(player);
        return true;
    }
};

void AddMallTeleportScripts()
{
    new MallTeleportWorldScript();
    new npc_secret_island_teleporter();
    new npc_secret_island_return();
    new npc_mystic_teleporter();
    new npc_vip_master();
}
