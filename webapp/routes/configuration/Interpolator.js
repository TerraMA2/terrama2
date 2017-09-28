module.exports = function (app) {
  'use strict';

  var passport = require('../../config/Passport');
  var controller = app.controllers.configuration.Interpolator;

  app.get(app.locals.BASE_URL + 'configuration/interpolator/new/:input_ds', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/interpolator/edit/:id', passport.isAuthenticated, controller.edit);
};