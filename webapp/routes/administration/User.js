var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.administration.User;

  app.get('/administration/users', passport.isAdministrator, controller.get);
  app.get('/administration/users/new', passport.isAdministrator, controller.new);
  app.get('/administration/users/edit/:id', passport.isAdministrator, controller.edit);
  app.post('/administration/users/new', passport.isAdministrator, controller.post);
  app.put('/administration/users/edit/:id', passport.isAdministrator, controller.put);
  app.delete('/administration/users/:id/delete', passport.isAdministrator, controller.delete);
};
