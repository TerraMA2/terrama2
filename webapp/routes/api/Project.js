module.exports = function (app) {

  var controller = app.controllers.api.Project;

  app.get('/api/Project/', controller.get);
  app.post('/api/Project/', controller.post);
  app.get('/api/Project/:id', controller.get);
  app.put('/api/Project/:id', controller.put);

};