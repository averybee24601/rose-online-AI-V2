# AI Rose Evolved: Roadmap

This document outlines the planned improvements, bug fixes, and modernization efforts for the AI Rose Evolved project.

## Phase 1: Security Hardening

The initial phase of the project will focus on addressing the most critical security vulnerabilities. This will involve:

*   **Comprehensive Input Validation**: All packet handlers will be reviewed to ensure that they are properly validating all client input. This will prevent a wide range of exploits, including SQL injection, cross-site scripting, and buffer overflows.
*   **Authentication and Authorization**: All player actions will be properly authenticated and authorized to prevent unauthorized access to GM commands and other sensitive features.
*   **Secure Coding Practices**: The codebase will be reviewed to ensure that it adheres to secure coding practices. This will include using modern C++ features, such as smart pointers and RAII, to prevent memory corruption and other common vulnerabilities.

## Phase 2: Quest System Overhaul

The second phase of the project will focus on overhauling the quest system. This will involve:

*   **QSD Decompiler/Recompiler**: A tool will be created to decompile and recompile the `.qsd` files. This will allow for the analysis and fixing of the quest scripts.
*   **Quest Script Analysis**: All of the quest scripts will be reviewed to identify and fix any bugs or inconsistencies.
*   **Improved Logging and Error Handling**: More detailed logging and error handling will be added to the quest system to make it easier to debug.

## Phase 3: Refactoring and Modernization

The third phase of the project will focus on refactoring and modernizing the codebase. This will involve:

*   **Code Cleanup**: The codebase will be cleaned up to remove redundant code, improve the overall code structure, and add unit tests.
*   **Modern C++**: The codebase will be updated to use modern C++ features, such as smart pointers and RAII.
*   **Performance Optimization**: The codebase will be profiled and optimized to improve performance.

## Phase 4: New Feature Development

The fourth phase of the project will focus on developing new features. This will include:

*   **New Crafting System**: A new crafting system will be developed that is more engaging and rewarding than the original system.
*   **New Skill System**: A new skill system will be developed that is more balanced and customizable than the original system.
*   **New Dungeon System**: A new dungeon system will be developed that is more challenging and rewarding than the original system.
