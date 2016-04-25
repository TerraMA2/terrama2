var passport = require('passport');
var DataManager = require('./../core/DataManager');

module.exports = function (app) {

  var controller = app.controllers.Login;

  app.get('/login', controller.login);

  app.post('/login/process', function(request, response, next){
    DataManager.load().then(function() {
      next()
    }).catch(function(err) {
      console.log("Could not load datamanager: " + err.message);
    })
  }, passport.authenticate('local', {
    successRedirect: '/',
    failureRedirect: '/login',
    failureFlash: true
  }));

  app.get('/logout', controller.logout);
};
