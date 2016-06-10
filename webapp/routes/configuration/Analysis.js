var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Analysis;

  app.get('/configuration/analyses', passport.isCommonUser, controller.index);
  app.get('/configuration/analyses/new', passport.isCommonUser, controller.new);
  app.get("/configuration/analyses/:id/edit", passport.isCommonUser, controller.edit);
}
