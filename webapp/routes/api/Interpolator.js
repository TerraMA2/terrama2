'use strict';

var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Interpolator;

  app.post(app.locals.BASE_URL + 'api/interpolator', passport.isAuthenticated, controller.post);
  app.get(app.locals.BASE_URL + 'api/interpolator', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/interpolator/:id', passport.isAuthenticated, controller.get);
  app.put(app.locals.BASE_URL + 'api/interpolator/:id', passport.isAuthenticated, controller.put);
  app.delete(app.locals.BASE_URL + "api/interpolator/:id/delete", passport.isAuthenticated, controller.delete);
};