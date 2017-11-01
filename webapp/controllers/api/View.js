(function() {
  'use strict';

  // Dependencies
  var handleRequestError = require("./../../core/Utils").handleRequestError;
  var TokenCode = require('./../../core/Enums').TokenCode;
  var Utils = require('./../../core/Utils');

  // Facade
  var ViewFacade = require("./../../core/facade/View");

  /**
   * Injecting NodeJS App configuration as dependency. It retrieves a Views controllers API
   * 
   * @param {Object}
   * @returns {Object}
   */
  module.exports = function(app) {
    return {
      get: function(request, response) {
        var viewId = request.params.id;
        var projectId = request.params.project_id;
        
        ViewFacade.retrieve(viewId, (projectId ? projectId : request.session.activeProject.id))
          .then(function(views) {
            return response.json(views);
          })

          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      },

      listByService: function(request, response) {
        var serviceId = request.params.service_id;
        var projectId = request.params.project_id;
        
        ViewFacade.retrieve(null, projectId, serviceId)
          .then(function(views) {
            return response.json(views);
          })

          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      },

      post: function(request, response) {
        var viewObject = request.body;
        var shouldRun = request.body.run;

        ViewFacade.save(viewObject, request.session.activeProject.id)
          .then(function(view) {
            var extra = {};
            if (shouldRun){
              extra = {
                id: view.id
              }
            }
            // generating token
            var token = Utils.generateToken(app, TokenCode.SAVE, view.name, extra);
            return response.json({status: 200, result: view.toObject(), token: token});
          })
          .catch(function(err){
            return handleRequestError(response, err, 400);
          });
      },
      
      put: function(request, response) {
        var viewId = parseInt(request.params.id);
        var shouldRun = request.body.run;

        ViewFacade.update(viewId, request.body, request.session.activeProject.id)
          .then(function(view) {
            var extra = {};
            if (shouldRun){
              extra = {
                id: viewId
              }
            }
            var token = Utils.generateToken(app, TokenCode.UPDATE, view.name, extra);
            return response.json({status: 200, result: view.toObject(), token: token});
          })

          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      },

      delete: function(request, response) {
        var viewId = parseInt(request.params.id);

        ViewFacade.remove(viewId)
          .then(function(view) {
            var token = Utils.generateToken(app, TokenCode.DELETE, view.name);
            return response.json({status: 200, result: view.toObject(), token: token});
          })
          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      }
    }
  };
} ());