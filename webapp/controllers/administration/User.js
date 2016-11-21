var logger = require("./../../core/Logger");
var User = require('../../config/Database').getORM().import('../../models/User.js');
var Utils = require("../../core/Utils");
var TokenCode = require('../../core/Enums').TokenCode;
var UserError = require("../../core/Exceptions").UserError;

module.exports = function(app) {
  return {
    get: function (request, response) {
      User.findAll({
        attributes: ['id', 'name', 'username', 'cellphone', 'email', 'administrator']
      }).then(function(users) {
        var usersArray = [];

        users.forEach(function(user) {
          var userObj = user.get();

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
      return response.render('administration/user', { currentTab: "users", redirectUrl: "/administration/users" });
    },

    edit: function (request, response) {
      User.findOne({
        where: {
          id: request.params.id
        }
      }).then(function(userObj) {
        if(userObj !== null) {
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
            redirectUrl: "/administration/users"
          });
        } else {
          Utils.handleRequestError(response, new UserError("Invalid user"), 400);
        }
      }).catch(function(err) {
        response.status(400);
        response.json({status: 400, message: err.message});
      });
    },

    // api: json
    post: function (request, response) {
      if((request.body.password !== undefined && request.body.passwordConfirm !== undefined) && (request.body.password === request.body.passwordConfirm) && (request.body.password !== '')) {
        var salt = User.generateSalt();

        var userObj = {
          name: request.body.name,
          username: request.body.username,
          password: User.generateHash(request.body.password, salt),
          salt: salt,
          cellphone: request.body.cellphone,
          email: request.body.email,
          administrator: (request.body.administrator !== undefined && request.body.administrator === true)
        };

        User.create(userObj).then(function(user) {
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
        User.findOne({
          where: {
            id: request.params.id
          }
        }).then(function(userObj) {
          if(userObj !== null) {
            var userToUpdate = {
              name: request.body.name,
              username: request.body.username,
              cellphone: request.body.cellphone,
              email: request.body.email,
              administrator: request.body.administrator
            };

            var fields = ["name", "username", "cellphone", "email", "administrator"];

            if(request.body.password !== undefined && request.body.password !== '') {
              userToUpdate['password'] = User.generateHash(request.body.password, userObj.salt);
              fields.push("password");
            }

            User.update(userToUpdate, {
              fields: fields,
              where: {
                id: request.params.id
              }
            }).then(function(user) {
              var token = Utils.generateToken(app, TokenCode.UPDATE, userToUpdate.name);
              response.json({status: 200, result: user, token: token});
            }).catch(function(err) {
              response.status(400);
              response.json({status: 400, message: err.message});
            });
          } else {
            Utils.handleRequestError(response, new UserError("Invalid user"), 400);
          }
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
        if (request.user.dataValues.id == userId) {
          Utils.handleRequestError(response, new UserError("You cannot remove yourself"), 400);
          return;
        }
        User.findOne({where: {id: userId}}).then(function(userResult) {
          var name = userResult.name;
          User.destroy({where: {id: userId}}).then(function() {
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
