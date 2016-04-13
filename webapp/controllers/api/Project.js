var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var ProjectError = require("../../core/Exceptions").ProjectError;


module.exports = function(app) {
  return {
    "post": function(request, response) {
      var projectObject = request.body;

      DataManager.addProject(projectObject).then(function(project) {
        response.json(project);
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      });
    },

    "get": function(request, response) {
      var id = request.params.id;

      if (id) {
        DataManager.getProject({id: id}).then(function(project) {
          response.json(project);
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        })
      } else {
        response.json(DataManager.listProjects());
      }
    },

    "put": function(request, response) {
      var id = request.params.id;

      if (id) {
        var projectGiven = request.body;
        projectGiven.id = id;
        DataManager.updateProject(projectGiven).then(function(project) {
          response.json(project);
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        })
      } else {
        Utils.handleRequestError(request, new ProjectError("Project name not identified"), 400);
      }
    },

  };
};