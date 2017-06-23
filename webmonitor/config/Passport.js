var passport = require('passport');
var LocalStrategy = require('passport-local').Strategy;
var request = require('request');

var setupPassport = function(app) {
  app.use(passport.initialize());
  app.use(passport.session());

  app.use(function(req, res, next) {
    if(req.session.passport !== undefined && req.session.passport.user !== undefined) {
      res.locals.currentUser = req.session.passport.user;
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
        url: app.locals.ADMIN_URL + 'login/remote',
        form: {
          username: username,
          password: password
        },
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

        return done(null, response.user);
      });
    }
  ));

  passport.serializeUser(function(userObj, done) {
    return done(null, userObj);
  });

  passport.deserializeUser(function(userObj, done) {
    return done(null, userObj);
  });
};

module.exports = {
  setupPassport: setupPassport
};
