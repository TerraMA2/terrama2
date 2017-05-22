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
        
        AlertFacade.retrieve(alertId, (projectId ? projectId : app.locals.activeProject.id))
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

        AlertFacade.save(alertObject, app.locals.activeProject.id, shouldRun)
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

        AlertFacade.update(alertId, request.body, app.locals.activeProject.id, shouldRun)
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
      listRisks: function(request, response){
        var projectId = request.params.project_id;

        AlertFacade.listRisks((projectId ? projectId : app.locals.activeProject.id))
          .then(function(risks) {
            return response.json(risks);
          })
          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      }
    }
  }
} ());