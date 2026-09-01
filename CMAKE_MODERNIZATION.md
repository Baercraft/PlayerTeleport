# CMake modernization

This package uses AzerothCore's current module loader convention.

Changes:
- removed deprecated `AC_ADD_SCRIPT_LOADER(...)`
- replaced `src/loader.h` with `src/loader.cpp`
- loader entry point is now `AddPlayerTeleportScripts()`
- removed the old manual AFTER_WORLDSERVER_CMAKE config-copy hook
- kept `conf/mod-mall-teleport.conf.dist`; AzerothCore discovers module configs from `conf/`
- retained `/utf-8` for MSVC builds

The teleport implementation itself was not changed.
