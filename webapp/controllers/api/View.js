(function() {
  'use strict';

  // Dependencies
  var DataManager = require("./../../core/DataManager");
  var handleRequestError = require("./../../core/Utils").handleRequestError;

  // Facade
  var ViewFacade = require("./../../core/facade/View");

  var TokenCode = require('./../../core/Enums').TokenCode;
  var Utils = require('./../../core/Utils');

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
        
        ViewFacade.retrieve(viewId)
          .then(function(views) {
            return response.json(views);
          })

          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      },

      post: function(request, response) {
        var viewObject = request.body;

        ViewFacade.save(viewObject)
          .then(function(view) {
            // generating token
            var token = Utils.generateToken(app, TokenCode.SAVE, view.name);
            return response.json({status: 200, result: view.toObject(), token: token});
          })
          
          .catch(function(err){
            return handleRequestError(response, err, 400);
          });
      },
      
      put: function(request, response) {
        var viewId = parseInt(request.params.id);

        ViewFacade.update(viewId, request.body)
          .then(function(view) {
            var token = Utils.generateToken(app, TokenCode.UPDATE, view.name);
            return response.json({status: 200, result: view.toObject(), token: token});
          })

          .catch(function(err) {
            return handleRequestError(response, err, 400);
          });
      }
    }
  };
} ());