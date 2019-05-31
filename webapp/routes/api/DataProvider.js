var passport = require('../../core/utility/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.DataProvider;

  app.get(app.locals.BASE_URL + 'api/DataProvider/', passport.isAuthenticated, controller.get);
  app.get(app.locals.BASE_URL + 'api/DataProvider/project/:project', passport.isAuthenticated, controller.get);
  app.post(app.locals.BASE_URL + 'api/DataProvider/', passport.isAuthenticated, controller.post);
  app.post(app.locals.BASE_URL + 'api/DataProvider/listobjects', passport.isAuthenticated, controller.listObjects);
  app.put(app.locals.BASE_URL + 'api/DataProvider/:id', passport.isAuthenticated, controller.put);
  app.delete(app.locals.BASE_URL + 'api/DataProvider/:id/delete', passport.isAuthenticated, controller.delete);
  app.get(app.locals.BASE_URL + 'api/DataProvider/changeStatus/:id', passport.isAuthenticated, controller.changeStatus);
};
