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
DELETE FROM `gossip_menu_option` WHERE `MenuID` IN (991105, 991106, 991107, 991108);
DELETE FROM `creature_template` WHERE `entry` IN (991105, 991106, 991107, 991108);
DELETE FROM `creature_template_model` WHERE `CreatureID` IN (991105, 991106, 991107, 991108);

-- --------------------------------------------------------------------
-- 2. CREATURE TEMPLATES ANLEGEN & MIT C++ VERKNÜPFEN (OHNE 'scale')
-- --------------------------------------------------------------------

-- NPC 991105: Kodo (Geheime Festung Teleporter)
INSERT INTO `creature_template` SET
    `entry` = 991105,
    `name` = 'Geheimnisvoller Kodo',
    `subname` = 'Zur Geheimen Festung',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `ScriptName` = 'npc_secret_island_teleporter';

-- NPC 991106: Rückkehr-NPC (Bringt den Spieler an den Ursprungsort)
INSERT INTO `creature_template` SET
    `entry` = 991106,
    `name` = 'Heimkehrer',
    `subname` = 'Rückkehr-Portal',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `ScriptName` = 'npc_secret_island_return';

-- NPC 991107: Mystischer Teleporter (10 verborgene Orte)
INSERT INTO `creature_template` SET
    `entry` = 991107,
    `name` = 'Der Weltenwanderer',
    `subname` = 'Mystischer Teleporter',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `ScriptName` = 'npc_mystic_teleporter';

-- NPC 991108: VIP Meister (VIP Verkauf & Teleport)
INSERT INTO `creature_template` SET
    `entry` = 991108,
    `name` = 'Lord Aurelius',
    `subname` = 'VIP Meister',
    `minlevel` = 80,
    `maxlevel` = 80,
    `faction` = 35,
    `npcflag` = 1,
    `ScriptName` = 'npc_vip_master';

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