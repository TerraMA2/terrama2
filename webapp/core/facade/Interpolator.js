(function(){
  "use strict";

  var Interpolator = module.exports = {};

  var DataManager = require("./../DataManager");
  var PromiseClass = require("./../Promise");
  var ScheduleType = require("./../Enums").ScheduleType;

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
        var removeSchedule = null;
        var scheduleIdToRemove = null;
        var scheduleTypeToRemove = null;
        return DataManager.updateInterpolator({id: interpolatorId }, interpolatorObject, options)
          .then(function(interpolator){
            return resolve(interpolator);
          });
      })
      .then(function(interpolator){
        return resolve(interpolator);
      })
      .catch(function(err){
        return reject(err);
      })
    });
  };

  Interpolator.delete = function(interpolatorId){
    return new PromiseClass(function(resolve, reject){
      DataManager.orm.transaction(function(t){
        var options = {transaction: t};
        return DataManager.removeInterpolator({id: interpolatorId}, options)
          .then(function(interpolatorId){
            return interpolatorId;
          });
      })
      .then(function(interpolatorId){
        return resolve(interpolatorId);
      })
      .catch(function(err){
        return reject(err);
      });
    });
  };
  
} ());