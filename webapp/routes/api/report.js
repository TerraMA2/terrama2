module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.report;

  app.get(app.locals.BASE_URL + 'api/report', controller.get);
  app.get(app.locals.BASE_URL + 'api/report/getReport', controller.getReport);
  app.get(app.locals.BASE_URL + 'api/report/getPropertiesData', controller.getPropertiesData);
};