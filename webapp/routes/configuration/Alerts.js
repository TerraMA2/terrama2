var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Alerts;

  app.get(app.locals.BASE_URL + 'configuration/alerts', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'configuration/alerts/new', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/alerts/edit/:id', passport.isAuthenticated, controller.edit);
}
