module.exports = function (app) {

  var controller = app.controllers.api.DataFormat;

  app.get('/api/DataFormat/', controller.get);

};