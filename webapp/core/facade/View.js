(function() {
  'use strict';

  var DataManager = require("./../DataManager");
  var TcpService = require("./../facade/tcp-manager/TcpService");
  var Enums = require("./../Enums");
  var Utils = require("./../Utils");
  var PromiseClass = require("./../Promise");
  var _ = require("lodash");
  /**
   * @type {RequestFactory}
   */
  var RequestFactory = require("./../RequestFactory");

  /**
   * It represents a mock to handle view.
   * It is used in View API
   * 
   * @class View
   */
  var View = module.exports = {};

  /**
   * Helper to send views via TCP
   * 
   * @param {Array|Object} args A view values to send
   */
  function sendView(args) {
    var objToSend = {
      "Views": []
    };
    if (args instanceof Array) {
      args.forEach(function(arg) {
        objToSend.Views.push(arg.toObject());
      });
    } else {
      objToSend.Views.push(args.toObject());
    }

    TcpService.send(objToSend);
  }
  /**
   * It applies a save operation and send views to the service
   * 
   * @param {Object} viewObject - A view object to save
   * @param {number} projectId - A project identifier
   * @param {Object} options - Transaction options
   * @returns {Promise<View>}
   */
  View.save = function(viewObject, projectId, options) {
    return new PromiseClass(function(resolve, reject) {
      var saveViewPromise;
      // Function to save view and dependencies on db
      var saveViewOnDb = function(options){
        // setting current project scope
        viewObject.project_id = projectId;

        var promiser;

        if (Utils.isEmpty(viewObject.schedule)) {
          promiser = PromiseClass.resolve();
        } else {
          promiser = DataManager.addSchedule(viewObject.schedule, options);
        }

        return promiser
          .then(function(schedule) {
            if (schedule) {
              if (viewObject.schedule_type == Enums.ScheduleType.AUTOMATIC){
                viewObject.automatic_schedule_id = schedule.id;
              } else {
                viewObject.schedule_id = schedule.id;
              }
            }
            return DataManager.addView(viewObject, options);
          });
      };

      // If already have a transaction, use to save view
      // else create a transaction
      if (options){
        saveViewPromise = saveViewOnDb(options);
      } else {
        saveViewPromise = DataManager.orm.transaction(function(t) {
          var options = {transaction: t};
          return saveViewOnDb(options);
        })
      }
      saveViewPromise
        .then(function(view) {
          // sending to the services
          sendView(view);
          return resolve(view);
        })
        .catch(function(err){
          return reject(err);
        });
    });
  };
  /**
   * It retrieves views from database. It applies a filter by ID if there is.
   * 
   * @param {number} viewId - View Identifier
   * @param {number} projectId - A project identifier
   * @param {number} serviceId - A service identifier
   * @returns {Promise<View[]>}
   */
  View.retrieve = function(viewId, projectId, serviceId) {
    return new PromiseClass(function(resolve, reject) {
      if (viewId) {
        return DataManager.getView({id: viewId})
          .then(function(view) { 
            return resolve(view.toObject()); })
          .catch(function(err) { 
            return reject(err); 
          });
      }

      if (projectId) {
        return DataManager.listViews({project_id: projectId})
          .then(function(views) {
            return resolve(views.map(function(view) {
              return view.toObject();
            }));
          })

          .catch(function(err) {
            return reject(err);
          });
      }

      if (serviceId) {
        return DataManager.listViews({service_instance_id: serviceId})
          .then(function(views) {
            return resolve(views.map(function(view) {
              return view.toObject();
            }));
          })

          .catch(function(err) {
            return reject(err);
          });
      }

      return DataManager.listViews()
        .then(function(views) {
          return resolve(views.map(function(view) {
            return view.toObject();
          }));
        })

        .catch(function(err) {
          return reject(err);
        });
    });
  };
  /**
   * It performs update view from database from view identifier
   * 
   * @param {number} viewId - View Identifier
   * @param {Object} viewObject - View object values
   * @param {number} projectId - A project identifier
   * @returns {Promise<View>}
   */
  View.update = function(viewId, viewObject, projectId) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};
        /**
         * @type {View}
         */
        var view;
        var removeSchedule = null;
        var scheduleIdToRemove = null;
        var scheduleTypeToRemove = null;
        return DataManager.getView({id: viewId, project_id: projectId}, options)
          .then(function(viewResult) {
            view = viewResult;
            //check if not changed type of schedule
            if (view.scheduleType == viewObject.schedule_type){
              if (view.scheduleType == Enums.ScheduleType.SCHEDULE){
                // update
                return DataManager.updateSchedule(view.schedule.id, viewObject.schedule, options)
                  .then(function() {
                    viewObject.schedule_id = view.schedule.id;
                    return null;
                  });
              } else if (view.scheduleType == Enums.ScheduleType.AUTOMATIC){
                viewObject.automatic_schedule.data_ids = viewObject.schedule.data_ids;
                return DataManager.updateAutomaticSchedule(view.automaticSchedule.id, viewObject.automatic_schedule, options)
                  .then(function(){
                    viewObject.automatic_schedule_id = view.automaticSchedule.id;
                    return null;
                  });
              }
            } else {
              // when change type of schedule
              // if old schedule is MANUAL, create the new schedule
              if (view.scheduleType == Enums.ScheduleType.MANUAL){
                return DataManager.addSchedule(viewObject.schedule, options)
                  .then(function(scheduleResult){
                    if (viewObject.schedule_type == Enums.ScheduleType.SCHEDULE){
                      view.schedule = scheduleResult;
                      viewObject.schedule_id = scheduleResult.id;
                      return null;
                    } else {
                      view.automaticSchedule = scheduleResult;
                      viewObject.automatic_schedule_id = scheduleResult.id;
                      return null;
                    }
                  });
              // if old schedule is SCHEDULE, delete schedule
              } else if (view.scheduleType == Enums.ScheduleType.SCHEDULE){
                removeSchedule = true;
                scheduleIdToRemove = view.schedule.id;
                scheduleTypeToRemove = Enums.ScheduleType.SCHEDULE;
                viewObject.schedule_id = null;
                // if new schedule is AUTOMATIC, create the schedule
                if (viewObject.schedule_type == Enums.ScheduleType.AUTOMATIC){
                  viewObject.schedule.id = null;
                  return DataManager.addSchedule(viewObject.schedule, options)
                    .then(function(scheduleResult){
                      view.automaticSchedule = scheduleResult;
                      viewObject.automatic_schedule_id = scheduleResult.id;    
                    });
                }
              } else {
                removeSchedule = true;
                scheduleIdToRemove = view.automaticSchedule.id;
                scheduleTypeToRemove = Enums.ScheduleType.AUTOMATIC;
                viewObject.automatic_schedule_id = null;
                if (viewObject.schedule_type == Enums.ScheduleType.SCHEDULE){
                  viewObject.schedule.id = null;
                  return DataManager.addSchedule(viewObject.schedule, options)
                    .then(function(scheduleResult){
                      view.schedule = scheduleResult;
                      viewObject.schedule_id = scheduleResult.id;    
                    });
                }

              }
            }
          })

          .then(function() {
            return DataManager.updateView({id: viewId}, viewObject, options)
              .then(function() {
                if (removeSchedule) {
                  if (scheduleTypeToRemove == Enums.ScheduleType.AUTOMATIC){
                    return DataManager.removeAutomaticSchedule({id: scheduleIdToRemove}, options);
                  } else {
                    return DataManager.removeSchedule({id: scheduleIdToRemove}, options);
                  }
                }
              });
          })

          .then(function() {
            if (!Utils.isEmpty(viewObject.legend) && !Utils.isEmpty(viewObject.legend.metadata)) {
              // if there is no legend before, insert a new one
              var legend = viewObject.legend;
              legend.view_id = viewId;
              if (!view.legend) {
                // insert legend
                return DataManager.addViewStyleLegend(legend, options);
              } else { // otherwise, update
                // TODO: remove it. It should just performs upSert operation instead remove and insert. This implementation was necessary due color generation in 
                // front end does not keep ID
                // remove all colors and insert again
                return DataManager.removeViewStyleColor({view_style_id: view.legend.id}, options)
                  .then(function() {
                    // update
                    var promises = [];
                    for(var i = 0; i < legend.colors.length; ++i) {
                      var color = legend.colors[i];
                      delete color.id; // if there is
                      color.view_style_id = view.legend.id;
                      promises.push(DataManager.addViewStyleColor(color, options));
                    }
                    return PromiseClass.all(promises)
                      .then(function() {
                        return DataManager.updateViewStyleLegend({id: view.legend.id}, legend, options);
                      })
                      .then(function() {
                        promises = [];
                        for(var k in legend.metadata) {
                          if (legend.metadata.hasOwnProperty(k)) {
                            promises.push(DataManager.upsertViewStyleLegendMetadata({key: k, legend_id: view.legend.id}, {key: k, value: legend.metadata[k], legend_id: view.legend.id}, options));
                          }
                        }
                        for (var k in view.legend.metadata){
                          if (!legend.metadata.hasOwnProperty(k)) {
                            promises.push(DataManager.removeViewStyleLegendMetadata({key: k, legend_id: view.legend.id}, options));
                          }
                        }
                        return PromiseClass.all(promises);
                      });
                  });
              }
            } else {
              // if there is legend before, remove it
              if (view.legend) {
                return DataManager.removeViewStyleLegend({id: view.legend.id}, options);
            }
              return null;
            }
          })
          // once updated all
          .then(function() {
            return DataManager.getView({id: viewId}, options);
          });
      })

      .then(function(view) {
        sendView(view);

        return resolve(view);
      })

      .catch(function(err) {
        return reject(err);
      });
    });
  };

  /**
   * It performs remove view from database from view identifier
   * 
   * @param {number} viewId - View Identifier
   * @param {Object} viewObject - View object values
   * @param {number} projectId - A project identifier
   * @returns {Promise<View>}
   */
  View.remove = function(viewId) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};
        
        return DataManager.getView({id: viewId}, options)
          .then(function(view) {
            // try get registered view to send info to monitor
            return DataManager.getRegisteredView({view_id: viewId}, options)
              .then(function(registeredView){
                return DataManager.removeView({id: viewId}, options)
                  .then(function(){
                    var viewObject = {
                      view: view,
                      registeredView: registeredView
                    };
                    return viewObject;
                  })
              })
              .catch(function(){
                return DataManager.removeView({id: viewId}, options)
                  .then(function() {
                    var viewObject = {
                      view: view
                    };
                    return viewObject;
                  });

              });
          });
      })
      
      .then(function(viewObject) {
        // removing views from tcp services
        TcpService.remove({"Views": [viewObject.view.id]});
        // if has view on monitor, send signal to remove
        if (viewObject.registeredView){
          TcpService.removeView(viewObject.registeredView);
        }
        return resolve(viewObject.view);
      })
      
      .catch(function(err) {
        return reject(err);
      });
    });
  };
} ());