var passport = require('passport');
var DataManager = require('./../core/DataManager');
var bcrypt = require('bcrypt');

module.exports = function(app) {

  var controller = app.controllers.Login;

  app.get(app.locals.BASE_URL + 'login', controller.login);

  app.post(app.locals.BASE_URL + 'login/process', function(request, response, next) {
    passport.authenticate('local', function(err, user, info) {
      if(err)
        next(err);

      if(!user)
        return response.render('login', { message: info.message });

      request.logIn(user, function(e) {
        if(e)
          return next(e);

        request.session.collapsed = false;
        request.session.activeProject = {};
        request.session.cachedProjects = DataManager.listProjects();

        return response.redirect(app.locals.BASE_URL + 'firstAccess')
      })
    })(request, response, next);
  });

  app.post(app.locals.BASE_URL + 'login/remote', function(request, response) {
    DataManager.getUser({'username': request.body.username}).then(function(userObj) {
      if(userObj === null)
        return response.json({
          error: true,
          message: 'INCORRECT-USER',
          user: null
        });

      var hashedPassword = bcrypt.hashSync(request.body.password, userObj.salt);

      if(userObj.password === hashedPassword) {
        var user = {
          id: userObj.id,
          name: userObj.name,
          email: userObj.email,
          cellphone: userObj.cellphone,
          username: userObj.username,
          administrator: userObj.administrator,
          token: userObj.token
        };

        return response.json({
          error: false,
          message: null,
          user: user
        });
      }

      return response.json({
        error: true,
        message: 'INCORRECT-PASSWORD',
        user: null
      });
    }).catch(function(err) {
      return response.json({
        error: true,
        message: 'INCORRECT-USER',
        user: null
      });
    });
  });

  app.get(app.locals.BASE_URL + 'logout', controller.logout);
};
