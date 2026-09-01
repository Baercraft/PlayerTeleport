# PlayerMallTeleport
Module for Azerothcore to teleport players to a mall with a command

# How to use
Install the SQL file to the character database on a GM account make two teleport locations with command :

.tele add 

two locations need to be added for this module to work.

1. VIPMall
2. PlayerMall

once that is complete go to the VIP table in the character database

add the accountid of the player account and also set it to active = 1

if the accountid is not active the VIP mall would not work for the current account.


# Show your appreciation
[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=SBJFTAJKUNEXC)


## Multi-language support

The module automatically uses the locale of each connected WoW client. No language setting in the module config is required.

Supported languages:
- English (enUS, fallback)
- German (deDE)
- Spanish (esES and esMX)
- French (frFR)
- Russian (ruRU)
- Simplified Chinese (zhCN)
- Traditional Chinese (zhTW)

Localized content includes gossip options, confirmation dialogs, system messages, VIP purchase messages, combat/error messages, secret-destination labels, and NPC names/titles. Unsupported client locales fall back to English.

For an existing installation, run `sql/world/multilanguage_npc_names.sql` once to add localized NPC names/titles. Recompile the module to activate the localized C++ gossip and system messages.
