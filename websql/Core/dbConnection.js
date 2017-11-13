//connect with postgresql

const pg = require('pg');
const connectionString = process.env.DATABASE_URL || 'postgres://postgres:123@localhost:5432/websql';
const client = new pg.Client(connectionString);
client.connect();

