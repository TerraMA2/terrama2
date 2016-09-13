var passport = require('passport'),
    LocalStrategy = require('passport-local').Strategy,
    bcrypt = require('bcrypt'),
    Database = require('./Database'),
    User = Database.getORM().import('../models/User.js');

var isAuthenticated = function(req, res, next) {
  if(req.isAuthenticated())
    return next();
  req.flash('error', 'You have to be logged in to access the page.');
  res.redirect('/');
};

var _handlePermission = function(condition, request, response, next) {
  if (request.isAuthenticated()) {
    if (condition)
      return next();
    else {
      request.flash('error', 'You don\'t have permission to access this page.');
      response.redirect('/');
    }
  } else {
    request.flash('error', 'You have to be logged in to access the page.');
    response.redirect('/');
  }
};

var isAdministrator = function(req, res, next) {
  _handlePermission(req.user && req.user.administrator, req, res, next);
};

var isCommonUser = function(request, response, next) {
  _handlePermission(request.user && !request.user.administrator, request, response, next);
};

var setupPassport = function(app) {
  app.use(passport.initialize());
  app.use(passport.session());

  passport.use(new LocalStrategy(
    {
      usernameField: 'username',
      passwordField: 'password'
    },
    function(username, password, done) {
      User.findOne({
        where: {
          'username': username
        }
      }).then(function(userObj) {
        if(userObj === null) {
          return done(null, false, { message: 'Incorrect credentials.' });
        }

        var hashedPassword = bcrypt.hashSync(password, userObj.salt);

        if(userObj.password === hashedPassword) {
          return done(null, userObj);
        }

        return done(null, false, { message: 'Incorrect credentials.' });
      });
    }
  ));

  passport.serializeUser(function(userObj, done) {
    return done(null, userObj.id);
  });

  passport.deserializeUser(function(id, done) {
    User.findOne({
      where: {
        'id': id
      }
    }).then(function(user) {
      if (user === null) {
        return done(new Error('Wrong user id.'));
      }

      return done(null, user);
    });
  });
};

module.exports = {
  isAuthenticated: isAuthenticated,
  isAdministrator: isAdministrator,
  isCommonUser: isCommonUser,
  setupPassport: setupPassport
};
