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
DELETE FROM `creature_template` WHERE `entry` IN (991105, 991106, 991107, 991108);
DELETE FROM `creature_template_model` WHERE `CreatureID` IN (991105, 991106, 991107, 991108);
DELETE FROM `gossip_menu_option` WHERE `MenuID` IN (991105, 991106, 991107, 991108);
DELETE FROM `gossip_menu` WHERE `MenuID` IN (991105, 991106, 991107, 991108);

-- A. Creature Templates anlegen (mit sauberen Namen & unit_class = 1)
INSERT INTO `creature_template` (`entry`, `name`, `subname`, `minlevel`, `maxlevel`, `faction`, `npcflag`, `unit_class`, `ScriptName`) VALUES
(991105, 'Flussgeist', 'Hueter der Quellen', 80, 80, 35, 1, 1, 'npc_secret_island_teleporter'),
(991106, 'Heimkehrer', 'Rueckkehr-Portal', 80, 80, 35, 1, 1, 'npc_secret_island_return'),
(991107, 'Geheimniskraemer', 'Mystischer Teleporter', 80, 80, 35, 1, 1, 'npc_mystic_teleporter'),
(991108, 'Karl', 'VIP Meister', 80, 80, 35, 1, 1, 'npc_vip_master');

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