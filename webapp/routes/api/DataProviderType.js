module.exports = function (app) {

  var controller = app.controllers.api.DataProviderType;

  app.get('/api/DataProviderType/', controller.get);

};