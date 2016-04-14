var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration;

  // Dynamic DataProviders And Dynamic DataSets (GET)
  app.get('/configuration/dynamic-data', passport.isAuthenticated, controller.DynamicData);
}
