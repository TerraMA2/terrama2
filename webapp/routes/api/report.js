module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.report;

  app.get(app.locals.BASE_URL + 'api/report', controller.get);
};