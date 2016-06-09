var passport = require("../../config/Passport");

module.exports = function(app) {
  var controllerDynamic = app.controllers.configuration.DynamicDataSeries;
  var controllerStatic = app.controllers.configuration.StaticDataSeries;

  app.get("/configuration/dynamic/dataseries/", passport.isCommonUser, controllerDynamic.get);
  app.get("/configuration/dynamic/dataseries/new", passport.isCommonUser, controllerDynamic.new);
  app.get("/configuration/dynamic/dataseries/:id", passport.isCommonUser, controllerDynamic.edit);

  app.get("/configuration/static/dataseries/", passport.isCommonUser, controllerStatic.get);
  app.get("/configuration/static/dataseries/new", passport.isCommonUser, controllerStatic.new);
  app.get("/configuration/static/dataseries/:id", passport.isCommonUser, controllerStatic.edit);

};
