module.exports = function (app) {

  var controller = app.controllers.Login;

  app.get('/login', controller.login);

  app.post('/login/process', passport.authenticate('local', {
    successRedirect: '/',
    failureRedirect: '/login',
    failureFlash: true
  }));

  app.get('/logout', controller.logout);
};
