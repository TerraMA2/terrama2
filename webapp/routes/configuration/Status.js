var passport = require('../../config/Passport');

module.exports = function(app) {
  var controller = app.controllers.configuration.Status;

  app.get(app.locals.BASE_URL + "configuration/status", passport.isAuthenticated, controller);
};
