module.exports = function (app) {
  var controller = app.controllers.configuration.Exportation;

  app.get(app.locals.BASE_URL + 'export', controller.exportData);
  app.get(app.locals.BASE_URL + 'export-grid', controller.exportGridFile);
  app.get(app.locals.BASE_URL + 'check-grid', controller.checkGridFile);
}