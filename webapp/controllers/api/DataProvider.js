"use strict";

var DataManager = require("../../core/DataManager");
var DataProviderError = require('./../../core/Exceptions').DataProviderError;
var ValidationError = require('./../../core/Exceptions').ValidationError;
var RequestFactory = require("../../core/RequestFactory");
var Utils = require('./../../core/Utils');
var TokenCode = require('./../../core/Enums').TokenCode;
var TcpService = require("./../../core/facade/tcp-manager/TcpService");

module.exports = function(app) {
  return {
    post: function(request, response) {
      var dataProviderReceived = request.body;

      var uriObject = dataProviderReceived.uriObject;

      var requester = RequestFactory.build(uriObject);

      var handleError = function(response, err, code) {
        var errors = err instanceof ValidationError ? err.getErrors() : {};
        response.status(code);
        response.json({status: code || 400, message: err.message, errors: errors});
      };

      var _makeProvider = function() {
        var projectName = dataProviderReceived.project;

        // check project
        return DataManager.getProject({name: projectName}).then(function(project) {
          // getting intent id
          return DataManager.getDataProviderIntent({name: dataProviderReceived.data_provider_intent_name || requester.intent()}).then(function(intentResult) {
            // getting provider type id
            return DataManager.getDataProviderType({name: uriObject[requester.syntax().SCHEME]}).then(function(typeResult) {
              var dataProviderObject = {
                name: dataProviderReceived.name,
                uri: requester.uri,
                description: dataProviderReceived.description,
                data_provider_intent_id: intentResult.id,
                data_provider_type_id: typeResult.id,
                project_id: project.id,
                active: dataProviderReceived.active || false
              };

              // try to save
              return DataManager.addDataProvider(dataProviderObject).then(function(result) {
                TcpService.send({
                  "DataProviders": [result.toObject()]
                });
                // generating token
                var token = Utils.generateToken(app, TokenCode.SAVE, result.name);
                response.json({status: 200, result: result.toObject(), token: token});
              }).catch(function(err) {
                handleError(response, err, 400);
              });
            }).catch(function(err) {
              handleError(response, err, 400);
            });
          }).catch(function(err) {
            handleError(response, err, 400);
          });

        }).catch(function(err) {
          handleError(response, err, 400);
        });
      };

      // check connection
      return requester.request()
        .finally(function() {
          _makeProvider();
        });
    },

    get: function(request, response) {
      var name = request.query.name;
      var project = request.params.project;

      if(project) {
        var output = [];
        DataManager.listDataProviders({project_id: project}).forEach(function(element) {
          output.push(element.rawObject());
        });
        response.json(output);
      } else if(name) {
        return DataManager.getDataProvider({name: name, project_id: app.locals.activeProject.id}).then(function(dataProvider) {
          response.json(dataProvider.toObject());
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        });
      } else {
        var output = [];
        DataManager.listDataProviders({project_id: app.locals.activeProject.id}).forEach(function(element) {
          output.push(element.rawObject());
        });
        response.json(output);
      }
    },

    put: function(request, response) {
      var dataProviderId = request.params.id;
      var uriObject = request.body.uriObject;

      var requester = RequestFactory.build(uriObject);

      var toUpdate = {
        name: request.body.name,
        active: request.body.active,
        description: request.body.description,
        uri: requester.uri
      };

      if (dataProviderId) {
        dataProviderId = parseInt(dataProviderId);
        return DataManager.updateDataProvider(dataProviderId, toUpdate).then(function() {
          return DataManager.getDataProvider({id: dataProviderId, project_id: app.locals.activeProject.id}).then(function(dProvider) {
            TcpService.send({
              "DataProviders": [dProvider.toObject()]
            });
            // generating token
            var token = Utils.generateToken(app, TokenCode.UPDATE, dProvider.name);

            response.json({status: 200, result: dProvider, token: token});
          }).catch(function(err) {
            response.status(400);
            response.json({status: 400, message: err.message});
          });
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        });

      } else {
        response.status(400);
        response.json({status: 400, message: "DataProvider not identified"});
      }
    },

    delete: function(request, response) {
      var id = request.params.id;
      if (id) {
        id = parseInt(id);
        return DataManager.getDataProvider({id: id}).then(function(dProvider) {
          return DataManager.removeDataProvider({id: id}).then(function(result) {
            var dataSeries = result.dataSeries;
            var dataProvider = result.dataProvider;
            TcpService.remove({
              "DataProvider": [dProvider.id],
              "DataSeries": dataSeries.map(function(dSeries) { return dSeries.id; })
            });
            // generating token
            return response.json({status: 200, name: dProvider.name});
          });
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        });
      } else {
        Utils.handleRequestError(response, new DataProviderError("Missing data provider id", []), 400);
      }
    }
  };
};
