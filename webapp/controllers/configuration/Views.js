module.exports = function(app) {
  'use strict';

  var DataManager = require("./../../core/DataManager");
  var UriBuilder = require("./../../core/UriBuilder");
  var UriSyntax = require("./../../core/Enums").Uri;
  var makeTokenParameters = require('../../core/Utils').makeTokenParameters;

  return {
    get: function(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      response.render("configuration/views", parameters);
    },
    new: function(request, response) {
      return response.render("configuration/view");
    },
    edit: function(request, response) {
      DataManager.getView({id: parseInt(request.params.id)})
        .then(function(view) {
          var output = Object.assign(
            {serverUriObject: UriBuilder.buildObject(view.serverUri, UriSyntax)},
            view.rawObject()
          );
          return response.render("configuration/view", {view: output});
        }).catch(function(err) {
          console.log(err);
          return response.render("base/404");
        });
    }
  };
};