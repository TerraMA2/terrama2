var Sequelize = require('sequelize');

var config =  {
  "username": "postgres",
  "password": "postgres",
  "database": "nodejs",
  "host": "127.0.0.1",
  "dialect": "postgres",
  define: {
    schema: "terrama2"
  }
};

var connection = new Sequelize(config.database, config.username, config.password, config);

module.exports = connection;
