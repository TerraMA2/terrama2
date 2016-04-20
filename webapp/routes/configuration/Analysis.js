var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Analysis;

  app.get('/configuration/analyses', passport.isAuthenticated, controller.index);
  app.get('/configuration/analyses/new', passport.isAuthenticated, controller.new);
}
