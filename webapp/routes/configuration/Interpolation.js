module.exports = function (app) {
  'use strict';

  var passport = require('../../config/Passport');
  var controller = app.controllers.configuration.Interpolation;

  app.get(app.locals.BASE_URL + 'configuration/interpolation/new', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/interpolation/edit/:id', passport.isAuthenticated, controller.edit);
};