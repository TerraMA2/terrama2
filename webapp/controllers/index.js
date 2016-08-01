"use strict";

var User = require('../config/Database').getORM().import('../models/User.js');

module.exports = function(app) {
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
