"use strict";

/**
 * Controller responsible for retrieving monitor state data.
 * @class MonitorState
 *
 * @author Jean Souza [jean.souza@funcate.org.br]
 *
 * @property {object} memberDataManager - 'DataManager' class.
 */
var MonitorState = function(app) {

  // 'DataManager' class
  var memberDataManager = require("../../core/DataManager");

  var saveState = function(request, response) {
    memberDataManager.getUser({ token: request.body.userToken }).then(function(user) {
      var stateObj = {
        name: "User State",
        state: request.body.state,
        user_id: user.id
      };

      memberDataManager.addMonitorState(stateObj).then(function(state) {
        response.json({ state: state });
      }).catch(function(err) {
        response.json({});
      });
    }).catch(function(err) {
      response.json({});
    });
  };

  var updateState = function(request, response) {
    var stateObj = {
      name: request.body.name,
      state: request.body.state
    };

    memberDataManager.updateMonitorState({ id: request.body.id }, stateObj).then(function(state) {
      response.json({ state: state });
    }).catch(function(err) {
      response.json({});
    });
  };

  var getState = function(request, response) {
    memberDataManager.getUser({ token: request.body.userToken }).then(function(user) {
      memberDataManager.getMonitorState({ user_id: user.id }).then(function(state) {
        response.json({
          state: state,
        });
      }).catch(function(err) {
        response.json({});
      });
    }).catch(function(err) {
      response.json({});
    });
  };

  var removeState = function(request, response) {
    memberDataManager.removeMonitorState({ user_id: request.body.user }).then(function() {
      response.json({});
    }).catch(function(err) {
      response.json({});
    });
  };

  return {
    saveState: saveState,
    getState: getState
  };
};

module.exports = MonitorState;