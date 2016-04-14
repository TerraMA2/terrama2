var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.administration.User;

  app.get('/administration/users', passport.isAdministrator, controller.get);
  app.get('/administration/users/new', passport.isAdministrator, controller.new);
  app.get('/administration/users/:name', passport.isAdministrator, controller.edit);
};
