module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.City;

  app.get(app.locals.BASE_URL + 'api/city/getAll', controller.getAll);
};
