Rose Online Evolved — Build and Run Guide
=======================================

Prerequisites
- C++ toolchain: GCC/Clang (Linux) or MSVC (Windows)
- CMake >= 3.16
- MariaDB/MySQL client development libraries (e.g., libmariadb-dev)
- Python 3.8+ (for config helper script)

Repository layout
- server/Common — shared networking, logging, database wrappers (legacy)
- server/Login Server — login server sources
- server/Char Server — character server sources
- server/World Server — world server sources
- include/ — bundled MySQL headers for compatibility
- config/*.yml — human-readable server configs
- scripts/generate_legacy_conf.py — YAML => legacy .conf generator
- sql/001_core_schema.sql — minimal DB schema to boot servers

Setup database (MySQL/MariaDB)
1) Create database and user
   CREATE DATABASE rose CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
   CREATE USER 'rose'@'%' IDENTIFIED BY 'rosepass';
   GRANT ALL PRIVILEGES ON rose.* TO 'rose'@'%';
   FLUSH PRIVILEGES;

2) Load minimal schema
   mysql -u rose -p rose < sql/001_core_schema.sql

3) Optional: load your server content SQL (items, npcs, quests, etc.).

Configuration
- Edit config/loginserver.yml, config/charserver.yml, config/worldserver.yml
- For now, servers still read legacy *.conf files. Generate them from YAML:
  pip install pyyaml
  python3 scripts/generate_legacy_conf.py --config-dir config --out-dir .
  This writes loginserver.conf, charserver.conf, worldserver.conf in repo root.

Build (Linux)
- Install dependencies (Debian/Ubuntu example):
  sudo apt-get update && sudo apt-get install -y build-essential cmake libmariadb-dev
- Configure and build:
  cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build -j
- Binaries will be at build/server/*/*. (Install target installs to bin/.)

Build (Windows)
- Install Visual Studio with C++ toolset and CMake
- Install MariaDB Connector/C or MySQL Connector/C and ensure library is found
- Configure from Developer Command Prompt:
  cmake -S . -B build -G "Visual Studio 17 2022" -A x64
  cmake --build build --config Release

Run order
1) Start Login Server
   ./build/server/Login\ Server/login_server loginserver.conf
2) Start Char Server
   ./build/server/Char\ Server/char_server charserver.conf
3) Start World Server
   ./build/server/World\ Server/world_server worldserver.conf

Notes
- The codebase is legacy OSRose with modernization steps. We preserved functionality while introducing a reproducible build and readable configs. During migration, YAML is converted to legacy INI-style conf to avoid breaking runtime behavior.
- If linking fails for MySQL client, verify libmariadb is installed and visible to the linker.
- If servers crash on startup due to missing DB tables, ensure sql/001_core_schema.sql and your content SQL are loaded.

Next modernization steps (planned)
- Replace legacy INI parser with direct YAML loader (no conversion step).
- Wrap MySQL usage in RAII and prepared statements; remove raw mysql_* calls.
- Introduce unit tests for packet handlers and DB layer.
- Add CI workflows (Linux/Windows) and containerized dev environment.
- Replace pthreads with std::thread and condition variables.
