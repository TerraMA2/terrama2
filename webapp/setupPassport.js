var passport = require('passport'),
    LocalStrategy = require('passport-local').Strategy,
    bcrypt = require('bcrypt'),
    User = require('./models/User.js');

module.exports = function(app) {
  app.use(passport.initialize());
  app.use(passport.session());

  passport.use(new LocalStrategy(
    function(user, password, done) {
      console.log('entrou');
      User.findOne({
        where: {
          'user': 'admin'
        }
      }).then(function(userObj) {
        if(userObj == null) {
          return done(null, false, { message: 'Incorrect credentials.' });
        }

        var hashedPassword = bcrypt.hashSync(password);

        console.log(password);
        console.log(userObj.password);

        if(userObj.password !== '') {//hashedPassword) {
          return done(null, user);
        }

        return done(null, false, { message: 'Incorrect credentials.' });
      })
    }
  ));

  passport.serializeUser(function(user, done) {
    done(null, user.id);
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
