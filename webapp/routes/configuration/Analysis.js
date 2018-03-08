var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Analysis;

  app.get(app.locals.BASE_URL + 'configuration/analysis', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'configuration/analysis/new', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/analysis/:id/edit', passport.isAuthenticated, controller.edit);
}
