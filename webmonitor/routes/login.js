var passport = require('passport');
var userToken = require('../config/UserToken');

module.exports = function(app) {
  app.post(app.locals.BASE_URL + 'login', function (request, response, next) {
    passport.authenticate('local', function(err, user, info) {
      if(err)
        console.log(err);

      if(!user)
        return response.json({
          error: info.message
        });
      else {
        request.logIn(user, function(e) {
          if(e)
            return next(e);

          userToken.setToken(user.token);

          return response.json({
            error: null,
            username: user.name
          });
        });
      }
    })(request, response, next);
  });

  app.get(app.locals.BASE_URL + 'logout', function(request, response) {
    request.logout();

    userToken.setToken(null);

    return response.json({ error: null });
  });
};