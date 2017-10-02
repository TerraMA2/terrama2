module.exports = function(app) {
  "use strict";

  var DataManager = require('../core/DataManager');

  function indexController(request, response) {
    if(request.isAuthenticated()) {
      var projects = DataManager.listProjects();

      if(request.originalUrl == app.locals.BASE_URL + "firstAccess" && projects.length == 1)
        return response.redirect(app.locals.BASE_URL + "configuration/projects/" + projects[0].name + "/activate");
      else
        return response.redirect(app.locals.BASE_URL + "configuration/projects");
    } else {
      return response.redirect(app.locals.BASE_URL + "login");
    }
  }

  return indexController;
};
