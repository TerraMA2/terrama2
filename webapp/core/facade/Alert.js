(function(){
  "use strict";

  var DataManager = require("./../DataManager");
  var TcpService = require("./../facade/tcp-manager/TcpService");
  var PromiseClass = require("./../Promise");
  var Enums = require("./../Enums");
  var Utils = require("./../Utils");
  var ViewFacade = require("./../facade/View")

  /**
   * It represents a mock to handle alert.
   * It is used in Alert API
   * 
   * @class Alert
   */
  var Alert = module.exports = {};

  /**
   * Helper to send alerts via TCP
   * 
   * @param {Array|Object} args A alert values to send
   * @param {boolean} shouldRun - A flag to defines if service should run context view
   */
  function sendAlert(args, shouldRun) {
    var objToSend = {
      "Alerts": [],
      "Legends": []
    };
    if (args instanceof Array) {
      args.forEach(function(arg) {
        objToSend.Alerts.push(arg.toService());
        objToSend.Legends.push(arg.legend.toService());
      });
    } else {
      objToSend.Alerts.push(args.toService());
      objToSend.Legends.push(args.legend.toService());
    }

    TcpService.send(objToSend)
      .then(function() {
        if (shouldRun && !(args instanceof Array)) {
          return TcpService.run({"ids": [args.id], "service_instance": args.service_instance_id});
        }
      });
  }

  /**
   * It applies a save operation and send alert to the service
   * 
   * @param {Object} alertObject - A alert object to save
   * @param {number} projectId - A project identifier
   * @param {boolean} shouldRun - Flag to determines if service should execute immediately after save process
   * @returns {Promise<View>}
   */
  Alert.save = function(alertObject, projectId, shouldRun) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};

        // setting current project scope
        alertObject.project_id = projectId;
        alertObject.legend.project_id = projectId;

        var viewPromise;
        if (alertObject.hasView){
          var viewObject = alertObject.view;
          viewPromise = ViewFacade.save(viewObject, projectId, options);
        } else {
          viewPromise = PromiseClass.resolve();
        }

        return viewPromise.then(function(view){
          if (view){
            alertObject.view_id = view.id;
          }
          var promiser;

		      if (Utils.isEmpty(alertObject.schedule))
            promiser = PromiseClass.resolve();
          else
            promiser = DataManager.addSchedule(alertObject.schedule, options);

          return promiser
		        .then(function(schedule) {
		          if (schedule) {
		            if (alertObject.schedule_type == Enums.ScheduleType.AUTOMATIC)
		              alertObject.automatic_schedule_id = schedule.id;
		            else
		              alertObject.schedule_id = schedule.id;
		          }
		          var legendPromise;
		          var legendObject = alertObject.legend;
		          if (legendObject.id){
		            legendPromise = DataManager.updateLegend({id: legendObject.id}, legendObject, options);
		          } else {
		            legendPromise = DataManager.addLegend(legendObject, options);
		          }
		          return legendPromise
		            .then(function(legendResult){
		              if (!alertObject.legend.id){
		                alertObject.legend_id = legendResult.id;
		              } else {
		                alertObject.legend_id = alertObject.legend.id;
		              }
		              return DataManager.addAlert(alertObject, options);
		            });
		        });

        })
      })

      .then(function(alert) {
        // sending to the services
        sendAlert(alert, shouldRun);
        return resolve(alert);
      })
      
      .catch(function(err){
        return reject(err);
      });
    });
  };

  /**
   * It retrieves alerts from database. It applies a filter by ID if there is.
   * 
   * @param {number} alertId - View Identifier
   * @param {number} projectId - A project identifier
   * @returns {Promise<View[]>}
   */
  Alert.retrieve = function(alertId, projectId) {
    return new PromiseClass(function(resolve, reject) {
      if (alertId) {
        return DataManager.getAlert({id: alertId})
          .then(function(alert) { 
            return resolve(alert.toObject()); 
          })
          .catch(function(err) { 
            return reject(err); 
          });
      }

      return DataManager.listAlerts({project_id: projectId})
        .then(function(alerts) {
          return DataManager.listAnalysis({}).then(function(analysisList) {
            var alertsObjects = [];

            alerts.map(function(alert) {
              var alertObject = alert.toObject();
              var isAnalysis = false;

              analysisList.map(function(analysis) {
                alertObject.dataSeries.dataSets.map(function(dataSet) {
                  if(analysis.dataset_output == dataSet.id) {
                    isAnalysis = true;
                    return;
                  }
                });
              });

              alertObject.dataSeries.isAnalysis = isAnalysis;

              alertsObjects.push(alertObject);
            });

            return resolve(alertsObjects);
          });
        })

        .catch(function(err) {
          return reject(err);
        });
    });
  };

  /**
   * It performs update alert from database from alert identifier
   * 
   * @param {number} alertId - Alert Identifier
   * @param {Object} alertObject - Alert object values
   * @param {number} projectId - A project identifier
   * @param {boolean} shouldRun - Flag to determines if service should execute immediately after save process
   * @returns {Promise<View>}
   */
  Alert.update = function(alertId, alertObject, projectId, shouldRun){
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t){
        var options = {transaction: t};
        var oldAlertNotifications = [];
        var alert;
        var removeSchedule = null;
        var scheduleIdToRemove = null;
        var scheduleTypeToRemove = null;
        var removeView = null;
        var viewIdToRemove = null;
        return DataManager.getAlert({id: alertId}, options)
          .then(function(alertResult){
            alert = alertResult;

            if (alert.scheduleType == alertObject.schedule_type){
              if (alert.scheduleType == Enums.ScheduleType.SCHEDULE){
                // update
                return DataManager.updateSchedule(alert.schedule.id, alertObject.schedule, options)
                  .then(function() {
                    alertObject.schedule_id = alert.schedule.id;
                    return null;
                  });
              } else if (alert.scheduleType == Enums.ScheduleType.AUTOMATIC){
                alertObject.automatic_schedule.data_ids = alertObject.schedule.data_ids;
                return DataManager.updateAutomaticSchedule(alert.automatic_schedule.id, alertObject.automatic_schedule, options)
                  .then(function(){
                    alertObject.automatic_schedule_id = alert.automatic_schedule.id;
                    return null;
                  });
              }
            } else {
              // when change type of schedule
              // if old schedule is MANUAL, create the new schedule
              if (alert.scheduleType == Enums.ScheduleType.MANUAL){
                return DataManager.addSchedule(alertObject.schedule, options)
                  .then(function(scheduleResult){
                    if (alertObject.schedule_type == Enums.ScheduleType.SCHEDULE){
                      alert.schedule = scheduleResult;
                      alertObject.schedule_id = scheduleResult.id;
                      return null;
                    } else {
                      alert.automaticSchedule = scheduleResult;
                      alertObject.automatic_schedule_id = scheduleResult.id;
                      return null;
                    }
                  });
              // if old schedule is SCHEDULE, delete schedule
              } else if (alert.scheduleType == Enums.ScheduleType.SCHEDULE){
                removeSchedule = true;
                scheduleIdToRemove = alert.schedule.id;
                scheduleTypeToRemove = Enums.ScheduleType.SCHEDULE;
                alertObject.schedule_id = null;
                // if new schedule is AUTOMATIC, create the schedule
                if (alertObject.schedule_type == Enums.ScheduleType.AUTOMATIC){
                  alertObject.schedule.id = null;
                  return DataManager.addSchedule(alertObject.schedule, options)
                    .then(function(scheduleResult){
                      alert.automaticSchedule = scheduleResult;
                      alertObject.automatic_schedule_id = scheduleResult.id;    
                    });
                }
              } else {
                removeSchedule = true;
                scheduleIdToRemove = alert.automatic_schedule.id;
                scheduleTypeToRemove = Enums.ScheduleType.AUTOMATIC;
                alertObject.automatic_schedule_id = null;
                if (alertObject.schedule_type == Enums.ScheduleType.SCHEDULE){
                  alertObject.schedule.id = null;
                  return DataManager.addSchedule(alertObject.schedule, options)
                    .then(function(scheduleResult){
                      alert.schedule = scheduleResult;
                      alertObject.schedule_id = scheduleResult.id;    
                    });
                }
              }
            }
          })
          .then(function(){
            oldAlertNotifications = alert.notifications;
            // Updating or adding a legend
            if (alertObject.legend.id){
              alertObject.legend_id = alertObject.legend.id;
              return DataManager.updateLegend({id: alertObject.legend.id}, alertObject.legend, options);
            } else {
              alertObject.legend.project_id = alertObject.project_id;
              return DataManager.addLegend(alertObject.legend, options)
            }
          })
          .then(function(legendResult){
            if (legendResult){
              alertObject.legend_id = legendResult.id;
            }
            //Updating Notifications
            var newAlertNotifications = alertObject.notifications;
            var alertNotificationsPromises = [];
            newAlertNotifications.forEach(function(notification){
              // checking if must update or add a notification
              if (notification.id){
                alertNotificationsPromises.push(DataManager.updateAlertNotification({id: notification.id}, notification, options));
              } else {
                notification.alert_id = alertId;
                alertNotificationsPromises.push(DataManager.addAlertNotification(notification, options));
              }
            });
            // checking if must remove a notification
            oldAlertNotifications.forEach(function(notification){
              var found = newAlertNotifications.some(function(newNotification){
                return newNotification.id === notification.id;
              });
              if (!found){
                alertNotificationsPromises.push(DataManager.removeAlertNotification({id: notification.id}, options));
              }
            });
            return Promise.all(alertNotificationsPromises);
          })
          //updating view alert
          .then(function(){
            var oldView = alert.view;
            var newView = alertObject.view;
            var alertViewPromise;
            if (oldView.id && newView) {
              alertViewPromise = ViewFacade.update(oldView.id, newView, projectId);
            } else if (oldView.id && !newView){
              removeView = true;
              viewIdToRemove = oldView.id;
              alertObject.view_id = null;
              alertViewPromise = PromiseClass.resolve();
            } else if (!oldView.id && newView){
              alertViewPromise = ViewFacade.save(newView, projectId, options);
            } else {
              alertViewPromise = PromiseClass.resolve();
            }
            return alertViewPromise.then(function(alertViewResult){
              if(alertViewResult){
                alertObject.view_id = alertViewResult.id;
              }
              // updating alert
              return DataManager.updateAlert({id: alertId}, alertObject, options)
                .then(function(){
                  if (removeSchedule) {
                    if (scheduleTypeToRemove == Enums.ScheduleType.AUTOMATIC){
                      return DataManager.removeAutomaticSchedule({id: scheduleIdToRemove}, options);
                    } else {
                      return DataManager.removeSchedule({id: scheduleIdToRemove}, options);
                    }
                  }
                })
                .then(function(){
                  if (removeView)
                    return DataManager.removeView({id: viewIdToRemove}, options);
                  else
                    return null;
                })
                .then(function(){
                  return DataManager.getAlert({id: alertId}, options);
                })
            });
          })
      })
      .then(function(alert){
        sendAlert(alert, shouldRun);

        return resolve(alert);
      })
      .catch(function(err){
        return reject(err);
      })
    });
  };

    /**
   * It performs remove view from database from view identifier
   * 
   * @param {number} alertId - Alert Identifier
   * @param {Object} viewObject - View object values
   * @param {number} projectId - A project identifier
   * @returns {Promise<View>}
   */
  Alert.remove = function(alertId) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};
        
        return DataManager.getAlert({id: alertId}, options)
          .then(function(alert) {
            return DataManager.removeAlert({id: alertId}, options)
              .then(function() {
                return alert;
              });
          });
      })
      
      .then(function(alert) {
        // removing views from tcp services
        TcpService.remove({"Alerts": [alert.id]});

        return resolve(alert);
      })
      
      .catch(function(err) {
        return reject(err);
      });
    });
  };
}());