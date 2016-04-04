module.exports = function (app) {

  var controller = app.controllers.administration.User;

  app.get('/administration/users', controller.get);
  app.get('/administration/users/new', controller.new);
  app.get('/administration/users/:name', controller.edit);

};