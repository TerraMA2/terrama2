
var passport = require("../../config/Passport");

module.exports = function(app) {
  var controllerStorage = app.controllers.administration.Storage;

  app.get(app.locals.BASE_URL + "administration/storages/", passport.isAuthenticated, controllerStorage.get);
  app.get(app.locals.BASE_URL + "administration/storages/new", passport.isAuthenticated, controllerStorage.new);
  app.get(app.locals.BASE_URL + "administration/storages/:id", passport.isAuthenticated, controllerStorage.edit);
};
