# AI Rose: Evolved Web

A futuristic, cyberpunk-themed website integrated with your existing Evolution MySQL schema for accounts and characters.

What’s included
- Registration and login (compatible with existing MD5 password hashing)
- Rankings: Level, Zulie, PvP (honor/valor placeholder)
- Server status (TCP probe to configured host:port)
- Download section (serves static client archive)
- Blue/black neon aesthetic single-page UI

Setup
1) Install Node.js 18+.
2) Copy web/.env.example to web/.env and update values:
   - MYSQL_HOST, MYSQL_PORT, MYSQL_USER, MYSQL_PASSWORD, MYSQL_DATABASE (defaults set to localhost:3307, DB=roseon)
   - SERVER_STATUS_HOST, SERVER_STATUS_PORT (e.g., your game channel host:port)
   - JWT_SECRET
3) Install dependencies:
   - In PowerShell: npm install --prefix web
4) Place your client archive at:
   - web/public/downloads/AI-Rose-Evolved-Client.zip

Run
- Development: npm run dev --prefix web
- Production: npm start --prefix web

Endpoints
- POST /api/register { username, password, email? }
- POST /api/login { username, password }
- GET  /api/rankings/level
- GET  /api/rankings/zulie
- GET  /api/rankings/pvp
- GET  /api/status

Notes
- This site reads from the existing Evolution database. The accounts table stores MD5 hashes; this matches your Evolution.sql schema. If you later migrate to a stronger hash, update server.js accordingly.
- PvP ranking currently uses honor/valor columns. If you add a kill counter column in the future, we can extend the endpoint.
- The About and footer sections incorporate content and tone from Manifest.json to emphasize AI-led development and quality gates.

