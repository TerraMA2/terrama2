(function() {
  'use strict';

  // Dependencies
  var handleRequestError = require("./../../core/Utils").handleRequestError;
  var TokenCode = require('./../../core/Enums').TokenCode;
  var Utils = require('./../../core/Utils');

  // Facade
  var LegendFacade = require("./../../core/facade/Legend");

  module.exports = function(app) {
    return {
      get: function(request, response) {
        var legendId = (request.params.id ? request.params.id : request.query.id);
        var projectId = (request.params.project_id ? request.params.project_id : request.query.project_id);

        LegendFacade.retrieve(legendId, (projectId ? projectId : app.locals.activeProject.id)).then(function(legends) {
          return response.json(legends);
        }).catch(function(err) {
          return handleRequestError(response, err, 400);
        });
      },
      post: function(request, response) {
        var legendObject = request.body;

        LegendFacade.save(legendObject, app.locals.activeProject.id).then(function(legend) {
          // generating token
          var token = Utils.generateToken(app, TokenCode.SAVE, legend.name);
          return response.json({status: 200, result: legend.toObject(), token: token});
        }).catch(function(err){
          return handleRequestError(response, err, 400);
        });
      },
      put: function(request, response) {
        var legendId = parseInt(request.params.id);

        LegendFacade.update(legendId, request.body, app.locals.activeProject.id).then(function(legend) {
          var token = Utils.generateToken(app, TokenCode.UPDATE, legend.name);
          return response.json({status: 200, result: legend.toObject(), token: token});
        }).catch(function(err) {
          return handleRequestError(response, err, 400);
        });
      },
      delete: function(request, response) {
        var legendId = parseInt(request.params.id);

        LegendFacade.remove(legendId).then(function(legend) {
          var token = Utils.generateToken(app, TokenCode.DELETE, legend.name);
          return response.json({status: 200, result: legend.toObject(), token: token});
        }).catch(function(err) {
          return handleRequestError(response, err, 400);
        });
      }
    }
  }
} ());