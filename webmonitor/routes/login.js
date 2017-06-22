var passport = require('passport');

module.exports = function(app) {
  app.post(app.locals.BASE_URL + 'login', function (request, response, next) {
    passport.authenticate('local', function(err, user, info) {
      if(err)
        console.log(err);

      if(!user)
        return response.redirect(app.locals.BASE_URL + "?message=" + info.message);
      else {
        request.logIn(user, function(e) {
          if(e)
            return next(e);

          return response.redirect(app.locals.BASE_URL);
        });
      }
    })(request, response, next);
  });

  app.get(app.locals.BASE_URL + 'logout', function(request, response) {
    request.logout();
    return response.redirect(app.locals.BASE_URL);
  });
};