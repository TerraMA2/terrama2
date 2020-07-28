'use strict';

var passport = require('../../core/utility/Passport');

module.exports = function(app) {
  var controller = app.controllers.api.Legend;

  app.post(app.locals.BASE_URL + 'api/Legend', passport.isAuthenticated, controller.post);
  app.get(app.locals.BASE_URL + 'api/Legend', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/Legend/:id', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/LegendByProject/:project_id', passport.isAuthenticated, controller.get);
  app.put(app.locals.BASE_URL + 'api/Legend/:id', passport.isAuthenticated, controller.put);
  app.delete(app.locals.BASE_URL + "api/Legend/:id/delete", passport.isAuthenticated, controller.delete);
};