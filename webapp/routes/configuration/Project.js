var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Project;

  app.get('/configuration/projects', passport.isAuthenticated, controller.get);
  app.get('/configuration/projects/new', passport.isAuthenticated, controller.new);
  app.get('/configuration/projects/:name/activate', passport.isAuthenticated, controller.activate);
  app.get('/configuration/projects/:name/activate/:token', passport.isAuthenticated, controller.activate);
  app.get('/configuration/projects/edit/:name', passport.isAuthenticated, controller.edit);
};
