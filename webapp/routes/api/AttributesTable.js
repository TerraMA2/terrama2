module.exports = function (app) {
  'use strict';

  var controller = app.controllers.api.AttributesTable;

  app.get(app.locals.BASE_URL + 'api/datasetid', controller.getDataSetId);
  app.get(app.locals.BASE_URL + 'api/attributes', controller.getAttributes);
  
  app.get(app.locals.BASE_URL + 'api/attributesByDataSerie', controller.getAttributesByDataSerie);
  app.get(app.locals.BASE_URL + 'api/datasetidByDataSerie', controller.getDataSetIdByDataSerie);

};