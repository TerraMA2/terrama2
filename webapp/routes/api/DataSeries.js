var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.DataSeries;

  app.get(app.locals.BASE_URL + 'api/DataSeries/', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/DataSeries/dataProvider/:dataProvider', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/DataSeries/project/:project', passport.isAuthenticated, controller.get);
  app.post(app.locals.BASE_URL + 'api/DataSeries/', passport.isAuthenticated, controller.post);
  app.get(app.locals.BASE_URL + 'api/DataSeries/:id', passport.isAuthenticated, controller.get);
  app.put(app.locals.BASE_URL + 'api/DataSeries/:id', passport.isAuthenticated, controller.put);
  app.delete(app.locals.BASE_URL + 'api/DataSeries/:id/delete', passport.isAuthenticated, controller.delete);
  app.post(app.locals.BASE_URL + 'api/DataSeries/duplicate', passport.isAuthenticated, controller.duplicate);
  app.get(app.locals.BASE_URL + "api/DataSeries/static/changeStatus/:id", passport.isAuthenticated, controller.changeStatusStatic);
  app.get(app.locals.BASE_URL + "api/DataSeries/dynamic/changeStatus/:id", passport.isAuthenticated, controller.changeStatusDynamic);
  app.post(app.locals.BASE_URL + 'api/DataSeries/validateView', passport.isAuthenticated, controller.validateViewCreation);
};
