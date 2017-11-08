var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Project;

  app.get(app.locals.BASE_URL + 'configuration/projects', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'configuration/projects/new', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/projects/:name/activate', passport.isAuthenticated, controller.activate);
  app.get(app.locals.BASE_URL + 'configuration/projects/:name/activate/:token', passport.isAuthenticated, controller.activate);
  app.get(app.locals.BASE_URL + 'configuration/projects/edit/:name', passport.isAuthenticated, controller.edit);
  app.get(app.locals.BASE_URL + 'configuration/projects/updateCache', passport.isAuthenticated, controller.updateCache);
};
