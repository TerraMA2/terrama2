var User = require('../config/Sequelize.js').import('../models/User.js');

module.exports = function(app) {
  function indexController(request, response) {
    User.findOne({
      where: {
        'username': 'admin'
      }
    }).then(function(userObj) {
      if(userObj === null) {
        var salt = User.generateSalt();

        User.create({
          name: "Administrator",
          username: "admin",
          password: User.generateHash("admin", salt),
          salt: salt,
          cellphone: '99999999999999',
          email: 'admin@admin',
          administrator: true
        }).then(function() {
          if(request.isAuthenticated() && request.user.dataValues.administrator) {
            return response.redirect("/configuration/projects");
          } else if(request.isAuthenticated()) {
            return response.redirect("/configuration/projects");
          } else {
            app.locals.activeProject = {};
            return response.redirect("/login");
          }
        }).catch(function(err) {
          console.log(err);
        });
      } else {
        if(request.isAuthenticated() && request.user.dataValues.administrator) {
          return response.redirect("/configuration/projects");
        } else if(request.isAuthenticated()) {
          return response.redirect("/configuration/projects");
        } else {
          app.locals.activeProject = {};
          return response.redirect("/login");
        }
      }
    });
  }

  return indexController;
};
