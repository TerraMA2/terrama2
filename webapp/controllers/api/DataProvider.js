var DataManager = require("../../core/DataManager");
var RequestFactory = require("../../core/RequestFactory");

module.exports = function(app) {
  return {
    post: function(request, response) {
      var dataProviderReceived = request.body;

      var uriObject = dataProviderReceived.uriObject;

      var requester = RequestFactory.build(uriObject);

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
            uri: requester.uri,
            description: dataProviderReceived.description,
            data_provider_intent_name: dataProviderReceived.data_provider_intent_name || requester.intent(),
            data_provider_type_name: uriObject[requester.syntax().SCHEME],
            project_id: project.id,
            active: dataProviderReceived.active || false
          };

          // try to save
          DataManager.addDataProvider(dataProviderObject).then(function(result) {
            response.json(result.toObject());
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

    get: function(request, response) {
      var name = request.query.name;

      if (name) {
        DataManager.getDataProvider({name: name}).then(function(dataProvider) {
          response.json(dataProvider.toObject());
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        })
      } else {
        var output = [];
        DataManager.listDataProviders().forEach(function(element) {
          output.push(element.toObject());
        });
        response.json(output);
      }
    },

    put: function(request, response) {
      var dataProviderName = request.params.name;

      if (dataProviderName) {
        DataManager.updateDataProvider({name: dataProviderName, active: request.body.active}).then(function() {
          response.json({status: 200});
        }).catch(function(err) {
          response.status(400);
          response.json({status: 400, message: err.message});
        })

      } else {
        response.status(400);
        response.json({status: 400, message: "DataProvider name not identified"});
      }
    }

  };
};