USE `acore_world`;

DELETE FROM `creature_template_locale` WHERE `entry` IN (991105, 991106, 991107, 991108);

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

