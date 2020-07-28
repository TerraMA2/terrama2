var passport = require('../../core/utility/Passport');

module.exports = function (app) {

  var controller = app.controllers.api.Service;

  app.get(app.locals.BASE_URL + 'api/Service/', passport.isAuthenticated, controller.get);
  app.post(app.locals.BASE_URL + 'api/Service/', passport.isAuthenticated, controller.post);
  app.put(app.locals.BASE_URL + 'api/Service/:id', passport.isAuthenticated, controller.put);
  app.delete(app.locals.BASE_URL + 'api/Service/:id/delete', passport.isAuthenticated, controller.delete);

};
