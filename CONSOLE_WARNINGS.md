# AzerothCore console warnings

## Warnings fixed by this module

The NPCs 991105, 991106, 991107 and 991108 use C++ `CreatureScript` gossip.
They do not need rows in `gossip_menu` or `gossip_menu_option`.

Older database versions can leave rows in `conditions` with gossip source types
14/15 and `SourceGroup` 991107/991108. AzerothCore then reports:

- `addToGossipMenus: GossipMenu 991107 not found`
- `Not handled grouped condition, SourceGroup 991107`
- the same messages for 991108

Run `sql/world/cleanup_orphan_gossip_conditions.sql`, or use the updated
`sql/world/fix.sql`.

## Warnings not caused by PlayerTeleport

The following identifiers do not occur anywhere in this module:

- creature/gossip 600001
- creature/gossip 600002
- `spell_costume_override`

Therefore these warnings belong to another custom SQL/module and are not
removed automatically by PlayerTeleport. Blindly deleting or changing those
records here could break that other content.

Typical meaning:

- `gossip_menu entry 600001/600002 ... non-existing TextID`: the referenced
  `npc_text` row is missing (or the gossip menu has the wrong TextID).
- `Creature entry ... has SmartAI enabled but no SmartAI entries`: the creature
  template uses `AIName='SmartAI'` but has no matching source_type=0 rows in
  `smart_scripts`. Either add the intended SmartAI script or remove SmartAI from
  that creature if another script handles it.
- `Script named spell_costume_override is not assigned in the database`: a C++
  spell script is registered in the core/module but no appropriate row assigns
  it in `spell_script_names`. The correct spell ID must be known before adding
  that assignment.
