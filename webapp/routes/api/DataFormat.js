var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.DataFormat;

  app.get('/api/DataFormat/', passport.isAuthenticated, controller.get);
};
