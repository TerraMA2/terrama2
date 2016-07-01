var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Analysis;

  app.get('/configuration/analyses', passport.isAuthenticated, controller.get);
  app.get('/configuration/analyses/new', passport.isAuthenticated, controller.new);
  app.get("/configuration/analyses/:id/edit", passport.isAuthenticated, controller.edit);
}
