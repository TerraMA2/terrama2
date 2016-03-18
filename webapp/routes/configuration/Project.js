module.exports = function (app) {

  var controller = app.controllers.configuration.Project;

  app.get('/configuration/projects', controller.get);
  app.get('/configuration/projects/new', controller.new);
  app.get('/configuration/projects/edit/:name', controller.edit);

};