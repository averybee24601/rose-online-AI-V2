const mysql = require('mysql2/promise');
const config = require('./config');

// Create a connection pool to the MySQL database.
// This allows us to reuse connections, which is more efficient than creating a new connection for every query.
const pool = mysql.createPool({
  host: config.db.host,
  port: config.db.port,
  user: config.db.user,
  password: config.db.password,
  database: config.db.database,
  waitForConnections: true,
  connectionLimit: 10,
  queueLimit: 0,
});

module.exports = pool;

