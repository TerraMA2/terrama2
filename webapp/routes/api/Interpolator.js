'use strict';

var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Interpolator;

  app.post(app.locals.BASE_URL + 'api/Interpolator', passport.isAuthenticated, controller.post);
  app.get(app.locals.BASE_URL + 'api/Interpolator', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/Interpolator/:id', passport.isAuthenticated, controller.get);
  app.put(app.locals.BASE_URL + 'api/Interpolator/:id', passport.isAuthenticated, controller.put);
};