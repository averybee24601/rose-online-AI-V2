# AI-Rose-Evolved
An AI-Managed ROSE Online Server

This is a test edit.


## Why this repo exists
AI Rose Evolved is not just another private server. It is the realization of a long-held dream: to honor the warmth, charm, and aesthetics of ROSE Online while applying thoughtful, AI-guided improvements. This repository captures the big-picture direction and non-negotiable guardrails that future code and content must respect.

If you’re a developer working on gameplay, content, tools, or services that touch AI Rose Evolved, you should use the manifest here to inform decisions, justify tradeoffs, and validate that changes align with the project’s soul.


## Repository structure
- Manifest.json — The contract for vision and decision-making. Read this file first.
- WARP.md — Guidance for working with this repository in Warp and PowerShell environments.
- server/.svn — VCS metadata only; not the live source code. Do not rely on this path for current gameplay/server implementation.

There is no build system, package manifest, or test suite in this repository (by design). Code that implements the server and tooling will live in other repositories or will be added later; those components must adhere to the manifest defined here.


## The Manifest (what it contains and how to use it)
Manifest.json structure (high level):
- manifest.title, description, vision — The identity and narrative of the project.
- manifest.directives — Guardrails and promises to players:
  - aesthetic_consistency — Changes must feel native to ROSE; visuals, UI, and mechanics should blend seamlessly.
  - user_experience_first — If impact to player experience is unclear, redesign before proceeding.
  - functionality_guard — Stability is sacred; no change may break essential functionality.
  - improvement_only — Only ship changes that measurably enhance gameplay, accessibility, immersion, or community connection.
  - strategic_AI_development — Balance nostalgia with innovation; plan with intention.
  - update_balance — Preserve fair gameplay; no skill, item, or mechanic should dominate.
- manifest.directives.quality_ratings — Dimensions to evaluate any change:
  - scalability — Can we handle growth in players/content without regressions?
  - functionality — Does it do exactly what it intends to do?
  - reliability — Is it stable, with no crashes or unstable behavior?
- heartfelt_note — The motivation behind this project—use it as a cultural anchor when making tradeoffs.

Practically speaking, the manifest is a decision framework:
- Use directives to justify or block changes.
- Use quality ratings to define acceptance criteria.
- Treat the vision as the top-level north star when principles conflict.


## Quick Start (Windows)
- Start everything with one double-click:
  - server\Binary\StartAllServers.bat
  - This launches LoginServer, CharServer, WorldServer, and a HealthWatcher that writes server\Binary\log\health.txt
- To view health at any time:
  - Double-click server\Binary\HealthViewer.bat (opens Notepad on health.txt)

## Developer workflows with the Manifest
Common PowerShell commands (Windows) to work with the manifest:
- Validate JSON syntax
  - Get-Content -Raw Manifest.json | ConvertFrom-Json | Out-Null
- Pretty-print (normalize formatting)
  - $json = Get-Content -Raw Manifest.json | ConvertFrom-Json; $json | ConvertTo-Json -Depth 32 | Set-Content Manifest.json -Encoding UTF8
- Inspect fields quickly
  - (Get-Content -Raw Manifest.json | ConvertFrom-Json).manifest | Format-List

Use in application/tooling code (examples):
- Node.js (TypeScript/JavaScript)
  - const fs = require("fs");
    const manifest = JSON.parse(fs.readFileSync("Manifest.json", "utf8")).manifest;
    // Example policy check
    if (!change.provesNoBreakage && manifest.directives.functionality_guard) {
      throw new Error("Rejecting change: violates functionality_guard");
    }
- Python
  - import json, pathlib
    data = json.loads(pathlib.Path("Manifest.json").read_text("utf-8"))
    manifest = data["manifest"]
    assert manifest["directives"]["improvement_only"]
- PowerShell
  - $data = Get-Content -Raw Manifest.json | ConvertFrom-Json
    $manifest = $data.manifest
    if (-not $manifest.directives.update_balance) { throw "update_balance directive missing" }

Recommended CI hooks (once code repos reference this manifest):
- Parse Manifest.json at build time and expose directives to test frameworks so they can:
  - Enforce stability checks for critical flows (functionality_guard)
  - Fail builds if gameplay balance thresholds are exceeded (update_balance)
  - Gate feature flags on UX acceptance tests (user_experience_first)


## Proposing changes to the Manifest
When you need to evolve the project direction:
1) Open a change proposal (PR) that explains:
   - The specific directive(s) you’re modifying/adding/removing
   - Why the change still preserves ROSE’s essence (reference aesthetic_consistency)
   - The expected player impact (positive) and why risks are unacceptable
   - How quality ratings (scalability, functionality, reliability) will be measured
2) Include acceptance criteria and any rollout/rollback plan
3) After approval, update Manifest.json and notify code repositories that consume it

Review checklist (for maintainers):
- Does the change preserve the visual/emotional essence of ROSE (aesthetic_consistency)?
- Is there a clear, positive player impact (user_experience_first, improvement_only)?
- Is essential functionality protected (functionality_guard)?
- Is gameplay balance maintained (update_balance)?
- Are quality metrics defined and testable (quality_ratings)?

Versioning suggestion:
- Consider tagging this repo (e.g., vX.Y) or embedding a manifest.version field to let downstream code tie behavior to a specific governance version.


## Suggested JSON Schema (optional)
For teams who want stricter validation, the following draft JSON Schema matches the current manifest shape. You can place it in schema/manifest.schema.json and use it in CI with your preferred validator.

{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "AI Rose Evolved Manifest",
  "type": "object",
  "required": ["manifest"],
  "properties": {
    "manifest": {
      "type": "object",
      "required": ["title", "description", "vision", "directives", "heartfelt_note"],
      "properties": {
        "title": { "type": "string", "minLength": 1 },
        "description": { "type": "string", "minLength": 1 },
        "vision": { "type": "string", "minLength": 1 },
        "directives": {
          "type": "object",
          "required": [
            "aesthetic_consistency",
            "user_experience_first",
            "functionality_guard",
            "improvement_only",
            "strategic_AI_development",
            "update_balance",
            "quality_ratings"
          ],
          "properties": {
            "aesthetic_consistency": { "type": "string", "minLength": 1 },
            "user_experience_first": { "type": "string", "minLength": 1 },
            "functionality_guard": { "type": "string", "minLength": 1 },
            "improvement_only": { "type": "string", "minLength": 1 },
            "strategic_AI_development": { "type": "string", "minLength": 1 },
            "update_balance": { "type": "string", "minLength": 1 },
            "quality_ratings": {
              "type": "object",
              "required": ["scalability", "functionality", "reliability"],
              "properties": {
                "scalability": { "type": "string", "minLength": 1 },
                "functionality": { "type": "string", "minLength": 1 },
                "reliability": { "type": "string", "minLength": 1 }
              },
              "additionalProperties": false
            }
          },
          "additionalProperties": false
        },
        "heartfelt_note": { "type": "string", "minLength": 1 }
      },
      "additionalProperties": false
    }
  },
  "additionalProperties": false
}

Validation example (PowerShell with Node + ajv-cli installed globally):
- ajv validate -s schema/manifest.schema.json -d Manifest.json


## Roadmap placeholders (to be replaced as code repos land)
- Link to server runtime repo and content tools once published
- Document build/test commands for those repos
- Define cross-repo integration points (e.g., how content pipelines query or enforce Manifest directives)


## Cultural note
Keep the heartfelt_note in mind. We are building with memory and care. When in doubt, choose options that honor the wonder of the original ROSE—make it feel like it always belonged.

