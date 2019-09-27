module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.report;

  app.get(app.locals.BASE_URL + 'api/report', controller.get);
  app.get(app.locals.BASE_URL + 'api/report/getData', controller.getData);
  app.get(app.locals.BASE_URL + 'api/report/getPropertiesData', controller.getPropertiesData);
  app.get(app.locals.BASE_URL + 'api/report/getCarData', controller.getCarData);
  app.get(app.locals.BASE_URL + 'api/report/getAnalysisData', controller.getAnalysisData);
};