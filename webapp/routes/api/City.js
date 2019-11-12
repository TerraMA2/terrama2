module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.City;

  app.get(app.locals.BASE_URL + 'api/city/getAll', controller.getAll);
  app.get(app.locals.BASE_URL + 'api/city/getAllRegions', controller.getAllRegions);
  app.get(app.locals.BASE_URL + 'api/city/getAllMesoregions', controller.getAllMesoregions);
  app.get(app.locals.BASE_URL + 'api/city/getAllMicroregions', controller.getAllMicroregions);
};
