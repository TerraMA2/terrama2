"use strict";

/**
 * Socket responsible for manipulating monitor states.
 * @class MonitorStateSocket
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberSockets - Sockets object.
 * @property {object} memberDataManager - TerraMA² data manager.
 */
var MonitorStateSocket = function(io) {

  // Sockets object
  var memberSockets = io.sockets;
  // TerraMA² data manager
  var memberDataManager = require("../core/DataManager.js");

  // Socket connection event
  memberSockets.on('connection', function(client) {

    client.on('addState', function(json) {
      var stateObj = {
        name: json.name,
        state: json.state,
        user_id: json.user
      };

      memberDataManager.addMonitorState(stateObj).then(function(state) {
        client.emit('addStateResponse', { error: null, state: state });
      }).catch(function(err) {
        client.emit('addStateResponse', { error: err.message });
      });
    });

    client.on('updateState', function(json) {
      var stateObj = {
        name: json.name,
        state: json.state
      };

      memberDataManager.updateMonitorState({ id: json.id }, stateObj).then(function(state) {
        client.emit('updateStateResponse', { error: null, state: state });
      }).catch(function(err) {
        client.emit('updateStateResponse', { error: err.message });
      });
    });

    client.on('getState', function(json) {
      memberDataManager.getMonitorState({ user_id: json.user }).then(function(state) {
        client.emit('getStateResponse', { error: null, state: state });
      }).catch(function(err) {
        client.emit('getStateResponse', { error: err.message });
      });
    });

    client.on('removeState', function(json) {
      memberDataManager.removeMonitorState({ user_id: json.user }).then(function() {
        client.emit('removeStateResponse', { error: null });
      }).catch(function(err) {
        client.emit('removeStateResponse', { error: err.message });
      });
    });
  });
};

module.exports = MonitorStateSocket;
