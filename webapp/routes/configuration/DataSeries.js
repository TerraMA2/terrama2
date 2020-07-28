var passport = require("../../core/utility/Passport");

module.exports = function(app) {
  var controllerDynamic = app.controllers.configuration.DynamicDataSeries;
  var controllerStatic = app.controllers.configuration.StaticDataSeries;

  app.get(app.locals.BASE_URL + "configuration/dynamic/dataseries/", passport.isAuthenticated, controllerDynamic.get);
  app.get(app.locals.BASE_URL + "configuration/dynamic/dataseries/new", passport.isAuthenticated, controllerDynamic.new);
  app.get(app.locals.BASE_URL + "configuration/dynamic/dataseries/:id", passport.isAuthenticated, controllerDynamic.edit);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/storeDcps", passport.isAuthenticated, controllerDynamic.storeDcps);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/storeDcpsStore", passport.isAuthenticated, controllerDynamic.storeDcpsStore);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/paginateDcps", passport.isAuthenticated, controllerDynamic.paginateDcps);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/paginateDcpsStore", passport.isAuthenticated, controllerDynamic.paginateDcpsStore);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/removeStoredDcp", passport.isAuthenticated, controllerDynamic.removeStoredDcp);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/removeStoredDcpStore", passport.isAuthenticated, controllerDynamic.removeStoredDcpStore);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/updateDcp", passport.isAuthenticated, controllerDynamic.updateDcp);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/updateDcpStore", passport.isAuthenticated, controllerDynamic.updateDcpStore);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/clearDcpsStore", passport.isAuthenticated, controllerDynamic.clearDcpsStore);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/deleteDcpsKey", passport.isAuthenticated, controllerDynamic.deleteDcpsKey);
  app.post(app.locals.BASE_URL + "configuration/dynamic/dataseries/deleteDcpsStoreKey", passport.isAuthenticated, controllerDynamic.deleteDcpsStoreKey);
  app.get(app.locals.BASE_URL + "configuration/static/dataseries/", passport.isAuthenticated, controllerStatic.get);
  app.get(app.locals.BASE_URL + "configuration/static/dataseries/new", passport.isAuthenticated, controllerStatic.new);
  app.get(app.locals.BASE_URL + "configuration/static/dataseries/:id", passport.isAuthenticated, controllerStatic.edit);

};
