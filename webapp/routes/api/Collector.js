var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Collector;

  app.get(app.locals.BASE_URL + 'api/Collector/', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/Collector/project/:project', passport.isAuthenticated, controller.get);
};
