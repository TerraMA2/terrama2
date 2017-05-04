(function() {
  'use strict';
  var Utils = require("./../../Utils");
  var PromiseClass = require("./../../Promise");
  var ServiceTypeError = require("./../../Exceptions").ServiceTypeError;
  var RegisteredViewError = require("./../../Exceptions").RegisteredViewError;
  var ServiceType = require("./../../Enums").ServiceType;
  /**
   * TerraMA² DataManager module
   * @type {DataManager}
   */
  var DataManager = require("./../../DataManager");

  /**
   * It represents a mock to handle ProcessFinished events, inserting/updating object retrieved from C++ services
   * 
   * @class ProcessFinished
   */
  var ProcessFinished = module.exports = {};
  /**
   * It handles process finished response from C++ service.
   * 
   * @param {Object} response - A registered object retrieved from C++ services.
   * @param {number} instance_id - A TerraMA² service instance identifier
   * @param {number} process_id - A TerraMA² process executed. The process may be: View, Analysis or Collector.
   * @param {?} response.any - A response values retrieved from C++ services
   * @returns {Promise<?>} 
   */
  ProcessFinished.handle = function(response) {
    var self = this;
    return new PromiseClass(function(resolve, reject) {
      // retrieving service instance
      return DataManager.getServiceInstance({id: response.instance_id})
        .then(function(service) {
          var handler = null;
          var output = {};
          switch(service.service_type_id) {
            case ServiceType.COLLECTOR:
              handler = self.handleFinishedCollector(response);
              break;
            case ServiceType.ANALYSIS:
              handler = self.handleFinishedAnalysis(response);
              break;
            case ServiceType.VIEW:
              handler = self.handleRegisteredViews(response);
              break;
            default:
              throw new ServiceTypeError(Utils.format("Invalid instance id %s", response.instance_id));
          }

          return handler
            .then(function(handlerResult) {
              return resolve(handlerResult);
            });
        })
        // on any error
        .catch(function(err) {
          return reject(err);
        });
    });
  };
  /**
   * It handles registered views process finished. Once values received, it tries to retrieve a registered view
   * by view_id. If found, applies upSert layers operation (insert or update). If not found, add a new registered view
   * 
   * @param {Object} registeredViewObject - A registered object retrieved from C++ services.
   * @param {string} class - A class name
   * @param {number} process_id - Determines which view were processed
   * 
   * @returns {Promise<RegisteredView>}
   */
  ProcessFinished.handleRegisteredViews = function(registeredViewObject) {
    return new PromiseClass(function(resolve, reject) {
      // preparing transaction mode
      return DataManager.orm.transaction(function(t) {
        var options = {transaction: t};
        var registeredView;

        return DataManager.getRegisteredView({view_id: registeredViewObject.process_id}, options)
          // on registeredView retrieved, performs update layers
          .then(function(registeredViewResult) {
            registeredView = registeredViewResult;
            // performs update registered view (updated_at)
            return DataManager.updateRegisteredView({id: registeredView.id}, registeredViewObject, options);
          })

          .then(function() {
            var promises = [];
            registeredViewObject.layers_list.forEach(function(layer) {
              promises.push(DataManager.upsertLayer({registered_view_id: registeredView.id, name: layer.layer}, {
                name: layer.layer,
                registered_view_id: registeredView.id
              }, options));
            });
            return PromiseClass.all(promises)
              .then(function() {
                var registeredView = DataManager.getRegisteredView({view_id: registeredViewObject.process_id}, options);
                var objectResponse = {
                  serviceType: ServiceType.VIEW,
                  registeredView: registeredView
                };
                return objectResponse;
              });
          })
          // NotFound... tries to insert a new one
          .catch(function(err) {
            if (err instanceof RegisteredViewError) {
              registeredViewObject.uri = registeredViewObject.maps_server;
              registeredViewObject.view_id = registeredViewObject.process_id;
              return DataManager.addRegisteredView(registeredViewObject, options)
                .then(function(registeredView) {
                  var registeredView = DataManager.getRegisteredView({id: registeredView.id}, options);
                  var objectResponse = {
                    serviceType: ServiceType.VIEW,
                    registeredView: registeredView
                  };
                  return objectResponse;
                });
            } else {
              throw err;
            }
          });
      })
      // on commit
      .then(function(registeredView) {
        return resolve(registeredView);
      })
      // on any error
      .catch(function(err) {
        return reject(new Error(Utils.format("Error during registered views: %s" + err.toString())));
      });
    });
  };
  /**
   * It handles analysis process finished. Once values received, check if the analysis data series is condition to run another process.
   * If is condition, send the view or analysis ids to run.
   * 
   * @param {Object} analysisResultObject - A analysis result object retrieved from C++ services.
   * 
   * @returns {Promise} - Objects with process ids to run
   */
  ProcessFinished.handleFinishedAnalysis = function(analysisResultObject){
    return new PromiseClass(function(resolve, reject){
      if (analysisResultObject.result){
        return DataManager.orm.transaction(function(t){
          var options = {transaction: t};
          return DataManager.getAnalysis({id: analysisResultObject.process_id}, options)
            .then(function(analysis){
              var analysisDatasetOutput = analysis.dataset_output;
              var restritions = {
                data_ids: {
                  $contains: [analysisDatasetOutput]
                }
              };
              //return the process are conditioned by the analysis
              return listConditionedProcess(restritions, options, resolve, reject);
            })
            .catch(function(err){
              return reject(new Error(err.toString()));
            });
        });
      }
      else {
        return reject(new Error("The collector process finished with error"));
      }
    });
  }

  /**
   * It handles collector process finished. Once values received, check if the collector is condition to run another process.
   * If is condition, send the view or analysis ids to run.
   * 
   * @param {Object} collectorResultObject - A collector result object retrieved from C++ services.
   * 
   * @returns {Promise} - Objects with process ids to run
   */
  ProcessFinished.handleFinishedCollector = function(collectorResultObject){
    return new PromiseClass(function(resolve, reject){
      if (collectorResultObject.result){
        return DataManager.orm.transaction(function(t){
          var options = {transaction: t};
          // Get collector object that run
          return DataManager.getCollector({id: collectorResultObject.process_id}, options)
            .then(function(collector){
              var dataSeriesId = collector.data_series_output;
              var restritions = {
                data_ids: {
                  $contains: [dataSeriesId]
                }
              };
              // return the process are conditioned by collector
              return listConditionedProcess(restritions, options, resolve, reject);
            })
            .catch(function(err){
              return reject(new Error(err.toString()));
            });
        });
      } else {
        return reject(new Error("The collector process finished with error"));
      }
    });
  };
  /**
   * Function to list conditioned process
   */
  var listConditionedProcess = function(restritions, options, resolve, reject){
    // Get conditional schedule list that contais the collector
    return DataManager.listConditionalSchedule(restritions, options)
      .then(function(conditionalScheduleList){
        if (conditionalScheduleList.length > 0){
          var promises = [];
          //for each conditional schedule in list, check if belong to an analysis or a view
          conditionalScheduleList.forEach(function(conditionalSchedule){
            promises.push(DataManager.getAnalysis({conditional_schedule_id: conditionalSchedule.id}, options)
              .then(function(analysisResult){
                return DataManager.getServiceInstance({id: analysisResult.instance_id}, options)
                  .then(function(instanceServiceResponse){
                    var objectToRun = {
                      ids: [analysisResult.id],
                      instance: instanceServiceResponse,
                    };
                    return objectToRun;
                  })
              })
              .catch(function(err){
                return DataManager.getView({conditional_schedule_id: conditionalSchedule.id}, options)
                  .then(function(viewResult){
                    return DataManager.getServiceInstance({id: viewResult.serviceInstanceId}, options)
                      .then(function(instanceServiceResponse){
                        var objectToRun = {
                          ids: [viewResult.id],
                          instance: instanceServiceResponse,
                        };
                        return objectToRun;
                      });
                  })
                  .catch(function(err){
                    return DataManager.getAlert({conditional_schedule_id: conditionalSchedule.id}, options)
                      .then(function(alertResult){
                        return DataManager.getServiceInstance({id: alertResult.instance_id}, options)
                          .then(function(instanceServiceResponse){
                            var objectToRun = {
                              ids: [alertResult.id],
                              instance: instanceServiceResponse
                            };
                            return objectToRun;
                          });
                      })
                      .catch(function(){
                        return null;
                      });
                  });
              }));
          });
          return Promise.all(promises).then(function(processToRun){
            var objectResponse = {
              serviceType: ServiceType.COLLECTOR,
              processToRun: processToRun
            };
            return resolve(objectResponse);
          });
        } else {
          return resolve();
        }
      })
      .catch(function(err){
        return reject(new Error(err.toString()));
      });
  };

} ());