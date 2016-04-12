module.exports = function (app) {

  var controller = app.controllers.api.DataSeriesSemantics;

  app.get('/api/DataSeriesSemantics/', controller.get);
  app.get('/api/DataSeriesSemantics/:name', controller.get);

};