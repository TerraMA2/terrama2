module.exports = function (app) {

  var controller = app.controllers.configuration.DataProvider;
  app.get('/configuration/providers', controller.get);
  app.get('/configuration/providers/new', controller.new);
  app.get('/configuration/providers/edit/:name', controller.edit);

};