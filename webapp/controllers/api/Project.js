"use strict";

var DataManager = require("../../core/DataManager.js");
var Utils = require("../../core/Utils");
var TokenCode = require("./../../core/Enums").TokenCode;
var ProjectFacade = require("./../../core/facade/Project");

module.exports = function(app) {
  return {
    post: function(request, response) {
      ProjectFacade.save(request.body, request.user.id).then(function(projectResult) {
        request.session.cachedProjects = DataManager.listProjects();

        var token = Utils.generateToken(app, TokenCode.SAVE, projectResult.name);
        return response.json({status: 200, result: projectResult, token: token});
      }).catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });
    },

    get: function(request, response) {
      ProjectFacade.retrieve(request.params.id, request.user).then(function(projects) {
        return response.json(projects)
      }).catch(function(err) {
        return Utils.handleRequestError(response, err, 400);
      });
    },

    put: function(request, response) {
      ProjectFacade.update(request.params.id, request.body, request.user.id).then(function(projectResult) {
        request.session.cachedProjects = DataManager.listProjects();

        var token = Utils.generateToken(app, TokenCode.UPDATE, projectResult.name);
        return response.json({status: 200, result: projectResult, token: token});
      }).catch(function(err){
        return Utils.handleRequestError(response, err, 400);
      });
    },

    delete: function(request, response) {
      ProjectFacade.remove(request.params.id).then(function(project) {
        // un-setting cache project
        request.session.activeProject = {};
        request.session.cachedProjects = DataManager.listProjects();

        var token = Utils.generateToken(app, TokenCode.DELETE, project.name);
        return response.json({status: 200, name: project.name, token: token});
      }).catch(function(err){
        return Utils.handleRequestError(response, err, 400);
      });
    }
  };
};
