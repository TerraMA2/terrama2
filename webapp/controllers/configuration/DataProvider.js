"use strict";

var logger = require("./../../core/Logger");
var DataManager = require("../../core/DataManager");
var Utils = require('../../helpers/Utils');
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;
var UriBuilder = require('../../core/UriBuilder');
var RequestFactory = require("../../core/RequestFactory");


module.exports = function(app) {
  return {
    get: function(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);

      return response.render("configuration/providers", parameters);
    },

    new: function(request, response) {
      var redirectTo = request.query.redirectTo ? request.query : {redirectTo: "/configuration/providers"};

      return response.render("configuration/provider", {
        isEditing: false,
        dataProvider: {},
        saveConfig: {
          url: "/api/DataProvider",
          method: "POST"
        },
        redirectTo: redirectTo
      });
    },

    edit: function(request, response) {
      var dataProviderId = request.params.id;
      var redirectTo = request.query.redirectTo ? request.query : {redirectTo: "/configuration/providers"};

      DataManager.getDataProvider({id: parseInt(dataProviderId || "0")}).then(function(dataProvider) {
        var requester = RequestFactory.buildFromUri(dataProvider.uri);

        return response.render('configuration/provider', {
          isEditing: true,
          dataProvider: {
            name: dataProvider.name,
            description: dataProvider.description,
            active: dataProvider.active,
            timeout: dataProvider.timeout ? parseInt(dataProvider.timeout) : dataProvider.timeout,
            active_mode: dataProvider.active_mode,
            data_provider_type_name: dataProvider.data_provider_type.name,
            uriObject: requester.params
          },
          saveConfig: {
            url: "/api/DataProvider/" + dataProvider.id,
            method: "PUT"
          },
          fields: requester.constructor.fields(),
          redirectTo: redirectTo
        });
      }).catch(function(err) {
        logger.debug(err);
        response.render("base/404");
      });
    }
  };
};
