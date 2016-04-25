var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Uri;

  app.post('/uri/', passport.isAuthenticated, controller.post);
};
