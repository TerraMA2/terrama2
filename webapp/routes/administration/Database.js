var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.administration.Database;

  app.get('/administration/databases/new', passport.isAdministrator, controller.new);
};
