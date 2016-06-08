var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.configuration.Project;

  app.get('/configuration/projects', passport.isCommonUser, controller.get);
  app.get('/configuration/projects/new', passport.isCommonUser, controller.new);
  app.get('/configuration/projects/:name/activate', passport.isCommonUser, controller.activate);
  app.get('/configuration/projects/edit/:name', passport.isCommonUser, controller.edit);
};
