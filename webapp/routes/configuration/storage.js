
var passport = require("../../config/Passport");

module.exports = function(app) {
  var controllerStorage = app.controllers.configuration.storage;

  app.get(app.locals.BASE_URL + "configuration/storages/", passport.isAuthenticated, controllerStorage.get);
  app.get(app.locals.BASE_URL + "configuration/storages/new", passport.isAuthenticated, controllerStorage.new);
  app.get(app.locals.BASE_URL + "configuration/storages/:id", passport.isAuthenticated, controllerStorage.edit);
};
