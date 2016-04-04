module.exports = function (app) {

  var controller = app.controllers.Login;

  app.get('/login', controller.login);
  app.post('/login/process', controller.process);
  app.get('/logout', controller.logout);
};