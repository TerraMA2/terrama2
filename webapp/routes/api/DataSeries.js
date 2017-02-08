var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.DataSeries;

  app.get('/api/DataSeries/', passport.isAuthenticated, controller.get);
  app.get('/api/DataSeries/dataProvider/:dataProvider', passport.isAuthenticated, controller.get);
  app.get('/api/DataSeries/project/:project', passport.isAuthenticated, controller.get);
  app.post('/api/DataSeries/', passport.isAuthenticated, controller.post);
  app.get('/api/DataSeries/:id', passport.isAuthenticated, controller.get);
  app.put('/api/DataSeries/:id', passport.isAuthenticated, controller.put);
  app.delete('/api/DataSeries/:id/delete', passport.isAuthenticated, controller.delete);
};
