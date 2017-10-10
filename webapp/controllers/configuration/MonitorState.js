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
      memberDataManager.getMonitorState({ user_id: user.id }).then(function(state) {
        var stateObj = {
          name: state.name,
          state: request.body.state
        };

        memberDataManager.updateMonitorState({ id: state.id }, stateObj).then(function(state) {
          response.json({ state: state });
        }).catch(function(err) {
          response.json({});
        });
      }).catch(function(err) {
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
      });
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

  return {
    saveState: saveState,
    getState: getState
  };
};

module.exports = MonitorState;