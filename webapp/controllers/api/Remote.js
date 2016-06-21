var TcpManager = require("./../../core/TcpManager");
var DataManager = require("./../../core/DataManager");
var Utils = require('./../../core/Utils');

var _handleError = function(response, err) {
  console.log(err);
  response.status(400);
  response.json({status: 400, message: err.message, online: false});
};

module.exports = function(app) {
  return {
    startService: function(request, response) {
      var serviceId = request.body.serviceId;

      DataManager.getServiceInstance({id: serviceId}).then(function(serviceInstance) {
        TcpManager.startService(serviceInstance).then(function(code) {
          try {
            setTimeout(function() {
              TcpManager.connect(serviceInstance).then(function() {
                // sending update service with data
                TcpManager.updateService(serviceInstance).then(function() {
                  setTimeout(function() {
                    // ping
                    TcpManager.statusService(serviceInstance).then(function(result) {
                      console.log("Result: ", result);
                      Utils.prepareAddSignalMessage(DataManager).then(function(data) {
                        console.log(JSON.stringify(data));
                        TcpManager.sendData(serviceInstance, data);
                        // todo: check it/ping
                        response.json({status: 200, online: Object.keys(result).length > 0});
                      }).catch(function(err) {
                        _handleError(response, err)
                      }); // end prepare
                    }).catch(function(err) {
                      _handleError(response, err);
                    }); // end statusService
                  }, 2000);
                }).catch(function(err) {
                  // error during connection
                  _handleError(response, err);
                });
              }).catch(function(err) {
                _handleError(response, err);
              }) // end connect
            }, 1000);
          } catch (e) {
            _handleError(response, e);
          }
        }).catch(function(err) {
          console.log("start service errror");
          _handleError(response, err);
        }); // end startService
      }).catch(function(err) {
        _handleError(response, err);
      }); // end getServiceInstance
    },

    stopService: function(request, response) {
      var serviceId = request.body.serviceId;

      DataManager.getServiceInstance({id: serviceId}).then(function(serviceInstance) {
        TcpManager.stopService(serviceInstance).then(function(result) {
          console.log("Stoped", result);
          response.json({status: 200, online: false, result: result});
        // TcpManager.statusService(serviceInstance).then(function(statusResult) {
          //   response.json({status: 200, online: statusResult.instance_id == serviceInstance.id, result: result})
          // }).catch(function(err) {
          //   _handleError(response, err);
          // })
        }).catch(function(err) {
          _handleError(response, err);
        })
      }).catch(function(err) {
        _handleError(response, err);
      });
    },

    statusService: function(request, response) {
      var serviceId = request.params.id;
      DataManager.getServiceInstance({id: serviceId}).then(function(serviceInstance) {
        var _sendStatus = function() {
          TcpManager.statusService(serviceInstance).then(function(result) {
            response.json({status: 200, online: result.message.instance_id != 0})
          }).catch(function(err) {
            _handleError(response, err);
          })
        }

        TcpManager.connect(serviceInstance).then(function() {
          _sendStatus()
        }).catch(function(err) {
          _sendStatus();
        });

      }).catch(function(err) {
        _handleError(response, err);
      });
    }
  }
}
