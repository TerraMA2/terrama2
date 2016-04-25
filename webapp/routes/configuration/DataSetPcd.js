var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration;

  app.get('/configuration/pcd', passport.isAuthenticated, controller.DataSetPcd);
}
