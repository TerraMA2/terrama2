var passport = require('../../config/Passport');

module.exports = function(app) {
  var controller = app.controllers.configuration.DataSetGrid;

  app.get("/configuration/grid", passport.isAuthenticated, controller);
}
