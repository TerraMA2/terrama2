'use strict';

var DataManager = require('../../core/DataManager');
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;

module.exports = function(app) {
  return {
    get: function (request, response) {
      DataManager.load().then(function() {
        var parameters = makeTokenParameters(request.query.token, app);

        response.render("configuration/projects", Object.assign({context: request.query.context}, parameters));
      });
    },

    new: function (request, response) {
      response.render("configuration/project", {method: "POST", url: "/api/Project"});
    },

    edit: function(request, response) {
      var projectName = request.params.name;

      DataManager.getProject({name: projectName}).then(function(project) {
        response.render("configuration/project", {project: project, method: "PUT", url: "/api/Project/"+project.id});
      }).catch(function(err) {
        response.render("base/404");
      });
    },

    activate: function(request, response) {
      var projectName = request.params.name;

      DataManager.getProject({name: projectName}).then(function(project) {
        app.locals.activeProject = {id: project.id, name: project.name};

        // Redirect for start application
        if(request.params.token !== undefined)
          response.redirect("/configuration/status?token=" + request.params.token);
        else
          response.redirect("/configuration/status");
      }).catch(function(err) {
        response.render('base/404');
      });
    }
  };
};
