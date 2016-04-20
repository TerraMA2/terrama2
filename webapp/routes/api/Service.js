module.exports = function (app) {

  var controller = app.controllers.api.Service;

  app.get('/api/Service/', controller.get);
  app.post('/api/Service/', controller.post);

};