var passport = require("../../config/Passport");

module.exports = function(app) {
  var controllerDynamic = app.controllers.configuration.DynamicDataSeries;
  var controllerStatic = app.controllers.configuration.StaticDataSeries;

  app.get("/configuration/dynamic/dataseries/", passport.isAuthenticated, controllerDynamic.get);
  app.get("/configuration/dynamic/dataseries/new", passport.isAuthenticated, controllerDynamic.new);
  app.get("/configuration/dynamic/dataseries/:id", passport.isAuthenticated, controllerDynamic.edit);
  app.post("/configuration/dynamic/dataseries/storeDcps", passport.isAuthenticated, controllerDynamic.storeDcps);
  app.post("/configuration/dynamic/dataseries/storeDcpsStore", passport.isAuthenticated, controllerDynamic.storeDcpsStore);
  app.post("/configuration/dynamic/dataseries/paginateDcps", passport.isAuthenticated, controllerDynamic.paginateDcps);
  app.post("/configuration/dynamic/dataseries/paginateDcpsStore", passport.isAuthenticated, controllerDynamic.paginateDcpsStore);
  app.post("/configuration/dynamic/dataseries/removeStoredDcp", passport.isAuthenticated, controllerDynamic.removeStoredDcp);
  app.post("/configuration/dynamic/dataseries/removeStoredDcpStore", passport.isAuthenticated, controllerDynamic.removeStoredDcpStore);
  app.post("/configuration/dynamic/dataseries/updateDcp", passport.isAuthenticated, controllerDynamic.updateDcp);
  app.post("/configuration/dynamic/dataseries/updateDcpStore", passport.isAuthenticated, controllerDynamic.updateDcpStore);
  app.get("/configuration/static/dataseries/", passport.isAuthenticated, controllerStatic.get);
  app.get("/configuration/static/dataseries/new", passport.isAuthenticated, controllerStatic.new);
  app.get("/configuration/static/dataseries/:id", passport.isAuthenticated, controllerStatic.edit);

};
