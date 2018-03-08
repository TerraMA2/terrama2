"use strict";

var logger = require("./../../core/Logger");
var DataManager = require("../../core/DataManager");
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;
var UriBuilder = require('../../core/UriBuilder');
var RequestFactory = require("../../core/RequestFactory");
var fs = require('fs');
var path = require("path");
var Application = require("./../../core/Application");

module.exports = function(app) {
  return {
    get: function(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;
      parameters.hasProjectPermission = hasProjectPermission;

      return response.render("configuration/providers", parameters);
    },

    new: function(request, response) {
      var redirectTo = request.query.redirectTo ? request.query : {redirectTo: app.locals.BASE_URL + "configuration/providers"};
      // Creating default PostGIS and File providers
      var configFile = Application.getContextConfig();

      return response.render("configuration/provider", {
        isEditing: false,
        dataProvider: {},
        saveConfig: {
          url: app.locals.BASE_URL + "api/DataProvider",
          method: "POST"
        },
        redirectTo: redirectTo,
        defaultFilePath: configFile.defaultFilePath
      });
    },

    edit: function(request, response) {
      var dataProviderId = request.params.id;
      var redirectTo = request.query.redirectTo ? request.query : {redirectTo: app.locals.BASE_URL + "configuration/providers"};

      // Creating default PostGIS and File providers
      var configFile = Application.getContextConfig();
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;

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
            url: app.locals.BASE_URL + "api/DataProvider/" + dataProvider.id,
            method: "PUT"
          },
          fields: requester.constructor.fields(),
          redirectTo: redirectTo,
          defaultFilePath: configFile.defaultFilePath,
          hasProjectPermission: hasProjectPermission
        });
      }).catch(function(err) {
        logger.debug(err);
        response.render("base/404");
      });
    }
  };
};
