(function() {
  'use strict';

  // Dependencies
  var handleRequestError = require("./../../core/Utils").handleRequestError;
  var TokenCode = require('./../../core/Enums').TokenCode;
  var Utils = require('./../../core/Utils');

  // Facade
  var AlertFacade = require("./../../core/facade/Alert");

  module.exports = function(app){
    return {
      get: function(request, response){
        var alertId = request.params.id;
        var projectId = request.params.project_id;
        
        AlertFacade.retrieve(alertId, (projectId ? projectId : request.session.activeProject.id))
          .then(function(alerts) {
            return response.json(alerts);
          })

          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      },
      post: function(request, response){
        var alertObject = request.body;
        var shouldRun = request.body.run;

        AlertFacade.save(alertObject, request.session.activeProject.id, shouldRun)
          .then(function(alert) {
            // generating token
            var token = Utils.generateToken(app, TokenCode.SAVE, alert.name);
            return response.json({status: 200, result: alert.toObject(), token: token});
          })
          
          .catch(function(err){
            return handleRequestError(response, err, 400);
          });
      },
      put: function(request, response){
        var alertId = parseInt(request.params.id);
        var shouldRun = request.body.run;

        AlertFacade.update(alertId, request.body, request.session.activeProject.id, shouldRun)
          .then(function(alert) {
            var token = Utils.generateToken(app, TokenCode.UPDATE, alert.name);
            return response.json({status: 200, result: alert.toObject(), token: token});
          })

          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      },
      delete: function(request, response){
        var alertId = parseInt(request.params.id);

        AlertFacade.remove(alertId)
          .then(function(alert) {
            var token = Utils.generateToken(app, TokenCode.DELETE, alert.name);
            return response.json({status: 200, result: alert.toObject(), token: token});
          })

          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      },
      changeStatus: function(request, response) {
        AlertFacade.changeStatus(parseInt(request.params.id))
          .then(function() {
            return response.json({});
          })
          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      }
    }
  }
} ());