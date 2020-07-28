var passport = require('../../core/utility/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.DataFormat;

  app.get(app.locals.BASE_URL + 'api/DataFormat/', passport.isAuthenticated, controller.get);
};
