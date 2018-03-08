module.exports = function (app) {
  'use strict';

  var passport = require('../../config/Passport');
  var controller = app.controllers.configuration.Views;

  app.get(app.locals.BASE_URL + 'configuration/views', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'configuration/views/new', passport.isAuthenticated, controller.new);
  app.get(app.locals.BASE_URL + 'configuration/views/edit/:id', passport.isAuthenticated, controller.edit);
};