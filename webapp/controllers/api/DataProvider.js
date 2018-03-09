"use strict";

var Utils = require('./../../core/Utils');
var TokenCode = require('./../../core/Enums').TokenCode;

// Facade
var DataProviderFacade = require("./../../core/facade/DataProvider");

module.exports = function(app) {

  return {
    post: function(request, response) {
      var dataProviderReceived = request.body;
      DataProviderFacade.save(dataProviderReceived, request.session.activeProject.id)
        .then(function(dataProviderResult){
          var token = Utils.generateToken(app, TokenCode.SAVE, dataProviderResult.name);
          return response.json({status: 200, result: dataProviderResult.toObject(), token: token});
        })
        .catch(function(err){
          return Utils.handleRequestError(response, err, 400);
        });
    },

    get: function(request, response) {
      var dataProviderId = request.params.id;
      var project = request.params.project ? request.params.project : request.session.activeProject.id;
      var restriction = request.params.id ? {id: request.params.id} : undefined;

      DataProviderFacade.retrieve(restriction, project)
        .then(function(providers){
          return response.json(providers)
        })
        .catch(function(error){
          return Utils.handleRequestError(response, err, 400);
        });
    },

    put: function(request, response) {
      var dataProviderObject = request.body;
      var dataProviderId = parseInt(request.params.id);

      DataProviderFacade.update(dataProviderId, dataProviderObject, request.session.activeProject.id)
        .then(function(dataProviderResult){
          var token = Utils.generateToken(app, TokenCode.UPDATE, dataProviderResult.name);
          return response.json({status: 200, result: dataProviderResult.toObject(), token: token});
        })
        .catch(function(err){
          return Utils.handleRequestError(response, err, 400);
        });
    },

    delete: function(request, response) {
      var id = parseInt(request.params.id);

      DataProviderFacade.remove(id)
        .then(function(dataProvider){
          var token = Utils.generateToken(app, TokenCode.DELETE, dataProvider.name);
          return response.json({status: 200, result: dataProvider.toObject(), token: token});
        })
        .catch(function(err){
          return Utils.handleRequestError(response, err, 400);
        });
    },

    listObjects: function(request, response){
      DataProviderFacade.listObjects(request.body)
        .then(function(data){
          return response.json({status: 200, data: data});
        })
        .catch(function(err){
          return response.json({status: 400, message: err.message});
        });
    },

    changeStatus: function(request, response) {
      DataProviderFacade.changeStatus(parseInt(request.params.id))
        .then(function() {
          return response.json({});
        })
        .catch(function(err) {
          return response.json({status: 400, message: err.message});
        });
    }
  };
};
