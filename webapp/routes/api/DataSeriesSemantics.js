var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.DataSeriesSemantics;

  app.get('/api/DataSeriesSemantics/', passport.isAuthenticated, controller.get);
  app.get('/api/DataSeriesSemantics/:name', passport.isAuthenticated, controller.get);
};
