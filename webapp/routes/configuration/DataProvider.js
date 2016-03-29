module.exports = function (app) {

  var controller = app.controllers.configuration.DataProvider;
  app.get('/configuration/provider', controller.get);
  app.get('/configuration/provider/:name', controller.get);

};