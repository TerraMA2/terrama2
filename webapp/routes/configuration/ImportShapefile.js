var multiparty = require('connect-multiparty');
var multipartyMiddleware = multiparty();

module.exports = function(app) {
  var controller = app.controllers.configuration.ImportShapefile;

  app.post(app.locals.BASE_URL + 'import-shapefile', multipartyMiddleware, controller);
};