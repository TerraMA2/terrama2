module.exports = function(app) {
  return {
    login: function (request, response) {
      response.render('login');
    },

    logout: function(request, response) {
      app.locals.activeProject = {};
      response.redirect("/login");
    }
  };
};