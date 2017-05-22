var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Analysis;

  app.get(app.locals.BASE_URL + 'api/Analysis/', passport.isAuthenticated, controller.get);
  app.post(app.locals.BASE_URL + 'api/Analysis/', passport.isAuthenticated, controller.post);
  app.put(app.locals.BASE_URL + 'api/Analysis/:id', passport.isAuthenticated, controller.put);
  app.delete(app.locals.BASE_URL + 'api/Analysis/:id/delete', passport.isAuthenticated, controller.delete);

  app.post(app.locals.BASE_URL + "api/Analysis/validate", passport.isAuthenticated, controller.validate);
};
