var passport = require('../../config/Passport');

module.exports = function (app) {

  var controller = app.controllers.api.Service;

  app.get('/api/Service/', passport.isAuthenticated, controller.get);
  app.post('/api/Service/', passport.isAuthenticated, controller.post);
  app.delete('/api/Service/:id/delete', passport.isAuthenticated, controller.delete);

};