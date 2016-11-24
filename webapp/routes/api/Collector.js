var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Collector;

  app.get('/api/Collector/', passport.isAuthenticated, controller.get);
  app.get('/api/Collector/project/:project', passport.isAuthenticated, controller.get);
};
