var passport = require('../../core/utility/Passport');

module.exports = function (app) {
  var controller = app.controllers.administration.Service;

  app.get(app.locals.BASE_URL + 'administration/services', passport.isAdministrator, controller.get);
  app.get(app.locals.BASE_URL + 'administration/services/new', passport.isAdministrator, controller.new);
  app.get(app.locals.BASE_URL + 'administration/services/:id', passport.isAdministrator, controller.edit);
};
