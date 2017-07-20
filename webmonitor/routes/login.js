var passport = require('passport');

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

          var io = app.getIo();

          for(var key in io.sockets.sockets) {
            io.sockets.sockets[key].userToken = user.token;
          }

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

    var io = app.getIo();

    for(var key in io.sockets.sockets) {
      delete io.sockets.sockets[key].userToken;
    }

    return response.json({ error: null });
  });
};