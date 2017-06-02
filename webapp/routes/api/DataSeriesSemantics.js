var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.DataSeriesSemantics;

  app.get(app.locals.BASE_URL + 'api/DataSeriesSemantics/', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/DataSeriesSemantics/:name', passport.isAuthenticated, controller.get);
};
