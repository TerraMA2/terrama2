var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.DataProvider;

  app.get('/configuration/providers', passport.isAuthenticated, controller.get);
  app.get('/configuration/providers/new', passport.isAuthenticated, controller.new);
  app.get('/configuration/providers/edit/:id', passport.isAuthenticated, controller.edit);
};
