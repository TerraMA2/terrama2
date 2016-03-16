module.exports = function (app) {

  var controller = app.controllers.api.DataProvider;

  app.get('/api/DataProvider/', controller.get);
  app.post('/api/DataProvider/', controller.post);

};