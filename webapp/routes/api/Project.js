module.exports = function (app) {

  var controller = app.controllers.api.Project;

  app.get('/api/Project/', controller.get);
  app.post('/api/Project/', controller.post);

};