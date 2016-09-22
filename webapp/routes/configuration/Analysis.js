var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Analysis;

  app.get('/configuration/analysis', passport.isAuthenticated, controller.get);
  app.get('/configuration/analysis/new', passport.isAuthenticated, controller.new);
  app.get("/configuration/analysis/:id/edit", passport.isAuthenticated, controller.edit);
}
