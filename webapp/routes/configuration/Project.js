var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Project;

  app.get(app.locals.BASE_URL + 'configuration/projects', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'configuration/projects/new', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/projects/:name/activate', passport.isAuthenticated, controller.activate);
  // The route below was created only for the first system load, because of a bug with projects with special characters in the name
  app.get(app.locals.BASE_URL + 'configuration/projects/:id/id/activate', passport.isAuthenticated, controller.activateById);
  app.get(app.locals.BASE_URL + 'configuration/projects/:name/activate/:token', passport.isAuthenticated, controller.activate);
  app.get(app.locals.BASE_URL + 'configuration/projects/edit/:name', passport.isAuthenticated, controller.edit);
  app.get(app.locals.BASE_URL + 'configuration/projects/updateCache', passport.isAuthenticated, controller.updateCache);
};
