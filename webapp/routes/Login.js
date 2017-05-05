var passport = require('passport');
var DataManager = require('./../core/DataManager');

module.exports = function(app) {

  var controller = app.controllers.Login;

  app.get('/login', controller.login);

  app.post('/login/process', function(request, response, next) {
    passport.authenticate('local', function(err, user, info) {
      if(err)
        next(err);

      if(!user)
        return response.render('login', { message: info.message });

      request.logIn(user, function(e) {
        if(e)
          return next(e);

        app.locals.collapsed = false;
        return response.redirect('/firstAccess')
      })
    })(request, response, next);
  });

  app.get('/logout', controller.logout);
};
