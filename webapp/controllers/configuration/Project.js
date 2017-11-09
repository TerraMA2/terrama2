'use strict';

var DataManager = require('../../core/DataManager');
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;

module.exports = function(app) {
  return {
    get: function (request, response) {
      DataManager.load().then(function() {
        var parameters = makeTokenParameters(request.query.token, app);
        var hasProjectPermission = request.session.activeProject.hasProjectPermission || true;
        parameters.hasProjectPermission = hasProjectPermission;

        response.render("configuration/projects", Object.assign({context: request.query.context, user_id: request.user.id}, parameters));
      });
    },

    new: function (request, response) {
      response.render("configuration/project", {method: "POST", url: app.locals.BASE_URL + "api/Project"});
    },

    edit: function(request, response) {
      var projectName = request.params.name;

      DataManager.getProject({name: projectName}).then(function(project) {
        project.canProtect = request.user.id == project.user_id || project.user_id == null;
        response.render("configuration/project", {project: project, method: "PUT", url: app.locals.BASE_URL + "api/Project/"+project.id});
      }).catch(function(err) {
        response.render("base/404");
      });
    },

    activate: function(request, response) {
      var projectName = request.params.name;

      DataManager.getProject({name: projectName}).then(function(project) {
        var hasProjectPermission = false;
        if (project.user_id == null || request.user.id == project.user_id || !project.protected){
          hasProjectPermission = true;
        } 

        request.session.activeProject = {id: project.id, name: project.name, protected: project.protected, userId: project.user_id, hasProjectPermission: hasProjectPermission};

        // Redirect for start application
        if(request.params.token !== undefined)
          response.redirect(app.locals.BASE_URL + "configuration/status?token=" + request.params.token);
        else
          response.redirect(app.locals.BASE_URL + "configuration/status");
      }).catch(function(err) {
        response.render('base/404');
      });
    },

    updateCache: function(request, response){

      request.session.cachedProjects = DataManager.listProjects();
      response.redirect(app.locals.BASE_URL + "configuration/projects");
    }
  };
};
