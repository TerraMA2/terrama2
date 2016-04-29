var DataManager = require("./../core/DataManager");

module.exports = function(app) {
  return {
    login: function (request, response) {
      response.render('login');
    },
    logout: function(request, response) {
      DataManager.unload().then(function() {
        console.log("Unloaded");
        app.locals.activeProject = {};
        app.locals.collapsed = false;
        request.logout();
        response.redirect('/');
      });
    }
  };
};
