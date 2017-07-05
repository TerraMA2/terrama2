var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Legends;

  app.get(app.locals.BASE_URL + 'configuration/legends/new', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/legends/edit/:id', passport.isAuthenticated, controller.edit);
}
