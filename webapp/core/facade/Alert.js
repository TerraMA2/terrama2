(function(){
  "use strict";

  var DataManager = require("./../DataManager");
  var TcpService = require("./../facade/tcp-manager/TcpService");
  var PromiseClass = require("./../Promise");

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
      "Alerts": []
    };
    if (args instanceof Array) {
      args.forEach(function(arg) {
        objToSend.Alerts.push(arg.toObject());
      });
    } else {
      objToSend.Alerts.push(args.toObject());
    }

    TcpService.send(objToSend)
      .then(function() {
        if (shouldRun && !(args instanceof Array)) {
          return TcpService.run({"ids": [args.id], "service_instance": args.serviceInstanceId});
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

        var promiser;

        promiser = DataManager.addSchedule(alertObject.schedule, options);

        return promiser
          .then(function(schedule) {
            if (schedule) {
              alertObject.conditional_schedule_id = schedule.id;
            }
            var riskPromise;
            var riskObject = alertObject.risk;
            if (riskObject.id){
              riskPromise = DataManager.updateRisk(riskObject.id, riskObject, options);
            } else {
              riskPromise = DataManager.addRisk(riskObject, options);
            }
            return riskPromise
              .then(function(riskResult){
                alertObject.risk_id = riskResult.id;
                return DataManager.addAlert(alertObject, options);
              });
          });
      })

      .then(function(alert) {
        // sending to the services
        return resolve(alert);
      })
      
      .catch(function(err){
        return reject(err);
      });
    });
  };

}());