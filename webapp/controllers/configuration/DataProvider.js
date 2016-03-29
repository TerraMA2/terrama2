var DataManager = require("../../core/DataManager");
var Utils = require('../../helpers/Utils');
var UriBuilder = require('../../core/UriBuilder');
var RequestFactory = require("../../core/RequestFactory");


module.exports = function(app) {
  return {
    get: function(request, response) {
      var dataProviderId = request.query.id,
          method = request.query.method;

      DataManager.getDataProvider({id: dataProviderId}).then(function(dataProvider) {
        return response.render("configuration/providers", {dataProvider: dataProvider.name, method: method});
      }).catch(function(err) {
        return response.render("configuration/providers");
      });
    },

    new: function(request, response) {
      return response.render("configuration/provider", {
        isEditing: false,
        dataProvider: {},
        saveConfig: {
          url: "/api/DataProvider",
          method: "POST"
        }
      });
    },

    edit: function(request, response) {
      var dataProviderName = request.params.name;

      DataManager.getDataProvider({name: dataProviderName}).then(function(dataProvider) {
        var requester = RequestFactory.buildFromUri(dataProvider.uri);

        return response.render('configuration/provider', {
          isEditing: true,
          dataProvider: {
            name: dataProvider.name,
            description: dataProvider.description,
            active: dataProvider.active,
            data_provider_type_name: dataProvider.data_provider_type_name,
            uriObject: requester.params
          },
          saveConfig: {
            url: "/api/DataProvider/" + dataProvider.name,
            method: "PUT"
          },
          fields: requester.constructor.fields()
        });
      }).catch(function(err) {
        console.log(err);
        response.render("base/404");
      });
    }
  };
};