-- PlayerTeleport: cleanup for orphaned gossip conditions
-- Safe to run on acore_world when the console reports e.g.:
--   addToGossipMenus: GossipMenu 991107 not found
--   Not handled grouped condition, SourceGroup 991107
--
-- The module builds gossip menus for 991105..991108 in C++, so old DB gossip
-- conditions for those MenuIDs are obsolete and must be removed.

USE `acore_world`;

DELETE FROM `conditions`
WHERE `SourceTypeOrReferenceId` IN (14, 15)
  AND `SourceGroup` IN (991105, 991106, 991107, 991108);

DELETE FROM `gossip_menu_option`
WHERE `MenuID` IN (991105, 991106, 991107, 991108);

DELETE FROM `gossip_menu`
WHERE `MenuID` IN (991105, 991106, 991107, 991108);
