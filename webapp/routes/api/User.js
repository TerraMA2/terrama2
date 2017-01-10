'use strict';

module.exports = function(app) {
  var passport = require('./../../config/Passport');
  var controller = app.controllers.api.User;

  app.get("/api/users", passport.isAuthenticated, controller.get);
  app.post("/api/users/", passport.isAuthenticated, controller.post);
  app.get("/api/users/:userId", passport.isAuthenticated, controller.get);
  app.put("/api/users/:userId", passport.isAuthenticated, controller.put);
};