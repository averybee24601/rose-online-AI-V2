const express = require('express');
const cors = require('cors');
const path = require('path');
const jwt = require('jsonwebtoken');
const bcrypt = require('bcrypt');
const net = require('net');

const config = require('./config');
const pool = require('./db');

const app = express();
app.use(cors());
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Static site
app.use(express.static(path.join(__dirname, 'public')));

// Ensure security table exists (stores secret question/answer)
async function ensureSecurityTable() {
  await pool.query(`CREATE TABLE IF NOT EXISTS account_security (
    id INT AUTO_INCREMENT PRIMARY KEY,
    account_id INT NOT NULL UNIQUE,
    question VARCHAR(255) NOT NULL,
    answer_hash VARCHAR(64) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  ) ENGINE=InnoDB`);
}
ensureSecurityTable().catch(console.error);

// Middleware to get user from auth header if present
app.use((req, res, next) => {
  const auth = req.headers.authorization || '';
  const token = auth.startsWith('Bearer ') ? auth.slice(7) : null;
  if (!token) return next();
  try {
    req.user = jwt.verify(token, config.jwtSecret);
  } catch (e) {
    // ignore invalid
  }
  next();
});

// Register - create account in accounts table
app.post('/api/register', async (req, res) => {
  try {
    const { username, password, email, secretQuestion, secretAnswer } = req.body;
    if (!username || !password || !email || !secretQuestion || !secretAnswer) {
      return res.status(400).json({ error: 'username, password, email, secretQuestion and secretAnswer are required' });
    }
    // username uniqueness
    const [rows] = await pool.query('SELECT id FROM accounts WHERE username = ?', [username]);
    if (rows.length > 0) {
      return res.status(409).json({ error: 'username already exists' });
    }
    // hash per existing schema (md5)
    const salt = await bcrypt.genSalt(10);
    const passHash = await bcrypt.hash(password, salt);
    const [result] = await pool.query(
      'INSERT INTO accounts (username, password, email, active) VALUES (?, ?, ?, 1)',
      [username, passHash, email]
    );
    const accountId = result.insertId;
    // store security info (answer hashed)
    const answerHash = md5(secretAnswer);
    await pool.query(
      'INSERT INTO account_security (account_id, question, answer_hash) VALUES (?, ?, ?)',
      [accountId, secretQuestion, answerHash]
    );
    return res.json({ ok: true, id: accountId });
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'server error' });
  }
});

// Login - validate against accounts table
app.post('/api/login', async (req, res) => {
  try {
    const { username, password } = req.body;
    if (!username || !password) return res.status(400).json({ error: 'username and password are required' });
    const [rows] = await pool.query(
      'SELECT id, username, password, accesslevel FROM accounts WHERE username = ? AND active = 1',
      [username]
    );
    if (rows.length === 0) return res.status(401).json({ error: 'invalid credentials' });
    const user = rows[0];
    const validPassword = await bcrypt.compare(password, user.password);
    if (!validPassword) return res.status(401).json({ error: 'invalid credentials' });
    const token = jwt.sign({ id: user.id, username: user.username, accesslevel: user.accesslevel }, config.jwtSecret, { expiresIn: '7d' });
    return res.json({ ok: true, token });
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'server error' });
  }
});

// Rankings - top level
app.get('/api/rankings/level', async (req, res) => {
  try {
    const [rows] = await pool.query(
      'SELECT char_name, level, account_name FROM characters ORDER BY level DESC, exp DESC LIMIT 20'
    );
    return res.json(rows);
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'server error' });
  }
});

// Rankings - top zuly
app.get('/api/rankings/zulie', async (req, res) => {
  try {
    const [rows] = await pool.query(
      'SELECT char_name, zuly, account_name FROM characters ORDER BY zuly DESC LIMIT 20'
    );
    return res.json(rows);
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'server error' });
  }
});

// Rankings - PvP placeholder: using honor column for now
app.get('/api/rankings/pvp', async (req, res) => {
  try {
    const [rows] = await pool.query(
      'SELECT char_name, honor, valor, account_name FROM characters ORDER BY honor DESC, valor DESC LIMIT 20'
    );
    return res.json(rows);
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'server error' });
  }
});

// Server status - TCP probe
app.get('/api/status', async (req, res) => {
  const socket = new net.Socket();
  let done = false;
  const finish = (online) => {
    if (done) return; done = true;
    try { socket.destroy(); } catch {}
    res.json({ online, host: config.status.host, port: config.status.port });
  };
  socket.setTimeout(2000);
  socket.on('connect', () => finish(true));
  socket.on('timeout', () => finish(false));
  socket.on('error', () => finish(false));
  socket.connect(config.status.port, config.status.host);
});

// Explicit routes for multi-page site (static files handle most cases)
app.get('/', (req, res) => res.sendFile(path.join(__dirname, 'public', 'index.html')));
app.get('/rankings', (req, res) => res.sendFile(path.join(__dirname, 'public', 'rankings.html')));
app.get('/download', (req, res) => res.sendFile(path.join(__dirname, 'public', 'download.html')));
app.get('/status', (req, res) => res.sendFile(path.join(__dirname, 'public', 'status.html')));
app.get('/login', (req, res) => res.sendFile(path.join(__dirname, 'public', 'login.html')));
app.get('/register', (req, res) => res.sendFile(path.join(__dirname, 'public', 'register.html')));

// 404 fallback
app.use((req, res) => {
  res.status(404).sendFile(path.join(__dirname, 'public', 'index.html'));
});

app.listen(config.port, () => {
  console.log(`AI Rose: Evolved web running on port ${config.port}`);
});

