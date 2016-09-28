var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.MapDisplay;

  app.get('/configuration/views', passport.isAuthenticated, controller.get);
};
