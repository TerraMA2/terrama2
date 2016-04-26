var passport = require('../../config/Passport');

module.exports = function (app) {
  var controller = app.controllers.api.Project;

  app.get('/api/Project/', passport.isAuthenticated, controller.get);
  app.post('/api/Project/', passport.isAuthenticated, controller.post);
  app.get('/api/Project/:id', passport.isAuthenticated, controller.get);
  app.put('/api/Project/:id', passport.isAuthenticated, controller.put);
  app.delete('/api/Project/:id/delete', passport.isAuthenticated, controller.delete);
};
