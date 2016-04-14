var passport = require('passport'),
    LocalStrategy = require('passport-local').Strategy,
    bcrypt = require('bcrypt'),
    connection = require('./Sequelize.js'),
    User = connection.import('../models/User.js');

module.exports = function(app) {

  app.use(passport.initialize());
  app.use(passport.session());

  passport.use(new LocalStrategy(
    {
      usernameField: 'user',
      passwordField: 'password'
    },
    function(user, password, done) {
      User.findOne({
        where: {
          'user': 'admin'
        }
      }).then(function(userObj) {
        if(userObj == null) {
          return done(null, false, { message: 'Incorrect credentials.' });
        }

        var hashedPassword = bcrypt.hashSync(password, userObj.salt);

        if(userObj.password === hashedPassword) {
          return done(null, userObj);
        }

        return done(null, false, { message: 'Incorrect credentials.' });
      })
    }
  ));

  passport.serializeUser(function(userObj, done) {
    done(null, userObj.id);
  });

  passport.deserializeUser(function(id, done) {
    User.findOne({
      where: {
        'id': id
      }
    }).then(function(user) {
      if (user == null) {
        done(new Error('Wrong user id.'));
      }

      done(null, user);
    });
  });
};
