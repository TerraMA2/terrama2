var passport = require('passport');
var LocalStrategy = require('passport-local').Strategy;
var request = require('request');

var setupPassport = function(app) {
  app.use(passport.initialize());
  app.use(passport.session());

  app.use(function(req, res, next) {
    if(req.session.passport !== undefined && req.session.passport.user !== undefined) {
      res.locals.currentUser = app.locals.user;
      next();
    } else {
      res.locals.currentUser = null;
      next();
    }
  });

  passport.use(new LocalStrategy(
    {
      usernameField: 'username',
      passwordField: 'password'
    },
    function(username, password, done) {
      var options = {
        url: 'http://localhost:36000/login/remote',
        form: {username:username,password:password},
        method: 'POST',
        headers: {
          'accept': 'application/json',
          'content-Type': 'application/json'
        }
      };

      request(options, function(err, res, html) {
        if(err)
          return done(null, false, { message: err.message });

        var response = JSON.parse(res.body);

        if(response.error)
          return done(null, false, { message: response.message });

        app.locals.user = response.user;
        return done(null, response.user);
      });
    }
  ));

  passport.serializeUser(function(userObj, done) {
    return done(null, userObj.id);
  });

  passport.deserializeUser(function(id, done) {
    return done(null, app.locals.user);
  });
};

module.exports = {
  setupPassport: setupPassport
};
