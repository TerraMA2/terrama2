var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Project;

  app.get(app.locals.BASE_URL + 'api/Project/', passport.isAuthenticated, controller.get);
  app.post(app.locals.BASE_URL + 'api/Project/', passport.isAuthenticated, controller.post);
  app.get(app.locals.BASE_URL + 'api/Project/:id', passport.isAuthenticated, controller.get);
  app.put(app.locals.BASE_URL + 'api/Project/:id', passport.isAuthenticated, controller.put);
  app.delete(app.locals.BASE_URL + 'api/Project/:id/delete', passport.isAuthenticated, controller.delete);
};
