var User = require('../../config/Sequelize.js').import('../../models/User.js');

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
      });
    },

    new: function (request, response) {
      return response.render('administration/user', { method: 'POST', action: '/administration/users/new' });
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

          return response.render('administration/user', { method: 'PUT', action: '/administration/users/edit', user: user });
        } else {
          return response.render('administration/users');
        }
      });
    },

    post: function (request, response) {
      if(request.body.password === request.body.passwordConfirm) {
        var salt = User.generateSalt();

        var userObj = {
          name: request.body.name,
          username: request.body.username,
          password: User.generateHash(request.body.password, salt),
          salt: salt,
          cellphone: request.body.cellphone,
          email: request.body.email,
          administrator: (request.body.administrator !== undefined && request.body.administrator === 'on')
        };

        User.create(userObj).then(function() {
          return response.redirect("/administration/users");
        });
      } else {
        return response.redirect("back");
      }
    },

    put: function (request, response) {
      console.log(request.body);
      /*if(request.body.password === request.body.passwordConfirm) {
        var userObj = {
          name: request.body.name,
          username: request.body.username,
          cellphone: request.body.cellphone,
          email: request.body.email,
          administrator: (request.body.administrator !== undefined && request.body.administrator === 'on')
        };

        var fields = ["name", "username", "cellphone", "email", "administrator"];

        if(request.body.password !== '')
        password: User.generateHash(request.body.password, salt),

        User.update(userObj, {
          fields: ,
          where: {
            id: project.id
          }
        }).then(function() {
          return response.redirect("/administration/users");
        });
      } else {
        return response.redirect("back");
      }*/
    }
  };
};
