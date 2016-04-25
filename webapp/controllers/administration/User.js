var User = require('../../config/Sequelize.js').import('../../models/User.js');
var Utils = require("../../core/Utils");
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

          if(userObj.administrator)
            userObj.administrator = "Sim";
          else
            userObj.administrator = "Não";

          usersArray.push(userObj);
        });

        return response.render('administration/users', { usersList: usersArray });
      }).catch(function(err) {
        response.status(400);
        response.json({status: 400, message: err.message});
      });
    },

    new: function (request, response) {
      return response.render('administration/user', { method: 'POST', url: '/administration/users/new' });
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

          return response.render('administration/user', { user: user, method: 'PUT', url: '/administration/users/edit/' + userObj.id });
        } else {
          Utils.handleRequestError(response, new UserError("Invalid user"), 400);
        }
      }).catch(function(err) {
        response.status(400);
        response.json({status: 400, message: err.message});
      });
    },

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
          response.json(user);
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        });
      } else {
        Utils.handleRequestError(response, new UserError("Incorrect password"), 400);
      }
    },

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
              administrator: (request.body.administrator !== undefined && request.body.administrator === 'on')
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
              response.json(user);
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
    }
  };
};
