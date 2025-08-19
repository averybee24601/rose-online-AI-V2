const dotenv = require('dotenv');
const path = require('path');

// Load environment variables from a .env file into process.env
dotenv.config({ path: path.resolve(__dirname, '.env') });

// Export the configuration object
module.exports = {
  // The port that the web server will listen on
  port: process.env.PORT || 8080,
  // The current environment (development, production, etc.)
  env: process.env.NODE_ENV || 'development',
  // The secret key used to sign JWTs
  jwtSecret: process.env.JWT_SECRET || 'dev_secret',
  // The database configuration
  db: {
    host: process.env.MYSQL_HOST || 'localhost',
    port: Number(process.env.MYSQL_PORT || 3307),
    user: process.env.MYSQL_USER || 'root',
    password: process.env.MYSQL_PASSWORD || '',
    database: process.env.MYSQL_DATABASE || 'roseon',
  },
  // The server status configuration
  status: {
    host: process.env.SERVER_STATUS_HOST || 'localhost',
    port: Number(process.env.SERVER_STATUS_PORT || 29000),
  },
};

