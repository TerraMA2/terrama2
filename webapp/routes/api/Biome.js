module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.Biome;

  app.get(app.locals.BASE_URL + 'api/biome/getAll', controller.getAll);
};
