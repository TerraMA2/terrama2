var DataManager = require('../../core/DataManager');

module.exports = function(app) {
  return {
    get: function (request, response) {
      response.render("configuration/projects");
    },

    new: function (request, response) {
      response.render("configuration/project", {method: "POST", url: "/api/Project"});
    },

    edit: function(request, response) {
      var projectName = request.params.name;

      DataManager.getProject({name: projectName}).then(function(project) {
        response.render("configuration/project", {project: project, method: "PUT", url: "/api/Project/"+project.id});
      }).catch(function(err) {
        response.send("Error 404 not found");
      });
    }
  };
};