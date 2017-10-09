(function(){
  "use strict";

  var Interpolator = module.exports = {};

  var DataManager = require("./../DataManager");
  var PromiseClass = require("./../Promise");
  var ScheduleType = require("./../Enums").ScheduleType;
  var TcpService = require("./../facade/tcp-manager/TcpService");

  /**
   * Helper to send alerts via TCP
   * 
   * @param {Array|Object} args An interpolator values to send
   */
  function sendInterpolator(args) {
    var objToSend = {
      "Interpolator": [],
      "DataSeries": []
    };
    if (args instanceof Array) {
      args.forEach(function(arg) {
        objToSend.Interpolator.push(arg.toService());
        objToSend.DataSeries.push(arg.dataSeriesOutput.toObject());
      });
    } else {
      objToSend.Interpolator.push(args.toService());
      objToSend.DataSeries.push(args.dataSeriesOutput.toObject());
    }

    TcpService.send(objToSend);
  }

  Interpolator.save = function(interpolatorObject, scheduleObject, projectId){
    return new PromiseClass(function(resolve, reject){
      try{
        // if does not have project_id, getting from cache
        if (!interpolatorObject.project_id)
          interpolatorObject.project_id = projectId;
        if (!interpolatorObject.data_series_output.project_id)
          interpolatorObject.data_series_output.project_id = projectId;
  
        DataManager.orm.transaction(function(t){
          var options = {transaction: t};
          return DataManager.addDataSeries(interpolatorObject.data_series_output, null, options)
            .then(function(dataSeriesResult){
              interpolatorObject.dataSeriesOutput = dataSeriesResult;
              interpolatorObject.data_series_output = dataSeriesResult.id;
              return DataManager.addSchedule(scheduleObject, options).then(function(scheduleResult){
                interpolatorObject.schedule_type = scheduleObject.scheduleType;
                if (scheduleObject.scheduleType == ScheduleType.AUTOMATIC){
                  interpolatorObject.automatic_schedule_id = scheduleResult.id
                } else if (scheduleObject.scheduleType == ScheduleType.SCHEDULE){
                  interpolatorObject.schedule_id = scheduleResult.id;
                }
                return DataManager.addInterpolator(interpolatorObject, options).then(function(interpolatorResult){
                  return interpolatorResult;
                });
              });
            });
        }).then(function(interpolator){
          sendInterpolator(interpolator);
          return resolve(interpolator);
        }).catch(function(err){
          return reject(err);
        });
      } catch (err){
        return reject(err);
      }
    });
  };

  Interpolator.retrieve = function(interpolatorId, projectId){
    return new PromiseClass(function(resolve, reject){
      if (interpolatorId){
        return DataManager.getInterpolator({id: interpolatorId})
          .then(function(interpolator){
            return resolve(interpolator);
          })
          .catch(function(err){
            return reject(err);
          })
      } else {
        return DataManager.listInterpolators({project_id: projectId})
          .then(function(interpolators){
            return resolve(interpolators);
          })
          .catch(function(err){
            return reject(err);
          });
      }
    });
  };

  Interpolator.update = function(interpolatorId, interpolatorObject, scheduleObject, projectId){
    return new PromiseClass(function(resolve, reject){
      DataManager.orm.transaction(function(t){
        var options = {transaction: t};
        var interpolator;
        var removeSchedule = null;
        var scheduleIdToRemove = null;
        var scheduleTypeToRemove = null;
        interpolatorObject.schedule_type = scheduleObject.scheduleType;
        return DataManager.getInterpolator({id: interpolatorId}, options)
          .then(function(interpolatorResult){
            interpolator = interpolatorResult;

            if (interpolator.schedule_type == scheduleObject.scheduleType){
              if (interpolator.schedule_type == ScheduleType.SCHEDULE){
                // update
                return DataManager.updateSchedule(interpolator.schedule.id, scheduleObject, options)
                .then(function() {
                  interpolatorObject.schedule_id = interpolator.schedule_id;
                  return null;
                });
              } else if (interpolator.schedule_type == ScheduleType.AUTOMATIC) {
                return DataManager.updateAutomaticSchedule(interpolator.automaticSchedule.id, scheduleObject, options)
                .then(function(){
                  interpolatorObject.automatic_schedule_id = interpolator.automatic_schedule_id;
                  return null;
                });
              }
            } else {
              // when change type of schedule
              // if old schedule is MANUAL, create the new schedule
              if (interpolator.schedule_type == ScheduleType.MANUAL){
                return DataManager.addSchedule(scheduleObject, options)
                  .then(function(scheduleResult){
                    if (scheduleObject.scheduleType == ScheduleType.SCHEDULE){
                      interpolator.schedule = scheduleResult;
                      interpolatorObject.schedule_id = scheduleResult.id;
                      return null;
                    } else {
                      interpolator.automaticSchedule = scheduleResult;
                      interpolatorObject.automatic_schedule_id = scheduleResult.id;
                      return null;
                    }
                  });
              // if old schedule is SCHEDULE, delete schedule
              } else if (interpolator.schedule_type == ScheduleType.SCHEDULE){
                removeSchedule = true;
                scheduleIdToRemove = interpolator.schedule.id;
                scheduleTypeToRemove = ScheduleType.SCHEDULE;
                interpolatorObject.schedule_id = null;
                // if new schedule is AUTOMATIC, create the schedule
                if (interpolatorObject.schedule_type == ScheduleType.AUTOMATIC){
                  interpolatorObject.schedule.id = null;
                  return DataManager.addSchedule(scheduleObject, options)
                    .then(function(scheduleResult){
                      interpolator.automaticSchedule = scheduleResult;
                      interpolatorObject.automatic_schedule_id = scheduleResult.id;    
                    });
                }
              } else {
                removeSchedule = true;
                scheduleIdToRemove = interpolator.automaticSchedule.id;
                scheduleTypeToRemove = ScheduleType.AUTOMATIC;
                interpolatorObject.automatic_schedule_id = null;
                if (interpolatorObject.schedule_type == ScheduleType.SCHEDULE){
                  interpolatorObject.schedule.id = null;
                  return DataManager.addSchedule(scheduleObject, options)
                    .then(function(scheduleResult){
                      interpolator.schedule = scheduleResult;
                      interpolatorObject.schedule_id = scheduleResult.id;    
                    });
                }
              }
            }
          })
          .then(function(){
            return DataManager.updateInterpolator({id: interpolatorId}, interpolatorObject, options)
              .then(function(){
                if (removeSchedule) {
                  if (scheduleTypeToRemove == ScheduleType.AUTOMATIC){
                    return DataManager.removeAutomaticSchedule({id: scheduleIdToRemove}, options);
                  } else {
                    return DataManager.removeSchedule({id: scheduleIdToRemove}, options);
                  }
                }
              })
              .then(function(){
                return DataManager.updateDataSeries(interpolatorObject.data_series_output.id, interpolatorObject.data_series_output, options);
              })
              .then(function(){
                return DataManager.getInterpolator({id: interpolatorId}, options);
              });
          })
      })
      .then(function(interpolator){
        sendInterpolator(interpolator);
        return resolve(interpolator);
      });
    });
  };
  
} ());