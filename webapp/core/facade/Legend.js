(function(){
  "use strict";

  var DataManager = require("./../DataManager");
  var TcpService = require("./../facade/tcp-manager/TcpService");
  var PromiseClass = require("./../Promise");

  /**
   * It represents a mock to handle legend.
   * It is used in Legend API
   * 
   * @class Legend
   */
  var Legend = module.exports = {};

  /**
   * Helper to send legends via TCP
   * 
   * @param {Array|Object} args A legend values to send
   */
  function sendLegend(args) {
    var objToSend = {
      "Legends": []
    };

    if(args instanceof Array) {
      args.forEach(function(arg) {
        objToSend.Legends.push(arg.toService());
      });
    } else {
      objToSend.Legends.push(args.toService());
    }

    TcpService.send(objToSend);
  }

  /**
   * It applies a save operation and send legend to the service
   * 
   * @param {Object} legendObject - A legend object to save
   * @param {number} projectId - A project identifier
   * @returns {Promise<View>}
   */
  Legend.save = function(legendObject, projectId) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};

        // setting current project scope
        legendObject.project_id = projectId;

        return DataManager.addLegend(legendObject, options);
      }).then(function(legend) {
        // sending to the services
        sendLegend(legend);
        return resolve(legend);
      }).catch(function(err) {
        return reject(err);
      });
    });
  };

  /**
   * It retrieves legends from database. It applies a filter by ID if there is.
   * 
   * @param {number} legendId - View Identifier
   * @param {number} projectId - A project identifier
   * @returns {Promise<View[]>}
   */
  Legend.retrieve = function(legendId, projectId) {
    return new PromiseClass(function(resolve, reject) {
      if(legendId) {
        return DataManager.getLegend({id: legendId}).then(function(legend) { 
          return resolve(legend.toObject()); 
        }).catch(function(err) { 
          return reject(err); 
        });
      }

      return DataManager.listLegends({ project_id: projectId }).then(function(legends) {
        return resolve(legends.map(function(legend) {
          return legend.toObject();
        }));
      }).catch(function(err) {
        return reject(err);
      });
    });
  };

  /**
   * It performs update legend from database from legend identifier
   * 
   * @param {number} legendId - Legend Identifier
   * @param {Object} legendObject - Legend object values
   * @param {number} projectId - A project identifier
   * @returns {Promise<View>}
   */
  Legend.update = function(legendId, legendObject, projectId) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};

        return DataManager.getLegend({ id: legendId }, options).then(function(legendResult) {
          // updating legend
          return DataManager.updateLegend({ id: legendId }, legendObject, options).then(function() {
            return DataManager.getLegend({id: legendId}, options);
          })
        })
      }).then(function(legend) {
        sendLegend(legend);
        return resolve(legend);
      }).catch(function(err) {
        return reject(err);
      })
    });
  };

  /**
   * It performs remove view from database from view identifier
   * 
   * @param {number} legendId - Legend Identifier
   * @param {Object} viewObject - View object values
   * @param {number} projectId - A project identifier
   * @returns {Promise<View>}
   */
  Legend.remove = function(legendId) {
    return new PromiseClass(function(resolve, reject) {
      DataManager.orm.transaction(function(t) {
        var options = {transaction: t};
        
        return DataManager.getLegend({ id: legendId }, options).then(function(legend) {
          return DataManager.removeLegend({ id: legendId }, options).then(function() {
            return legend;
          });
        });
      }).then(function(legend) {
        // removing views from tcp services
        TcpService.remove({"Legends": [legend.id]});
        return resolve(legend);
      }).catch(function(err) {
        return reject(err);
      });
    });
  };
}());