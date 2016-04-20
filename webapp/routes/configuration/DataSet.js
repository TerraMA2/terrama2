var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.DataSet;

  app.get("/configuration/dataset", passport.isAuthenticated, controller);
}
