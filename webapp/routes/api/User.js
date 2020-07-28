'use strict';

module.exports = function(app) {
  var passport = require('./../../core/utility/Passport');
  var controller = app.controllers.api.User;

  app.get(app.locals.BASE_URL + "api/users", passport.isAuthenticated, controller.get);
  app.post(app.locals.BASE_URL + "api/users/", passport.isAuthenticated, controller.post);
  app.get(app.locals.BASE_URL + "api/users/:userId", passport.isAuthenticated, controller.get);
  app.put(app.locals.BASE_URL + "api/users/:userId", passport.isAuthenticated, controller.put);
};