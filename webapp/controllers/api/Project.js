var DataManager = require("../../core/DataManager.js");


module.exports = function(app) {
  return {
    "post": function(request, response) {
      var projectObject = request.body;

      DataManager.addProject(projectObject).then(function(project) {
        response.json(project);
      }).catch(function(err) {
        response.status(400);
        response.json({status: 400, message: err.message});
      });
    },

    "get": function(request, response) {
      var id = request.params.id;

      if (id) {
        DataManager.getProject({id: id}).then(function(project) {
          console.log(project);
          response.json(project);
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        })
      } else {
        response.json(DataManager.listProjects());
      }
    },

    "put": function(request, response) {
      var id = request.params.id;

      if (id) {
        var projectGiven = request.body;
        projectGiven.id = id;
        DataManager.updateProject(projectGiven).then(function(project) {
          console.log('updated');
          response.json(project);
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        })
      } else {
        response.status(400);
        response.json({status: 400, message: "Project name not identified"});
      }
    }

  };
};