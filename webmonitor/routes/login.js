module.exports = function(app) {
  var controller = app.controllers.LoginController;

  app.post(app.locals.BASE_URL + 'login', controller.login);
  app.get(app.locals.BASE_URL + 'logout', controller.logout);
  app.post(app.locals.BASE_URL + 'check-authentication', controller.checkAuthentication);
};