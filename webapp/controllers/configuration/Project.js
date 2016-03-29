var DataManager = require('../../core/DataManager');

module.exports = function(app) {
  return {
    get: function (request, response) {
      // Its temp code. It should get from adminstration
      var config =  {
        "username": "postgres",
        "password": "postgres",
        "database": "nodejs",
        "host": "127.0.0.1",
        "dialect": "postgres",
        define: {
          schema: "terrama2"
        }
        //logging: false
      };

      DataManager.setConfiguration(config);

      DataManager.init(function() {
        DataManager.load().then(function() {
          response.render("configuration/projects");
        });
      });
    },

    new: function (request, response) {
      response.render("configuration/project", {method: "POST", url: "/api/Project"});
    },

    edit: function(request, response) {
      var projectName = request.params.name;

      DataManager.getProject({name: projectName}).then(function(project) {
        response.render("configuration/project", {project: project, method: "PUT", url: "/api/Project/"+project.id});
      }).catch(function(err) {
        response.render("base/404");
      });
    },

    activate: function(request, response) {
      var projectName = request.params.name;

      DataManager.getProject({name: projectName}).then(function(project) {
        app.locals.activeProject = {name: project.name};

        // Redirect for start application
        response.redirect("/configuration/providers");
      }).catch(function(err) {
        console.log(err);
        response.render('base/404');
      })
    }
  };
};