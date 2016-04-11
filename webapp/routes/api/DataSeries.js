module.exports = function (app) {

  var controller = app.controllers.api.DataSeries;

  app.get('/api/DataSeries/', controller.get);
  app.post('/api/DataSeries/', controller.post);
  app.get('/api/DataSeries/:id', controller.get);

};
