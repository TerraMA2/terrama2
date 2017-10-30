var DataManager = require("./../core/DataManager");

module.exports = function(app) {
  return {
    login: function (request, response) {
      response.render('login');
    },
    logout: function(request, response) {
      if(request.session && request.session.collapsed)
        delete request.session.collapsed;

      if(request.session && request.session.activeProject)
        delete request.session.activeProject;

      if(request.session && request.session.cachedProjects)
        delete request.session.cachedProjects;

      request.logout();
      response.redirect(app.locals.BASE_URL);
    }
  };
};
