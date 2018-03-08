var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.DataProvider;

  app.get(app.locals.BASE_URL + 'configuration/providers', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'configuration/providers/new', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/providers/edit/:id', passport.isAuthenticated, controller.edit);
};
