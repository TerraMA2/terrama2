var passport = require('../../core/utility/Passport');

module.exports = function (app) {
  var controller = app.controllers.administration.User;

  app.get(app.locals.BASE_URL + 'administration/users', passport.isAdministrator, controller.get);
  app.get(app.locals.BASE_URL + 'administration/users/new', passport.isAdministrator, controller.new);
  app.get(app.locals.BASE_URL + 'administration/users/edit/:id', passport.isAdministrator, controller.edit);
  app.post(app.locals.BASE_URL + 'administration/users/new', passport.isAdministrator, controller.post);
  app.put(app.locals.BASE_URL + 'administration/users/edit/:id', passport.isAdministrator, controller.put);
  app.delete(app.locals.BASE_URL + 'administration/users/:id/delete', passport.isAdministrator, controller.delete);
};
