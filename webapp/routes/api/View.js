
'use strict';

var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.View;

  app.post('/api/View', passport.isAuthenticated, controller.post);
  app.get('/api/View', passport.isAuthenticated, controller.get);
  app.get('/api/View/:id', passport.isAuthenticated, controller.get);
  app.get('/api/ViewByProject/:project_id', passport.isAuthenticated, controller.get);
  app.put('/api/View/:id', passport.isAuthenticated, controller.put);
  app.delete("/api/View/:id/delete", passport.isAuthenticated, controller.delete);
};