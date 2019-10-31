module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.report;

  app.get(app.locals.BASE_URL + 'api/report/getCarData', controller.getCarData);
  app.get(app.locals.BASE_URL + 'api/report/getAnalysisData', controller.getAnalysisData);
  app.get(app.locals.BASE_URL + 'api/report/getStaticData', controller.getStaticData);
  app.get(app.locals.BASE_URL + 'api/report/getDynamicData', controller.getDynamicData);
  app.get(app.locals.BASE_URL + 'api/report/getAnalysisTotals', controller.getAnalysisTotals);
  app.get(app.locals.BASE_URL + 'api/report/getViewsDetails', controller.getViewsDetails);
};
