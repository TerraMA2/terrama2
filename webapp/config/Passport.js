var passport = require('passport'),
    LocalStrategy = require('passport-local').Strategy,
    bcrypt = require('bcrypt'),
    DataManager = require("./../core/DataManager"),
    BASE_URL = null;

/**
 * Middleware to check if current user is authenticated and redirect him to correct path
 * 
 * @param {Request} req - Express request instance
 * @param {Response} res - Express response instance
 */
var isAuthenticated = function(req, res, next) {
  if(req.isAuthenticated()) {
    return next();
  }
  req.flash('error', 'You have to be logged in to access the page.');
  res.redirect(BASE_URL);
};

var _handlePermission = function(condition, request, response, next) {
  if (request.isAuthenticated()) {
    if (condition) {
      return next();
    } else {
      request.flash('error', 'You don\'t have permission to access this page.');
      response.redirect(BASE_URL);
    }
  } else {
    request.flash('error', 'You have to be logged in to access the page.');
    response.redirect(BASE_URL);
  }
};

var isAdministrator = function(req, res, next) {
  _handlePermission(req.user && req.user.administrator, req, res, next);
};

var isCommonUser = function(request, response, next) {
  _handlePermission(request.user && !request.user.administrator, request, response, next);
};

var setupPassport = function(app) {
  BASE_URL = app.locals.BASE_URL;

  app.use(passport.initialize());
  app.use(passport.session());

  app.use(function(req, res, next) {
    res.locals.userLocale = req.session.userLocale;

    if(req.session.passport !== undefined && req.session.passport.user !== undefined) {
      res.locals.activeProject = req.session.activeProject;
      res.locals.collapsed = req.session.collapsed;

      return DataManager.getUser({ 'id': req.session.passport.user })
        .then(function(userObj) {
          res.locals.currentUser = {
            id: userObj.id,
            name: userObj.name,
            email: userObj.email,
            cellphone: userObj.cellphone,
            username: userObj.username,
            administrator: userObj.administrator
          };
          return;
        })
        .catch(function(err) {
          res.locals.currentUser = null;
          return;
        })
        .finally(function() {
          return next();
        });
    } else {
      res.locals.currentUser = null;
      res.locals.activeProject = {};
      res.locals.collapsed = false;
      next();
    }
  });

  passport.use(new LocalStrategy(
    {
      usernameField: 'username',
      passwordField: 'password'
    },
    function(username, password, done) {
      return DataManager.getUser({'username': username}).then(function(userObj) {
        if(userObj === null)
          return done(null, false, { message: 'Incorrect user.' });

        var hashedPassword = bcrypt.hashSync(password, userObj.salt);

        if(userObj.password === hashedPassword)
          return done(null, userObj);

        return done(null, false, { message: 'Incorrect password.' });
      }).catch(function(err) {
        return done(null, false, { message: 'Incorrect user.' });
      });
    }
  ));

  passport.serializeUser(function(userObj, done) {
    return done(null, userObj.id);
  });

  passport.deserializeUser(function(id, done) {
    return DataManager.getUser({'id': id})
      .then(function(user) {
        if (!user) {
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
