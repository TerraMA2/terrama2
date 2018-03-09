'use strict';

var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Alert;

  app.post(app.locals.BASE_URL + 'api/Alert', passport.isAuthenticated, controller.post);
  app.get(app.locals.BASE_URL + 'api/Alert', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/Alert/:id', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/AlertByProject/:project_id', passport.isAuthenticated, controller.get);
  app.put(app.locals.BASE_URL + 'api/Alert/:id', passport.isAuthenticated, controller.put);
  app.delete(app.locals.BASE_URL + "api/Alert/:id/delete", passport.isAuthenticated, controller.delete);
  app.get(app.locals.BASE_URL + 'api/Alert/changeStatus/:id', passport.isAuthenticated, controller.changeStatus);
};