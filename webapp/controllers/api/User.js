'use strict';

module.exports = function(app) {
  var logger = require("./../../core/Logger");
  var DataManager = require("./../../core/DataManager");
  var Utils = require("./../../core/Utils");
  var TokenCode = require("./../../core/Enums").TokenCode;

  return {
    get: function(request, response) {
      var userId = request.params.userId;

      var _handleError = function(err) {
        var errors = err.getErrors ? err.getErrors() : [];
        response.statusCode(400);
        response.json({status: 400, message: err.toString(), errors: errors});
      };

      var promise = null;

      if (userId) {
        promise = DataManager.getUser({id: userId}).then(function(user) {
          return {
            id: user.id,
            name: user.name,
            username: user.username,
            cellphone: user.cellphone,
            email: user.email
          };
        });
      } else {
        promise = DataManager.listUsers({});
      }

      promise.then(function(result) {
        response.json(result);
      }).catch(_handleError);
    },
    put: function(request, response) {
      var userId = parseInt(request.params.userId);
      var userObject = request.body;
      if (request.user.id !== userId) {
        response.statusCode = 400;
        return response.json({status: 400, message: "Missing user id", errors: []});
      }
      DataManager.updateUser({id: userId}, userObject).then(function() {
        // todo: token generation
        DataManager.getUser({id: userId}).then(function(user) {
          var token = Utils.generateToken(app, TokenCode.UPDATE, user.name);
          response.json({status: 200, result: {id: user.id, name: user.name, username: user.username, cellphone: user.cellphone, email: user.email},
                         token: token, context: "User"});
        });
      }).catch(function(err) {
        var errors = err.getErrors ? err.getErrors() : [];
        logger.error(err);
        response.statusCode = 400;
        response.json({status: 400, message: err.toString(), errors: errors});
      });
    }
  };
};