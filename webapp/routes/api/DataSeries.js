module.exports = function (app) {

  var controller = app.controllers.api.DataSeries;

  app.post('/api/DataSeries/', controller.post);

};
