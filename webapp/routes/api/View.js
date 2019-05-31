
'use strict';

var passport = require('../../core/utility/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.View;

  app.post(app.locals.BASE_URL + 'api/View', passport.isAuthenticated, controller.post);
  app.get(app.locals.BASE_URL + 'api/View', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/View/:id', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/ViewByProject/:project_id', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/ViewByService/:service_id/:project_id', passport.isAuthenticated, controller.listByService);
  app.put(app.locals.BASE_URL + 'api/View/:id', passport.isAuthenticated, controller.put);
  app.delete(app.locals.BASE_URL + "api/View/:id/delete", passport.isAuthenticated, controller.delete);
  app.get(app.locals.BASE_URL + 'api/View/changeStatus/:id', passport.isAuthenticated, controller.changeStatus);
};