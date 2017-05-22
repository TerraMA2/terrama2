module.exports = function(app) {
  var logger = require("./../../core/Logger");
  var DataManager = require('../../core/DataManager');
  var Utils = require("../../core/Utils");
  var TokenCode = require('../../core/Enums').TokenCode;
  var UserError = require("../../core/Exceptions").UserError;
  return {
    get: function (request, response) {
      return DataManager.listUsers()
        .then(function(users) {
          var usersArray = [];

          users.forEach(function(userObj) {
            // TODO: use i18n module
            if(userObj.administrator)
              userObj.administrator = "Yes";
            else
              userObj.administrator = "No";

            usersArray.push(userObj);
          });

          var parameters = Utils.makeTokenParameters(request.query.token, app);

          return response.render('administration/users', Object.assign({ usersList: usersArray }, parameters));
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        });
    },

    new: function (request, response) {
      return response.render('administration/user', { currentTab: "users", redirectUrl: app.locals.BASE_URL + "administration/users" });
    },

    edit: function (request, response) {
      return DataManager.getUser({id: request.params.id}).then(function(userObj) {
        var user = {
          id: userObj.id,
          name: userObj.name,
          username: userObj.username,
          email: userObj.email,
          cellphone: userObj.cellphone,
          administrator: userObj.administrator
        };

        return response.render('administration/user', {
          user: user,
          update: true,
          currentTab: "users",
          redirectUrl: app.locals.BASE_URL + "administration/users"
        });
      }).catch(function(err) {
        response.status(400);
        response.json({status: 400, message: err.message});
      });
    },

    // api: json
    post: function (request, response) {
      if((request.body.password !== undefined && request.body.passwordConfirm !== undefined) && (request.body.password === request.body.passwordConfirm) && (request.body.password !== '')) {
        var userObj = {
          name: request.body.name,
          username: request.body.username,
          password: request.body.password,
          cellphone: request.body.cellphone,
          email: request.body.email,
          administrator: (request.body.administrator !== undefined && request.body.administrator === true)
        };
        return DataManager.addUser(userObj).then(function(user) {
          var token = Utils.generateToken(app, TokenCode.SAVE, user.name);
          response.json({status: 200, result: user, token: token});
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        });
      } else {
        Utils.handleRequestError(response, new UserError("Incorrect password"), 400);
      }
    },

    // api: json
    put: function (request, response) {
      if(request.body.password === request.body.passwordConfirm) {
        return DataManager.getUser({id: request.params.id}).then(function(userObj) {
          var userToUpdate = {
            name: request.body.name,
            username: request.body.username,
            password: request.body.password,
            cellphone: request.body.cellphone,
            email: request.body.email,
            administrator: request.body.administrator
          };

          return DataManager.updateUser({id: request.params.id}, userToUpdate).then(function(user) {
            var token = Utils.generateToken(app, TokenCode.UPDATE, userToUpdate.name);
            response.json({status: 200, result: user, token: token});
          }).catch(function(err) {
            response.status(400);
            response.json({status: 400, message: err.message});
          });
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        });
      } else {
        Utils.handleRequestError(response, new UserError("Incorrect password"), 400);
      }
    },

    // api: json
    delete: function(request, response) {
      var userId = request.params.id;
      if (userId) {
        if (request.user.id == userId) {
          Utils.handleRequestError(response, new UserError("You cannot remove yourself"), 400);
          return;
        }
        return DataManager.getUser({id: userId}).then(function(userResult) {
          var name = userResult.name;
          return DataManager.removeUser({id: userId}).then(function() {
            response.json({status:200, name: name});
          }).catch(function(err) {
            logger.debug("Remove user: ", err);
            Utils.handleRequestError(response, new UserError("Could not remove user ", err), 400);
          });
        }).catch(function(err) {
          Utils.handleRequestError(response, new UserError("User not found", err), 400);
        });
      } else {
        Utils.handleRequestError(response, new UserError("Missing user id"), 400);
      }
    }
  };
};
