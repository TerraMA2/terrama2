module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.Login;

  app.post(app.locals.BASE_URL + 'api/login', controller.login);
};