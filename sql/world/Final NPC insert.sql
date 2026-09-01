-- ====================================================================
-- FINALES SQL-SKRIPT FÜR DAS C++ TELEPORT- & VIP-MODUL
-- IDs: 991105 (Kodo), 991106 (Rück-NPC), 991107 (Mystiker), 991108 (VIP)
-- ====================================================================

USE `acore_world`;

-- --------------------------------------------------------------------
-- 1. ALTE LOGIK / KONFLIKTE LÖSCHEN
-- Delete alte SmartAI-, Gossip- & Template-Einträge
-- --------------------------------------------------------------------
DELETE FROM `smart_scripts` WHERE `entryorguid` IN (991105, 991106, 991107, 991108) AND `source_type` = 0;
DELETE FROM `gossip_menu` WHERE `MenuID` IN (991105, 991106, 991107, 991108);

-- Verwaiste Conditions aus frueheren DB-Gossip-Versionen entfernen.
-- SourceType 14 = gossip_menu, SourceType 15 = gossip_menu_option.
-- Die aktuellen NPCs bauen ihre Gossip-Menues ausschliesslich im C++ Script auf.
DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` IN (14, 15)
  AND `SourceGroup` IN (991105, 991106, 991107, 991108);
DELETE FROM `gossip_menu_option` WHERE `MenuID` IN (991105, 991106, 991107, 991108);
DELETE FROM `creature_template_locale` WHERE `entry` IN (991105, 991106, 991107, 991108);
DELETE FROM `creature_template` WHERE `entry` IN (991105, 991106, 991107, 991108);
DELETE FROM `creature_template_model` WHERE `CreatureID` IN (991105, 991106, 991107, 991108);

-- --------------------------------------------------------------------
-- 2. CREATURE TEMPLATES ANLEGEN & MIT C++ VERKNÜPFEN (OHNE 'scale')
-- --------------------------------------------------------------------

-- NPC 991105: Kodo (Geheime Festung Teleporter)
INSERT INTO `creature_template` SET
    `entry` = 991105,
    `name` = 'Mysterious Kodo',
    `subname` = 'To the Secret Fortress',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `ScriptName` = 'npc_secret_island_teleporter';

-- NPC 991106: Rückkehr-NPC (Bringt den Spieler an den Ursprungsort)
INSERT INTO `creature_template` SET
    `entry` = 991106,
    `name` = 'Returner',
    `subname` = 'Return Portal',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `ScriptName` = 'npc_secret_island_return';

-- NPC 991107: Mystischer Teleporter (10 verborgene Orte)
INSERT INTO `creature_template` SET
    `entry` = 991107,
    `name` = 'The Worldwalker',
    `subname` = 'Mystic Teleporter',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `ScriptName` = 'npc_mystic_teleporter';

-- NPC 991108: VIP Meister (VIP Verkauf & Teleport)
INSERT INTO `creature_template` SET
    `entry` = 991108,
    `name` = 'Lord Aurelius',
    `subname` = 'VIP Master',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `ScriptName` = 'npc_vip_master';

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

-- --------------------------------------------------------------------
-- 3. MODELLE UND SKALIERUNG (DisplayScale = 1.0)
-- --------------------------------------------------------------------
INSERT INTO `creature_template_model` (`CreatureID`, `Idx`, `CreatureDisplayID`, `DisplayScale`, `Probability`) VALUES
(991105, 0, 116, 1.0, 1),   -- Kodo Display
(991106, 0, 10034, 1.0, 1), -- Portal Display
(991107, 0, 25901, 1.0, 1), -- Human Mage Display
(991108, 0, 24949, 1.0, 1); -- Noble Bloodelf Display

-- --------------------------------------------------------------------
-- 4. BENÖTIGTE TELEPORTPUNKTE IN GAME_TELE ANLEGEN
-- --------------------------------------------------------------------

-- Kodo Zielpunkt: Geheim1
REPLACE INTO `game_tele` (`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `name`) 
VALUES (9000, 37, 975.7652, 274.0396, 319.33868, 0.1202559, 'Geheim1');

-- VIP Zielpunkt: VIP
REPLACE INTO `game_tele` (`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `name`) 
VALUES (9020, 0, -8833.38, 628.628, 94.0066, 1.0, 'VIP');

-- Mystischer Teleporter Zielpunkte: secret01 bis secret10
REPLACE INTO `game_tele` (`id`, `map`, `position_x`, `position_y`, `position_z`, `orientation`, `name`) VALUES 
(9001, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret01'),
(9002, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret02'),
(9003, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret03'),
(9004, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret04'),
(9005, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret05'),
(9006, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret06'),
(9007, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret07'),
(9008, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret08'),
(9009, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret09'),
(9010, 0, -8833.38, 628.628, 94.0066, 1.0, 'secret10');

-- --------------------------------------------------------------------
-- 5. VIP-TABELLE IN ACORE_CHARACTERS SICHERSTELLEN
-- --------------------------------------------------------------------
USE `acore_characters`;

CREATE TABLE IF NOT EXISTS `vip` (
  `AccountId` INT(10) UNSIGNED NOT NULL,
  `active` TINYINT(3) UNSIGNED NOT NULL DEFAULT '1',
  PRIMARY KEY (`AccountId`)
) ENGINE=INNODB DEFAULT CHARSET=utf8mb4;