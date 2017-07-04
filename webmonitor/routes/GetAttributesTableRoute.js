module.exports = function(app) {
  var controller = app.controllers.GetAttributesTableController;

  app.post(app.locals.BASE_URL + 'get-attributes-table', controller.getAttributesTable);
  app.get(app.locals.BASE_URL + 'get-columns', controller.getColumns);
  app.get(app.locals.BASE_URL + 'get-legend', controller.getLegend);
};