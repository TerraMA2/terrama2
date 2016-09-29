module.exports = function (app) {
  'use strict';

  var passport = require('../../config/Passport');
  var controller = app.controllers.configuration.Views;

  app.get('/configuration/views', passport.isAuthenticated, controller.get);
  app.get('/configuration/views/new', passport.isAuthenticated, controller.new);
  app.get('/configuration/views/edit/:id', passport.isAuthenticated, controller.edit);
};