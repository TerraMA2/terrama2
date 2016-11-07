module.exports = function(app) {
  "use strict";

  var User = require('../config/Database').getORM().import('../models/User.js');
  
  function indexController(request, response) {
    if(request.isAuthenticated()) {
      return response.redirect("/configuration/projects");
    } else {
      app.locals.activeProject = {};
      return response.redirect("/login");
    }
  }

  return indexController;
};
