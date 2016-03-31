module.exports = function (app) {

  var controller = app.controllers.api.DataSeriesSemantics;

  app.get('/api/DataSeriesSemantics/', controller.get);

};