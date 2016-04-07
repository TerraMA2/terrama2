module.exports = function (app) {

  var controller = app.controllers.administration.Service;

  app.get('/administration/services', controller.get);
  app.get('/administration/services/new', controller.new);
  app.get('/administration/services/:name', controller.edit);

};