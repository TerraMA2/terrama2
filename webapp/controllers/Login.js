module.exports = function(app) {
  return {
    login: function (request, response) {
      response.render('login');
    },

    logout: function(request, response) {
      app.locals.activeProject = {};
      app.locals.user = {};
      response.redirect("/login");
    },

    process: function(request, response) {
      var username = request.body.username;
      var password = request.body.password;
      var user = {
        name: username || "user"
      };

      if (username === 'admin' && password === 'admin') {
        user.role = 'admin';
        app.locals.user = user;
        return response.redirect('/administration/services');
      }

      app.locals.user = user;
      
      return response.redirect('/configuration/projects');
    }
  };
};