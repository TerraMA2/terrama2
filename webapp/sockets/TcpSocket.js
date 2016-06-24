"use strict";

/**
 * Socket responsible for handling Tcp events from TcpManager in front end application.
 * @class TcpSocket
 *
 * @author Raphael Willian da Costa
 *
 * @property {object} iosocket - Sockets object.
 * @property {object} memberScriptChecker - Script checker class.
 */
var TcpSocket = function(io) {

  // Sockets object
  var iosocket = io.sockets;

  // TcpManager
  var TcpManager = require('./../core/TcpManager');

  // TerraMA2 Utils
  var Utils = require('./../core/Utils');

  // DataManager
  var DataManager = require('./../core/DataManager');

  // A bool value to avoid add extra listeners
  var isRegistred = false;

  // Socket connection event
  iosocket.on('connection', function(client) {

    if (!isRegistred) {
      TcpManager.on('serviceStarted', function(service) {
        setTimeout(function() {
          TcpManager.emit('connect', service);
        }, 1000);
      });

      TcpManager.on('serviceConnected', function(service) {
        TcpManager.emit('updateService', service);

        setTimeout(function() {
          TcpManager.emit('statusService', service);
        }, 2000);
      });

      TcpManager.on('statusReceived', function(service, result) {
        Utils.prepareAddSignalMessage(DataManager).then(function(data) {
          TcpManager.emit('sendData', service, data);
        });
      });

      isRegistred = true;
    }

    // tcp listeners
    TcpManager.on('statusReceived', function(service, response) {
      client.emit('statusResponse', {
        status: 200,
        service: service.id,
        online: Object.keys(response).length > 0
      });
    });

    TcpManager.on('logReceived', function(service, response) {
// todo
    });

    TcpManager.on('stop', function(service, response) {
      client.emit('stopResponse', {
        status: 200,
        online: false,
        service: service.id
      })
    });

    TcpManager.on('close', function(service, response) {
      console.log(response);
      client.emit('closeResponse', {
        status: 400,
        service: service.id,
        online: false
      })
    });

    TcpManager.on('error', function(service, err) {
      client.emit('errorResponse', {
        status: 400,
        message: err.toString(),
        service: service.id
      })
    });

    // client listeners
    client.on('start', function(json) {
      DataManager.getServiceInstance({id: json.service}).then(function(instance) {
        TcpManager.emit('startService', instance);
      }).catch(function(err) {
        console.log(err);
      })
    });
    // end client start listener

    client.on('status', function(json) {
      DataManager.getServiceInstance({id: json.service}).then(function(instance) {
        TcpManager.emit('statusService', instance);
      }).catch(function(err) {
        console.log(err);
        client.emit('statusResponse', {
          status: 400,
          online: false,
          service: json.service
        })
      });
    });
    // end client status listener

    client.on('stop', function(json) {
      DataManager.getServiceInstance({id: json.service}).then(function(instance) {
        TcpManager.emit('stopService', instance);
      }).catch(function(err) {
        console.log(err);
        client.emit('errorResponse', {
          status: 400,
          message: err.toString(),
          service: service.id
        })
      });
    });

    client.on('log', function(json) {
      var begin = json.begin || 0,
        end = json.end || 9;

      DataManager.listCollectors().then(function(collectors) {
        var obj = {
          process_ids: collectors.map(function(element) { return element.id }),
          begin: begin,
          end: end
        };

        TcpManager.emit('logData', null, obj);
      }).catch(function(err) {
        console.log(err);
        client.emit('errorResponse', {
          status: 400,
          message: err.toString()
        })
      })
    });
  });
};

module.exports = TcpSocket;
