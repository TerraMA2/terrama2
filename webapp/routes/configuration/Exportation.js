module.exports = function (app) {
  var controller = app.controllers.configuration.Exportation;

  app.get(app.locals.BASE_URL + 'export', controller);
}