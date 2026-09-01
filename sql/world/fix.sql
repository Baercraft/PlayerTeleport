-- ============================================================================
-- MASTER INSTALLATION/FIX SCRIPT: VIP & TELEPORT MODULE (AzerothCore)
-- ============================================================================

-- ----------------------------------------------------------------------------
-- 1. CHARACTERS DATENBANK: VIP-Tabelle anlegen (Accountgebunden)
-- ----------------------------------------------------------------------------
USE `acore_characters`;

CREATE TABLE IF NOT EXISTS `vip` (
  `AccountId` int(10) unsigned NOT NULL,
  `active` tinyint(3) unsigned NOT NULL DEFAULT '1',
  PRIMARY KEY (`AccountId`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='VIP Account Status Table';


-- ----------------------------------------------------------------------------
-- 2. WORLD DATENBANK: NPCs, Modelle, Dummy-Gossips & Teleportpunkte
-- ----------------------------------------------------------------------------
USE `acore_world`;

-- Alten Datenbestand sicherheitshalber aufräumen
DELETE FROM `creature_template_locale` WHERE `entry` IN (991105, 991106, 991107, 991108);
DELETE FROM `creature_template` WHERE `entry` IN (991105, 991106, 991107, 991108);
DELETE FROM `creature_template_model` WHERE `CreatureID` IN (991105, 991106, 991107, 991108);
DELETE FROM `gossip_menu_option` WHERE `MenuID` IN (991105, 991106, 991107, 991108);
DELETE FROM `gossip_menu` WHERE `MenuID` IN (991105, 991106, 991107, 991108);

-- Verwaiste Conditions aus frueheren DB-Gossip-Versionen entfernen.
-- SourceType 14 = gossip_menu, SourceType 15 = gossip_menu_option.
-- Die aktuellen NPCs bauen ihre Gossip-Menues ausschliesslich im C++ Script auf.
DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` IN (14, 15)
  AND `SourceGroup` IN (991105, 991106, 991107, 991108);

-- A. Creature Templates anlegen (mit sauberen Namen & unit_class = 1)
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `minlevel`, `maxlevel`, `faction`, `npcflag`, `unit_class`, `ScriptName`) VALUES
(991105, 'Mysterious Kodo', 'To the Secret Fortress', 80, 80, 35, 1, 1, 'npc_secret_island_teleporter'),
(991106, 'Returner', 'Return Portal', 80, 80, 35, 1, 1, 'npc_secret_island_return'),
(991107, 'The Worldwalker', 'Mystic Teleporter', 80, 80, 35, 1, 1, 'npc_mystic_teleporter'),
(991108, 'Lord Aurelius', 'VIP Master', 80, 80, 35, 1, 1, 'npc_vip_master');

-- Localized NPC names/titles. English is stored in creature_template as fallback.
INSERT INTO `creature_template_locale` (`entry`, `locale`, `Name`, `Title`) VALUES
(991105, 'deDE', 'Geheimnisvoller Kodo', 'Zur Geheimen Festung'),
(991105, 'frFR', 'Kodo mystérieux', 'Vers la Forteresse Secrète'),
(991105, 'esES', 'Kodo misterioso', 'A la Fortaleza Secreta'),
(991105, 'esMX', 'Kodo misterioso', 'A la Fortaleza Secreta'),
(991105, 'ruRU', 'Таинственный кодо', 'В Тайную крепость'),
(991105, 'zhCN', '神秘的科多兽', '前往秘密要塞'),
(991105, 'zhTW', '神秘的科多獸', '前往秘密要塞'),
(991106, 'deDE', 'Heimkehrer', 'Rückkehr-Portal'),
(991106, 'frFR', 'Guide du retour', 'Portail de retour'),
(991106, 'esES', 'Retornador', 'Portal de regreso'),
(991106, 'esMX', 'Retornador', 'Portal de regreso'),
(991106, 'ruRU', 'Возвращающий', 'Портал возвращения'),
(991106, 'zhCN', '归返者', '返回传送门'),
(991106, 'zhTW', '歸返者', '返回傳送門'),
(991107, 'deDE', 'Der Weltenwanderer', 'Mystischer Teleporter'),
(991107, 'frFR', 'Le Marchemonde', 'Téléporteur mystique'),
(991107, 'esES', 'El Caminamundos', 'Teletransportador místico'),
(991107, 'esMX', 'El Caminamundos', 'Teletransportador místico'),
(991107, 'ruRU', 'Странник миров', 'Мистический телепорт'),
(991107, 'zhCN', '世界行者', '神秘传送师'),
(991107, 'zhTW', '世界行者', '神秘傳送師'),
(991108, 'deDE', 'Lord Aurelius', 'VIP Meister'),
(991108, 'frFR', 'Lord Aurelius', 'Maître VIP'),
(991108, 'esES', 'Lord Aurelius', 'Maestro VIP'),
(991108, 'esMX', 'Lord Aurelius', 'Maestro VIP'),
(991108, 'ruRU', 'Лорд Аурелиус', 'VIP-мастер'),
(991108, 'zhCN', '奥雷利乌斯领主', 'VIP管理员'),
(991108, 'zhTW', '奧雷利烏斯領主', 'VIP管理員');

-- B. Creature Models zuweisen
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`) VALUES
(991105, 0, 1281,  1.0, 1), -- Flussgeist (Wassergeist-Modell)
(991106, 0, 10034, 1.0, 1), -- Heimkehrer (Portal-Modell)
(991107, 0, 25901, 1.0, 1), -- Geheimniskrämer (Magier-Modell)
(991108, 0, 24949, 1.0, 1); -- Karl (Edler Blutelf)

-- C. Gossip-Datenbankeintraege bewusst nicht anlegen
-- Die vier NPCs verwenden C++ CreatureScripts und bauen ihre Gossip-Menues
-- dynamisch auf. Veraltete/dummy DB-Menues wurden oben entfernt, damit keine
-- GossipMenu-/npc_text-Warnungen durch ungueltige TextIDs entstehen.

-- D. Standard Teleportpunkte in game_tele eintragen (falls nicht vorhanden)
-- HINWEIS: Pass die Koordinaten/Maps an, falls du eigene Spots in der DB nutzt!
REPLACE INTO `game_tele` (`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `name`) VALUES
(9901, 0, -8833.37, 628.62, 94.01, 1.0, 'Geheim1'),
(9902, 1,  1629.85, -4373.59, 31.55, 3.6, 'VIP'),
(9903, 0, -8833.37, 628.62, 94.01, 1.0, 'secret01'),
(9904, 0, -8833.37, 628.62, 94.01, 1.0, 'secret02'),
(9905, 0, -8833.37, 628.62, 94.01, 1.0, 'secret03'),
(9906, 0, -8833.37, 628.62, 94.01, 1.0, 'secret04'),
(9907, 0, -8833.37, 628.62, 94.01, 1.0, 'secret05'),
(9908, 0, -8833.37, 628.62, 94.01, 1.0, 'secret06'),
(9909, 0, -8833.37, 628.62, 94.01, 1.0, 'secret07'),
(9910, 0, -8833.37, 628.62, 94.01, 1.0, 'secret08'),
(9911, 0, -8833.37, 628.62, 94.01, 1.0, 'secret09'),
(9912, 0, -8833.37, 628.62, 94.01, 1.0, 'secret10');