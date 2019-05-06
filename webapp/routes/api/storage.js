
var passport = require("../../config/Passport");

module.exports = function(app) {
  var controllerStorage = app.controllers.api.storage;

  app.get(app.locals.BASE_URL + "api/storages/", passport.isAuthenticated, controllerStorage.get);
  app.post(app.locals.BASE_URL + "api/storages", passport.isAuthenticated, controllerStorage.save);
  app.get(app.locals.BASE_URL + "api/storages/:id", passport.isAuthenticated, controllerStorage.get);
  app.put(app.locals.BASE_URL + "api/storages/:id", passport.isAuthenticated, controllerStorage.put);
  app.delete(app.locals.BASE_URL + "api/storages/:id", passport.isAuthenticated, controllerStorage.delete);
};
