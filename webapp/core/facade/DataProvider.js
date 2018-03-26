(function(){
  'use strict';

  var RequestFactory = require("./../RequestFactory");
  var PromiseClass = require("./../Promise");
  var DataManager = require("./../DataManager");
  var TcpService = require("./../facade/tcp-manager/TcpService");
  var PostgisRequest = require("./../PostgisRequest");
  
  var DataProvider = module.exports = {};

  /**
   * Helper to send data provider via TCP
   * 
   * @param {Array|Object} args A data provider values to send
   */
  function sendDataProviders(args) {
    var objToSend = {
      "DataProviders": []
    };
    if (args instanceof Array) {
      args.forEach(function(arg) {
        objToSend.DataProviders.push(arg.toService());
      });
    } else {
      objToSend.DataProviders.push(args.toService());
    }

    TcpService.send(objToSend);
  }

  /**
   * Helper function to parse a PostGIS URI.
   * 
   * @returns {object}
   */
  function getPostgisUriInfo(uri) {
    var params = {};
    params.protocol = uri.split(':')[0];
    var hostData = uri.split('@')[1];

    if(hostData) {
      params.hostname = hostData.split(':')[0];
      params.port = hostData.split(':')[1].split('/')[0];
      params.database = hostData.split('/')[1];
    }

    var auth = uri.split('@')[0];

    if(auth) {
      var userData = auth.split('://')[1];

      if(userData) {
        params.user = userData.split(':')[0];
        params.password = userData.split(':')[1];
      }
    }

    return params;
  }
  
  /**
   * It applies a save operation and send data providers to the service
   * 
   * @param {Object} dataProviderReceived - A data provider object to save
   * @param {number} projectId - A project identifier
   * @param {Object} options - Transaction options
   * @returns {Promise<DataProvider>}
   */
  DataProvider.save = function(dataProviderReceived, projectId, options){
    return new PromiseClass(function(resolve, reject){
      var uriObject = dataProviderReceived.uriObject;

      var requester = RequestFactory.build(uriObject);

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

              if (uriObject.hasOwnProperty("timeout")) {
                dataProviderObject['configuration'] = {
                  timeout: uriObject.timeout
                }
                
                if (uriObject.hasOwnProperty("active_mode"))
                  dataProviderObject['configuration']['active_mode'] = uriObject.active_mode;
              }

              // try to save
              return DataManager.addDataProvider(dataProviderObject, options).then(function(result) {
                sendDataProviders(result);
                return resolve(result);
              }).catch(function(err) {
                return reject(err);
              });
            }).catch(function(err) {
              return reject(err);
            });
          }).catch(function(err) {
            return reject(err);
          });

        }).catch(function(err) {
          return reject(err);
        });
      };

      // check connection
      return requester.request()
        .finally(function() {
          _makeProvider();
        });
    });
  };
  /**
   * It retrieves data providers from database. It applies a filter by restriction if there is.
   * 
   * @param {number} restriction - Data provider restriction
   * @param {number} projectId - A project identifier
   * @returns {Promise<DataProvider[]>}
   */
  DataProvider.retrieve = function(restriction, projectId){
    return new PromiseClass(function(resolve, reject){
      if (restriction) {
        restriction.project_id = projectId;
        return DataManager.getDataProvider(restriction)
          .then(function(dataProvider) {
            return resolve(dataProvider.toObject());
          }).catch(function(err) {
            return reject(err);
          });
      }
      var output = [];
      DataManager.listDataProviders({project_id: projectId}).forEach(function(element) {
        output.push(element.rawObject());
      });
      return resolve(output);
    });
  };
  /**
   * It performs update data provider from database from data provider identifier
   * 
   * @param {number} dataProviderId - Data Provider Identifier
   * @param {Object} dataProviderObject - Data Provider object values
   * @param {number} projectId - A project identifier
   * @returns {Promise<DataProvider>}
   */
  DataProvider.update = function(dataProviderId, dataProviderObject, projectId){
    return new PromiseClass(function(resolve, reject){

      var uriObject = dataProviderObject.uriObject;
      var requester = RequestFactory.build(uriObject);

      var toUpdate = {
        name: dataProviderObject.name,
        active: dataProviderObject.active,
        timeout: dataProviderObject.timeout,
        description: dataProviderObject.description,
        uri: requester.uri
      };

      if (uriObject.hasOwnProperty("timeout")) {
        dataProviderObject['configuration'] = {
          timeout: uriObject.timeout
        }

        if (uriObject.hasOwnProperty("active_mode"))
          dataProviderObject['configuration']['active_mode'] = uriObject.active_mode;
      }

      return DataManager.updateDataProvider(dataProviderId, toUpdate)
        .then(function() {
          return DataManager.getDataProvider({id: dataProviderId, project_id: projectId}).then(function(dProvider) {
            sendDataProviders(dProvider);
            return resolve(dProvider);
          }).catch(function(err) {
            return reject(err);
          });
        }).catch(function(err) {
          return reject(err);
        });
    });
  };
  /**
   * It changes the status of a given Data Provider
   * 
   * @param {number} dataProviderId - Data Provider Identifier
   * @returns {Promise<DataProvider>}
   */
  DataProvider.changeStatus = function(dataProviderId) {
    return new PromiseClass(function(resolve, reject) {
      return DataManager.changeDataProviderStatus({ id: parseInt(dataProviderId) }).then(function(dataProvider) {
        return TcpService.send({
          "DataProviders": [dataProvider.toService()]
        });
      }).then(function() {
        resolve();
      }).catch(function(err) {
        reject(err);
      });
    });
  };
  /**
   * It performs remove data provider from database from data provider identifier
   * 
   * @param {number} dataProviderId - Data provider Identifier
   * @param {Object} dataProviderObject - Data provider object values
   * @param {number} projectId - A project identifier
   * @returns {Promise<DataProvider>}
   */
  DataProvider.remove = function(dataProviderId){
    return new PromiseClass(function(resolve, reject){
      return DataManager.getDataProvider({id: dataProviderId}).then(function(dProvider) {
        return DataManager.listDataSeries({data_provider_id: dataProviderId}).then(function(dataSeriesOfProvider){

          if(dataSeriesOfProvider.length > 0){
            var dataSeriesNames = [];
            dataSeriesOfProvider.forEach(function(dataSeries){
              dataSeriesNames.push(dataSeries.name);
            });
            throw new Error(dataSeriesNames.join(", "));
          }

          return DataManager.removeDataProvider({id: dataProviderId}).then(function(result) {
            var dataSeries = result.dataSeries;
            var dataProvider = result.dataProvider;
            TcpService.remove({
              "DataProvider": [dProvider.id],
              "DataSeries": dataSeries.map(function(dSeries) { return dSeries.id; })
            });
            return resolve(dProvider);
          });
        })
      }).catch(function(err) {
        return reject(err);
      });
    });
  };
  /**
   * It lists objects of data provider when POSTGIS type
   * 
   * @param {number} objectToList - Info to get objects
   * @returns {Promise<Object>[]}
   */
  DataProvider.listObjects = function(objectToList){
    return new PromiseClass(function(resolve, reject){
      var providerId = objectToList.providerId;
      var objectToGet = objectToList.objectToGet;
      var tableName = objectToList.tableName;
      var columnName = objectToList.columnName;
      return DataManager.getDataProvider({id: providerId})
        .then(function(dataProvider) {
          var providerObject = dataProvider.toObject();
          var uriInfo = getPostgisUriInfo(providerObject.uri);
          uriInfo.objectToGet = objectToGet;
          uriInfo.tableName = tableName;
          uriInfo.columnName = columnName;
          var postgisRequest = new PostgisRequest(uriInfo);
          return postgisRequest.get()
            .then(function(data){
              return resolve(data);
            }).catch(function(err){
              return reject(err);
            });
        }).catch(function(err){
          return reject(err);
        });
    });
  };

}());