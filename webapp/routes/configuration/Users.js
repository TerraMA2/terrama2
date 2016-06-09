var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Users;

  app.get('/configuration/users', passport.isCommonUser, controller.index);
  app.get('/configuration/user', passport.isCommonUser, controller["new"]);
}
