var DataManager = require("../../core/DataManager");
var Utils = require('../../helpers/Utils');
var UriBuilder = require('../../core/UriBuilder');
var RequestFactory = require("../../core/RequestFactory");


module.exports = function(app) {
  return {
    "get": function(request, response) {
      var dataProviderName = request.params.name;

      if (!dataProviderName) {
        Utils.UrlHandler(request, response, "DataProvider", 'configuration/provider', {
          isEditing: false,
          dataProvider: {},
          saveConfig: {
            url: "/api/DataProvider",
            method: "POST"
          }
        });

        return;
      }

      DataManager.getDataProvider({name: dataProviderName}).then(function(dataProvider) {
        var requester = RequestFactory.buildFromUri(dataProvider.uri);

        Utils.UrlHandler(request, response, "DataProvider", 'configuration/provider', {
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