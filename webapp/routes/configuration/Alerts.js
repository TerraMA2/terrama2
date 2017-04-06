var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Alerts;

  app.get('/configuration/alerts', passport.isAuthenticated, controller.index);
  app.get('/configuration/alerts/new', passport.isAuthenticated, controller.new);
}
