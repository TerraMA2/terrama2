var express = require('express');
var router = express.Router();
var passport = require('passport');

router.post('/', function (request, response, next) {
  passport.authenticate('local', function(err, user, info) {
    if(err)
      console.log(err);

    if(!user)
      return response.redirect('/' + info.message);
    else {
      request.logIn(user, function(e) {
        if(e)
          return next(e);

        return response.redirect('/');
      });
    }
  })(request, response, next);
});

router.get('/logout', function(request, response) {
  request.logout();
  return response.redirect('/');
});

module.exports = router;
