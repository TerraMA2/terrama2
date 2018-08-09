var fs = require("fs");
var db = {};
var path = require("path");
var Sequelize = require("sequelize");

const isFunction = require("./../core/Utils").isFunction;


function load(sequelizeObject) {
  fs.readdirSync(__dirname).filter(function(file) {
    return (file.indexOf(".") !== 0) && (file !== "index.js");
  }).forEach(function(file) {
    var model = sequelizeObject.import(path.join(__dirname, file));
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

  db.sequelize = sequelizeObject;
  db.Sequelize = Sequelize;
}

module.exports = function() {
  return {
    db: db,
    load: load
  };
};
