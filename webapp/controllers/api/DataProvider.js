var DataManager = require("../../core/DataManager");
var UriBuilder = require("../../core/UriBuilder");
var RequestFactory = require("../../core/RequestFactory");

module.exports = function(app) {
  return {
    "post": function(request, response) {
      var dataProviderReceived = request.body;
      var uri = UriBuilder.buildUri(dataProviderReceived);

      var requester = RequestFactory.build(dataProviderReceived);

      var handleError = function(response, err, code) {
        response.status(code);
        response.json({status: code || 400, message: err.message});
      };

      // check connection
      requester.request().then(function() {
        var projectName = dataProviderReceived.project;

        // check project
        DataManager.getProject({name: projectName}).then(function(project) {
          var dataProviderObject = {
            name: dataProviderReceived.name,
            uri: uri,
            description: dataProviderReceived.description,
            data_provider_intent_name: "Intent1",
            data_provider_type_name: dataProviderReceived.kind,
            project_id: project.id, // todo: its temp code.get it from frontend
            active: dataProviderReceived.active || false
          };

          // try to save
          DataManager.addDataProvider(dataProviderObject).then(function(result) {
            response.json(result);
          }).catch(function(err) {
            handleError(response, err, 400);
          });
        }).catch(function(err) {
          handleError(response, err, 400);
        })
      }).catch(function(err) {
        handleError(response, err, 400);
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