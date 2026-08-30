Completely revised VIP teleport and updated so it runs currently. (I'm still working on removing unnecessary server entries)

Custom NPC Overview

This module includes several custom NPCs for secret locations, VIP areas, and player teleportation.

NPC Entry	NPC	Access	Function
991105	Secret Area Teleporter	Everyone	Teleports the player to the Geheim1 location and saves the player's previous position.
991106	Return Teleporter	Everyone	Returns the player to the previously saved position.
991107	Secret Teleporter	VIP only	Provides access to up to 10 configurable secret teleport locations (secret01 – secret10).
991108	VIP Master	Everyone / VIP	Allows normal players to purchase VIP status. VIP players can use this NPC to teleport to the VIP area.
NPC 991105 – Secret Area Teleporter

This NPC provides access to the main secret area.
When the player selects the teleport option, the module:
Saves the player's current position.
Searches the AzerothCore game_tele table for the teleport named:

Geheim1

Teleports the player to that location.
The saved position can later be restored by NPC 991106.
Players cannot use the teleport while in combat.

NPC 991106 – Return Teleporter

This NPC allows players to return from one of the special areas.
The module remembers the player's position before using one of the custom teleport NPCs.
When the player talks to NPC 991106, the player is teleported back to the previously saved position.
If no previous position is available, the module uses a faction-based fallback location:

Alliance → Stormwind
Horde → Orgrimmar

This makes it possible to place the Return Teleporter inside secret areas without requiring a separate teleport destination for every location.

NPC 991107 – VIP Secret Teleporter

This NPC provides VIP players with access to multiple secret locations.
Non-VIP players cannot use the secret teleport menu.
VIP players can select from up to 10 secret teleport destinations:

secret01
secret02
secret03
secret04
secret05
secret06
secret07
secret08
secret09
secret10

Before teleporting, the player's current position is saved. The player can therefore use NPC 991106 at the destination to return to the previous location.
The teleport destinations themselves are stored in the AzerothCore game_tele table.
This means that the coordinates can be changed without recompiling the module.
Editing the Secret Teleport Locations
The destinations used by NPC 991107 are normal AzerothCore game_tele entries.
The module searches for the following names:

secret01
secret02
secret03
secret04
secret05
secret06
secret07
secret08
secret09
secret10

You can change where these teleport destinations lead by editing their entries in the AzerothCore world database or set the tele (.tele add ...).
The important part is the teleport name.

For example:

secret01

can point to any valid location in the game world.

You may change its:

Map
X coordinate
Y coordinate
Z coordinate
Orientation

The C++ module does not need to be recompiled when only the destination coordinates are changed.

Example

A game_tele entry contains information similar to:

ID
position_x
position_y
position_z
orientation
map
name

For example:

name = secret01

The coordinates assigned to this entry determine where the first Secret Teleporter option sends the player.
Move or edit secret01 to change destination #1.

The same applies to all other destinations:

secret01 → Secret Destination 1
secret02 → Secret Destination 2
secret03 → Secret Destination 3
...
secret10 → Secret Destination 10
Important

The names secret01 through secret10 are referenced by the module.
You can freely change their coordinates in game_tele, but do not rename the entries unless you also change the corresponding names in the C++ source code.
Changing only the coordinates requires:
No C++ changes and no recompilation.

NPC 991108 – VIP Master

This NPC manages access to the VIP system.
Normal Players

Players without VIP status can purchase VIP access from this NPC.
The default price is: 10,000 Gold

The price can be configured through:
VIP.PriceInGold = 10000
After purchasing VIP status, the account is registered in the VIP system.
VIP status is account-wide, not character-specific.

VIP Players

Players who already have VIP status receive an option to enter the VIP area.
The NPC searches the AzerothCore game_tele table for:

VIP

and teleports the player to that destination.
The player's previous position is saved so that NPC 991106 can return the player afterward.

Teleport Structure

The complete teleport system works like this:

                         ┌─ secret01
                         ├─ secret02
                         ├─ secret03
VIP Player → NPC 991107 ─├─ ...
                         └─ secret10
                              │
                              ▼
                       Secret Location
                              │
                         NPC 991106
                              │
                              ▼
                       Previous Location

The VIP area works in the same way:

VIP Player
    │
    ▼
NPC 991108
    │
    ▼
game_tele: VIP
    │
    ▼
VIP Area
    │
NPC 991106
    │
    ▼
Previous Location

This design allows server administrators to move the VIP and secret destinations by changing the corresponding game_tele coordinates instead of modifying and recompiling the module.
