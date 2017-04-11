(function() {
  'use strict';

  // Dependencies
  var DataManager = require("./../../core/DataManager");
  var handleRequestError = require("./../../core/Utils").handleRequestError;
  var TokenCode = require('./../../core/Enums').TokenCode;
  var Utils = require('./../../core/Utils');

  // Facade
  var AlertFacade = require("./../../core/facade/Alert");

  module.exports = function(app){
    return {
      get: function(request, response){
        return 'getFunction';
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
        return 'putFunction';
      },
      delete: function(request, response){
        return 'deleteFunction';
      }
    }
  }
} ());