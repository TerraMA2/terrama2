var passport = require('../../core/utility/Passport');

module.exports = function(app) {
  var controller = app.controllers.configuration.SRIDValidator;

  app.get(app.locals.BASE_URL + "configuration/validate-srid/:srid", passport.isAuthenticated, controller.SRIDValidatorController);
  app.get(app.locals.BASE_URL + "configuration/get-srids", passport.isAuthenticated, controller.getSRIDsController);
};
