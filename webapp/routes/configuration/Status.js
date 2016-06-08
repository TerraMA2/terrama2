var passport = require('../../config/Passport');

module.exports = function(app) {
  var controller = app.controllers.configuration.Status;

  app.get("/configuration/status", passport.isCommonUser, controller);
};
