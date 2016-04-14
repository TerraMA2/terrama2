var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration;

  // Static DataProviders And Static DataSets (GET)
  app.get('/configuration/static-data', passport.isAuthenticated, controller.StaticData);
}
