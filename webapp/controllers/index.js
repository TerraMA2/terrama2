module.exports = function(app) {
  "use strict";

  var User = require('../config/Database').getORM().import('../models/User.js');
  var DataManager = require('../core/DataManager');

  function indexController(request, response) {
    if(request.isAuthenticated()) {
      var projects = DataManager.listProjects();

      if(request.originalUrl == "/firstAccess" && projects.length == 1)
        return response.redirect("/configuration/projects/" + projects[0].name + "/activate");
      else
        return response.redirect("/configuration/projects");
    } else {
      app.locals.activeProject = {};
      return response.redirect("/login");
    }
  }

  return indexController;
};
