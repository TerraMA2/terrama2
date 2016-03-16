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
      var name = request.query.name;

      if (name) {
        DataManager.getProject({name: name}).then(function(project) {
          response.json(project);
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        })
      } else {
        response.json(DataManager.listProjects());
      }
    }

  };
};