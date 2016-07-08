var passport = require('passport');
var DataManager = require('./../core/DataManager');

module.exports = function (app) {

  var controller = app.controllers.Login;

  app.get('/login', controller.login);

  app.post('/login/process', function(request, response, next){
    passport.authenticate('local', function(err, user, info) {
      if (err)
        next(err);

      if (!user) {
        // response.status(400);
        // return response.json({status: 400, message: info.message});
        return response.render('login', {message: info.message});
      }

      request.logIn(user, function(e) {
        if (e) {
          return next(e);
        }

        // response.status(200);
        return response.redirect('/')
      })
    })(request, response, next)
  },
    function(request, response, next) {
      DataManager.load().then(function() {
        next()
      }).catch(function(err) {
        console.log("Could not load datamanager: " + err.message);
      })
    }
  );

  app.get('/logout', controller.logout);
};
