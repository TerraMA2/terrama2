var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.administration.Service;

  app.get('/administration/services', passport.isAdministrator, controller.get);
  app.get('/administration/services/new', passport.isAdministrator, controller.new);
  app.get('/administration/services/:name', passport.isAdministrator, controller.edit);
};
