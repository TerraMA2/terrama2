var passport = require('passport');

module.exports = function (app) {

  var controller = app.controllers.passportTest;

  var isAuthenticated = function (req, res, next) {
    if(req.isAuthenticated())
      return next();
    req.flash('error', 'You have to be logged in to access the page.');
    res.redirect('/');
  }

  app.post('/passportTest', passport.authenticate('local', {
    successRedirect: '/dashboard',
    failureRedirect: '/passportTest',
    failureFlash: true
  }));

  app.get('/passportTest', controller);
};
