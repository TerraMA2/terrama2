module.exports = function (app) {
  'use strict';

  const dashboardController = app.controllers.api.Dashboard;

  app.get(app.locals.BASE_URL + 'api/dashboard/getAnalysisTotals', dashboardController.getAnalysisTotals);
  app.get(app.locals.BASE_URL + 'api/dashboard/getDetailsAnalysisTotals', dashboardController.getDetailsAnalysisTotals);
};
