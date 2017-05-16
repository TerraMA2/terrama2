"use strict";

var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var ProjectError = require("../../core/Exceptions").ProjectError;
var TokenCode = require('./../../core/Enums').TokenCode;


module.exports = function(app) {
  return {
    post: function(request, response) {
      var projectObject = request.body;

      DataManager.addProject(projectObject).then(function(project) {
        var token = Utils.generateToken(app, TokenCode.SAVE, project.name);
        response.json({status: 200, result: project, token: token});
      }).catch(function(err) {
        Utils.handleRequestError(response, err, 400);
      });
    },

    get: function(request, response) {
      var id = request.params.id;

      if (id) {
        DataManager.getProject({id: id}).then(function(project) {
          response.json(project);
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        });
      } else {
        response.json(DataManager.listProjects());
      }
    },

    put: function(request, response) {
      var id = request.params.id;

      if (id) {
        var projectGiven = request.body;
        projectGiven.id = id;
        DataManager.updateProject(projectGiven).then(function(project) {
          var token = Utils.generateToken(app, TokenCode.UPDATE, project.name);
          response.json({status: 200, result: project, token: token});
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        });
      } else {
        Utils.handleRequestError(response, new ProjectError("Project name not identified"), 400);
      }
    },

    delete: function(request, response) {
      var id = request.params.id;
      if (id) {
        DataManager.getProject({id: id}).then(function(project) {
          DataManager.removeProject({id: id}).then(function() {
            // un-setting cache project
            app.locals.activeProject = {};
            var token = Utils.generateToken(app, TokenCode.DELETE, project.name);
            response.json({status: 200, name: project.name, token: token});
          }).catch(function(err) {
            Utils.handleRequestError(response, err, 400);
          });
        }).catch(function(err) {
          Utils.handleRequestError(response, new ProjectError("Project not found"), 400);
        });
      } else {
        Utils.handleRequestError(response, new ProjectError("Project id not typed"), 400);
      }
    }
  };
};
