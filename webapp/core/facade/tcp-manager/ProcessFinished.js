(function() {
  'use strict';
  var Utils = require("./../../Utils");
  var PromiseClass = require("./../../Promise");
  var ServiceTypeError = require("./../../Exceptions").ServiceTypeError;
  var RegisteredViewError = require("./../../Exceptions").RegisteredViewError;
  var CollectorErrorNotFound = require("./../../Exceptions").CollectorErrorNotFound;
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
            case ServiceType.ANALYSIS:
            case ServiceType.COLLECTOR:
              handler = self.retrieveProcessFinished(service, response);
              break;
            case ServiceType.VIEW:
              /**
               * When a service is view, we must both prepare registered view and notify GUI interface about process finished in order to retrieve a new log
               */
              handler = self.handleRegisteredViews(response)
                            .then(function(registeredView) {
                              output.view = registeredView;
                              return;
                            })
                            .finally(function() {
                              return self.retrieveProcessFinished(service, response);
                            });
              break;
            default:
              throw new ServiceTypeError(Utils.format("Invalid instance id %s", response.instance_id));
          }

          return handler
            .then(function(handlerResult) {
              output.process = handlerResult;
              return resolve(output);
            });
        })
        // on any error
        .catch(function(err) {
          return reject(err);
        });
    });
  };
  /**
   * It aims to retrieve the context of process executed.
   * 
   * @param {Service} service, TerraMA² Service
   * @param {number}  response.instance_id - Service Identifier retrieved by C++
   * @param {number}  response.process_id - Process Identifier. It may be Analysis or Collector. (It is not a View due event handler before (self.handler));
   * @param {boolean} response.result - Flag to determines if instance generate a result
   * @returns {Promise<any>} A promise with meta result to help display process owner
   */
  ProcessFinished.retrieveProcessFinished = function retrieveProcessFinished(service, response) {
    return new PromiseClass(function(resolve, reject) {
      return DataManager.getCollector({id: response.process_id, service_instance_id: response.instance_id})
        .then(function(collector) {
          var output = {
            service: service.id,
            name: collector.dataSeriesOutput.name,
            process: collector.toObject()
          };
          return output;
        })
        .catch(function(err) {
          function _handleUnexpectedError(error) {
            return reject(new Error(Utils.format("An unexpected error occurred while retrieving process metadata %s", error.toString())));
          }
          if (err instanceof CollectorErrorNotFound) {
            return DataManager.getAnalysis({id: response.process_id, instance_id: response.instance_id})
              .then(function(analysis) {
                var output = {
                  service: service.id,
                  name: analysis.name,
                  process: analysis.toObject()
                };
                return output;
              })
              .catch(_handleUnexpectedError);
          }
          return _handleUnexpectedError(err);
        })
        // Once everything OK, resolve promise chain
        .tap(function(output) {
          return resolve(output);
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
                return DataManager.getRegisteredView({view_id: registeredViewObject.process_id}, options);
              });
          })
          // NotFound... tries to insert a new one
          .catch(function(err) {
            if (err instanceof RegisteredViewError) {
              registeredViewObject.uri = registeredViewObject.maps_server_uri;
              registeredViewObject.view_id = registeredViewObject.process_id;
              return DataManager.addRegisteredView(registeredViewObject, options)
                .then(function(registeredView) {
                  return DataManager.getRegisteredView({id: registeredView.id}, options);
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

} ());