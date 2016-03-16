var DataManager = require("../../core/DataManager.js");
//var Url = require('url');
var Util = require('util');

module.exports = function(app) {
  return {
    "post": function(request, response) {
      var dataProviderReceived = request.body;

      var uri = Util.format("%s://%s:%s@%s:%s%s", dataProviderReceived.kind.toLowerCase(),
                                                  dataProviderReceived.user,
                                                  dataProviderReceived.password,
                                                  dataProviderReceived.address,
                                                  dataProviderReceived.port,
                                                  dataProviderReceived.path);

      var project = {
        version: 1,
        name: "Project " + dataProviderReceived.name,
        description: "project aa"

      };

      DataManager.addProject(project).then(function(projectOutput) {
        var dataProviderObject = {
          name: dataProviderReceived.name,
          uri: uri,
          description: dataProviderReceived.description,
          data_provider_intent_name: "Intent1",
          data_provider_type_name: dataProviderReceived.kind,
          project_id: projectOutput.id, // todo: its temp code.get it from frontend
          active: dataProviderReceived.active || false
        };


        DataManager.addDataProvider(dataProviderObject).then(function(result) {
          response.json(result);
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        });
      });

    },

    "get": function(request, response) {
      var name = request.query.name;

      if (name) {
        DataManager.getDataProvider({name: name}).then(function(dataProvider) {
          response.json(dataProvider);
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        })
      } else {
        response.json(DataManager.listDataProviders());
      }
    }

  };
};