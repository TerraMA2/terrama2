var passport = require("../../config/Passport");

module.exports = function(app) {
  var controllerDynamic = app.controllers.configuration.DynamicDataSeries;
  var controllerStatic = app.controllers.configuration.StaticDataSeries;

  app.get("/configuration/dynamic/dataseries/", passport.isAuthenticated, controllerDynamic.get);
  app.get("/configuration/dynamic/dataseries/new", passport.isAuthenticated, controllerDynamic.new);
  app.get("/configuration/dynamic/dataseries/:id", passport.isAuthenticated, controllerDynamic.edit);

  app.get("/configuration/static/dataseries/", passport.isAuthenticated, controllerStatic.get);
  app.get("/configuration/static/dataseries/new", passport.isAuthenticated, controllerStatic.new);
  app.get("/configuration/static/dataseries/:id", passport.isAuthenticated, controllerStatic.edit);

};