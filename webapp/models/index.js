var fs = require("fs");
var path = require("path");
var Sequelize = require("sequelize");
const Application = require('./../core/Application');

const sequelize = require('./../config/Sequelize')(Application.getContextConfig().db);
const isFunction = require("./../core/Utils").isFunction;

const db = {};

fs.readdirSync(__dirname).filter(function(file) {
  return (file.indexOf(".") !== 0) && (file !== "index.js");
}).forEach(function(file) {
  var model = sequelize.import(path.join(__dirname, file));
  db[model.name] = model;
});

Object.keys(db).forEach(function(modelName) {
  if ("associate" in db[modelName]) {
    db[modelName].associate(db);

    // Register Hooks if there is
    if (isFunction(db[modelName].registerHooks))
      db[modelName].registerHooks(db)
  }
});

db.sequelize = sequelize;
db.Sequelize = Sequelize;

module.exports = {
  ...db,
};
