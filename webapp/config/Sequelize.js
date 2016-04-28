var Sequelize = require('sequelize');
var fs = require('fs');
var path = require('path');

// reading terrama2 database file configuration
var config = JSON.parse(fs.readFileSync(path.join(__dirname, 'config.terrama2'), 'utf-8'));

var connection = new Sequelize(config.database, config.username, config.password, config);

module.exports = connection;
