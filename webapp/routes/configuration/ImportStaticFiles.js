var multiparty = require('connect-multiparty');
var multipartyMiddleware = multiparty();

module.exports = function(app) {
  var controller = app.controllers.configuration.ImportStaticFiles;

  app.post(app.locals.BASE_URL + 'import-shapefile', multipartyMiddleware, controller.importShapefile);
  app.post(app.locals.BASE_URL + 'import-geotiff', multipartyMiddleware, controller.importGeoTIFF);
  app.post(app.locals.BASE_URL + 'import-file', multipartyMiddleware, controller.importFile);
};