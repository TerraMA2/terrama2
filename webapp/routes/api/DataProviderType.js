var passport = require('../../core/utility/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.DataProviderType;

  app.get(app.locals.BASE_URL + 'api/DataProviderType/', passport.isAuthenticated, controller.get);
};
