var passport = require("../../config/Passport");

module.exports = function(app) {
  var controller = app.controllers.configuration;

  app.get("/configuration/dynamic/dataseries/", passport.isAuthenticated, controller.DynamicDataSeries);
  app.get("/configuration/static/dataseries/", passport.isAuthenticated, controller.StaticDataSeries);

};