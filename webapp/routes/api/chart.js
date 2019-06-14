module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.chart;

  app.get(app.locals.BASE_URL + 'api/charts', controller.get);
};