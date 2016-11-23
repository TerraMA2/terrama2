var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.DataProvider;

  app.get('/api/DataProvider/', passport.isAuthenticated, controller.get);
  app.get('/api/DataProvider/project/:project', passport.isAuthenticated, controller.get);
  app.post('/api/DataProvider/', passport.isAuthenticated, controller.post);
  app.put('/api/DataProvider/:id', passport.isAuthenticated, controller.put);
  app.delete('/api/DataProvider/:id/delete', passport.isAuthenticated, controller.delete);
};
