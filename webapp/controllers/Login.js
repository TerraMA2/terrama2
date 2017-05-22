var DataManager = require("./../core/DataManager");

module.exports = function(app) {
  return {
    login: function (request, response) {
      app.locals.collapsed = false;
      response.render('login');
    },
    logout: function(request, response) {
      app.locals.activeProject = {};
      app.locals.collapsed = false;
      request.logout();
      response.redirect(app.locals.BASE_URL);
    }
  };
};
