var passport = require('passport');
var passportConfig = require('../config/Passport');

module.exports = function (app) {

  var controller = app.controllers.passportTest;

  app.post('/passportTest', passport.authenticate('local', {
    successRedirect: '/dashboard',
    failureRedirect: '/passportTest',
    failureFlash: true
  }));

  app.get('/dashboard', passportConfig.isAuthenticated, function(req, res) {
    if(req.user.dataValues.administrator) {
      console.log("adm");
    } else {
      console.log("normal");
    }
  });

  app.get('/passportTest', controller);
};
