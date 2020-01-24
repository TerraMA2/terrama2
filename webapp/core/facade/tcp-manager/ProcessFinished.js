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
            case ServiceType.ALERT:
              handler = self.handleFinishedAlert(response);
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
                return DataManager.getRegisteredView({view_id: registeredViewObject.process_id}, options)
                  .then(function(registeredView){
                    var objectResponse = {
                      serviceType: ServiceType.VIEW,
                      registeredView: registeredView
                    };
                    return objectResponse;
                  });
              });
          })
          // NotFound... tries to insert a new one
          .catch(function(err) {
            if (err instanceof RegisteredViewError) {
              registeredViewObject.uri = registeredViewObject.maps_server;
              registeredViewObject.view_id = registeredViewObject.process_id;
              return DataManager.addRegisteredView(registeredViewObject, options)
                .then(function(registeredView) {
                  return DataManager.getRegisteredView({id: registeredView.id}, options)
                    .then(function(registeredView){
                      var objectResponse = {
                        serviceType: ServiceType.VIEW,
                        registeredView: registeredView
                      };
                      return objectResponse;
                    });
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
  ProcessFinished.handleFinishedAnalysis = async (analysisResultObject) =>{
    if(!analysisResultObject.result)
      throw new Error("The analysis process finished with error");

    if (analysisResultObject.result){

      const options = {};

      try {
        const analysis = await DataManager.getAnalysis({id: analysisResultObject.process_id}, options);

        const analysisDataSeries = analysis.dataSeries.id;

        const where = {
          data_ids: {
            $contains: [analysisDataSeries]
          }
        };

        const res = await listConditionedProcess(where, options);

        return res;
      } catch (err) {
        throw err
      }
    }
  };
  /**
   * It handles collector process finished. Once values received, check if the collector is condition to run another process.
   * If is condition, send the view or analysis ids to run.
   *
   * @param {Object} collectorResultObject - A collector result object retrieved from C++ services.
   *
   * @returns {Promise} - Objects with process ids to run
   */
  ProcessFinished.handleFinishedCollector = async (collectorResultObject) =>{
    if(!collectorResultObject.result)
      throw new Error("The collector process finished with error");

    if (collectorResultObject.result){

      const options = {};

      try {
        const collector = await DataManager.getCollector({id: collectorResultObject.process_id}, options);

        var collectorDataSeriesId = collector.data_series_output;

        var restritions = {
          data_ids: {
            $contains: [collectorDataSeriesId]
          }
        };

        const res = await listConditionedProcess(restritions, options);

        return res;
      } catch (err) {
        throw err
      }
    }
  };
  /**
   * It handles alert process finished. Once alertResultObject.notify is true, send signal to web monitor notify the user
   *
   * @param {Object} alertResultObject - An alert result object retrieved from C++ services.
   *
   * @returns {Promise}
   */
  ProcessFinished.handleFinishedAlert = function(alertResultObject){
    return new PromiseClass(function(resolve, reject){
      if (alertResultObject.result){
        return DataManager.orm.transaction(function(t){
          var options = {transaction: t};
          return DataManager.getAlert({id: alertResultObject.process_id}, options)
            .then(function(alert){
              if (alertResultObject.notify && alert.view && alert.view.id){
                return DataManager.getRegisteredView({view_id: alert.view.id}, options)
                  .then(function(registeredView){
                    var objectResponse = {
                      serviceType: ServiceType.ALERT,
                      registeredView: registeredView
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
        })
      } else {
        return reject(new Error("The alert process finished with error"));
      }
    });
  }

  /**
   * Function to list conditioned process
   */
  var listConditionedProcess = function(restritions, options){
    // Get automatic schedule list that contais the collector
    return DataManager.listAutomaticSchedule(restritions, options)
      .then(async function(automaticScheduleList){
        if (automaticScheduleList.length > 0){
          var promises = [];
          //for each automatic schedule in list, check if belong to an analysis or a view

          /**
           * It tries to find the dependency process to dispatch in order to
           * make automatic trigger.
           * Currently, the model AutomaticSchedule does not contain context of ID, which
           * turns out wrong architecture. It should work as PID Resolver.
           * @todo Refactory automatic schedule next version
           */
          for(const automaticSchedule of automaticScheduleList) {
            let result = await DataManager.listAnalysis({ automatic_schedule_id: automaticSchedule.id });

            if (result.length === 0) {
              result = await DataManager.listViews({ automatic_schedule_id: automaticSchedule.id });

              if (result.length === 0) {
                result = await DataManager.listAlerts({ automatic_schedule_id: automaticSchedule.id });

                if (result.length === 0) {
                  continue;
                }
              }
            }

            const modelInstance = result[0];
            const id = modelInstance.instance_id || modelInstance.serviceInstanceId || modelInstance.service_instance_id;

            const serviceInstance = await DataManager.getServiceInstance({ id });

            promises.push({
              ids: [modelInstance.id],
              object: modelInstance,
              instance: serviceInstance,
            });
          }
          return Promise.all(promises).then(function(processToRun){
            var objectResponse = {
              serviceType: ServiceType.COLLECTOR,
              processToRun: processToRun
            };
            return objectResponse;
          });
        } else {
          return;
        }
      });
    };
} ());