var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.MapDisplay;

  app.get('/configuration/mapdisplay', passport.isCommonUser, controller.get);
};
