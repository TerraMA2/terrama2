var DataManager = require("../../core/DataManager");
var Utils = require('../../helpers/Utils');
var UriBuilder = require('../../core/UriBuilder');


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
        var uriObject = UriBuilder.buildObject(dataProvider.uri);
        dataProvider.uri = uriObject;
        Utils.UrlHandler(request, response, "DataProvider", 'configuration/provider', {
          isEditing: true,
          dataProvider: Object.assign({
            name: dataProvider.name,
            description: dataProvider.description,
            active: dataProvider.active
          }, uriObject),
          saveConfig: {
            url: "/api/DataProvider/" + dataProvider.name,
            method: "PUT"
          }
        });
      }).catch(function(err) {
        console.log(err);
        response.send(err);
      });
    }
  };
};