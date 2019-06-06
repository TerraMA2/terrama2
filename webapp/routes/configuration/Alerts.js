var passport = require('../../core/utility/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Alerts;

  app.get(app.locals.BASE_URL + 'configuration/alerts', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'configuration/alerts/legends', passport.isAuthenticated, controller.getLegends);
  app.get(app.locals.BASE_URL + 'configuration/alerts/new', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/alerts/edit/:id', passport.isAuthenticated, controller.edit);
}
