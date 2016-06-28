var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.DataProvider;

  app.get('/configuration/providers', passport.isCommonUser, controller.get);
  app.get('/configuration/providers/new', passport.isCommonUser, controller.new);
  app.get('/configuration/providers/edit/:id', passport.isCommonUser, controller.edit);
};
