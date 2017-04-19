'use strict';

var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Alert;

  app.post('/api/Alert', passport.isAuthenticated, controller.post);
  app.get('/api/Alert', passport.isAuthenticated, controller.get);
  app.get('/api/Alert/:id', passport.isAuthenticated, controller.get);
  app.put('/api/Alert/:id', passport.isAuthenticated, controller.put);
  app.delete("/api/Alert/:id/delete", passport.isAuthenticated, controller.delete);
};