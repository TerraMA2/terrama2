var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Uri;

  app.post(app.locals.BASE_URL + 'uri/', passport.isAuthenticated, controller.post);
  app.get(app.locals.BASE_URL + 'uri/', passport.isAuthenticated, controller.get);
};
