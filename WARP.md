# WARP.md

This file provides guidance to WARP (warp.dev) when working with code in this repository.

Repository overview and architecture
- Purpose: This repository currently serves as a governance/manifest source for the "AI Rose Evolved" project (a ROSE Online private server effort driven by agentic development). It defines project vision, directives, and quality criteria rather than application code.
- Key artifact: Manifest.json
  - manifest.title, description, vision: High-level purpose and positioning
  - manifest.directives: Guardrails for development (aesthetic consistency, UX-first, functionality guard, improvement-only, strategic AI development, update balance)
  - manifest.directives.quality_ratings: Dimensions for evaluating changes (scalability, functionality, reliability)
- Code footprint: No build system, package manifests, tests, or app/runtime code were detected. The server/ directory only contains a version-control metadata folder (server/.svn), indicating the actual server code lives elsewhere or is not checked in here.

Common commands (Windows PowerShell)
- Validate the JSON manifest (syntax check)
  - Get-Content -Raw Manifest.json | ConvertFrom-Json | Out-Null
- Pretty-print/normalize Manifest.json (preserves structure, rewrites formatting)
  - $json = Get-Content -Raw Manifest.json | ConvertFrom-Json; $json | ConvertTo-Json -Depth 32 | Set-Content Manifest.json -Encoding UTF8
- Inspect manifest keys/values quickly
  - (Get-Content -Raw Manifest.json | ConvertFrom-Json).manifest | Format-List
- Search the repo while ignoring the SVN metadata (requires Select-String only; -Exclude is applied per path spec)
  - Get-ChildItem -Recurse -File -Exclude .svn | Select-String -Pattern "your-term-here"

Build, lint, and test
- No build toolchain, lint configuration, or tests were found in this repository. If/when application code or automation is added, prefer to surface the following here for future automation:
  - Build: the exact command(s) to compile/build artifacts
  - Lint/format: the exact command(s) and config files used
  - Tests: how to run the full suite and a single test (including the framework in use)

Operational notes for future automation
- Treat Manifest.json as the project’s contract and decision framework. Any future code changes should reference and preserve the intent captured in:
  - manifest.directives (aesthetic_consistency, user_experience_first, functionality_guard, improvement_only, strategic_AI_development, update_balance)
  - manifest.directives.quality_ratings (scalability, functionality, reliability)
- The presence of server/.svn indicates an external SVN working copy structure; however, no actual source files are committed here. Avoid traversing server/.svn for code intelligence—it contains VCS internals, not current working files.

Gaps worth filling (to improve agent productivity)
- Add a README.md that explains where the server code lives and how this manifest is consumed by other repos/services.
- If this repo is intended to hold code in the future, add explicit build/lint/test scripts and update this WARP.md with the canonical commands.

