(function(){
  "use strict";

  var Interpolator = module.exports = {};

  var DataManager = require("./../DataManager");
  var PromiseClass = require("./../Promise");

  Interpolator.save = function(interpolatorObject, projectId){
    return new PromiseClass(function(resolve, reject){
      DataManager.orm.transaction(function(t){
        var options = {transaction: t};
        return DataManager.addinterpolator(interpolatorObject, options);
      }).then(function(interpolator){
        return resolve(interpolator);
      }).catch(function(err){
        reject(err);
      });
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

  Interpolator.update = function(interpolatorId, interpolatorObject){
    return new PromiseClass(function(resolve, reject){
      DataManager.orm.transaction(function(t){
        var options = {transaction: t};
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