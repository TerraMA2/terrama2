module.exports = function(app) {
  'use strict';

  var DataManager = require("./../../core/DataManager");
  var UriBuilder = require("./../../core/UriBuilder");
  var ScheduleType = require("./../../core/Enums").ScheduleType;
  var ViewSourceType = require("./../../core/Enums").ViewSourceType;
  var makeTokenParameters = require('../../core/Utils').makeTokenParameters;
  const ViewFacade = require('../../core/facade/View');

  return {
    get: function(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;
      parameters.hasProjectPermission = hasProjectPermission;
      response.render("configuration/views", parameters);
    },
    new: function(request, response) {
      return response.render("configuration/view", {ScheduleType, ViewSourceType});
    },
    edit: function(request, response) {
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;
      ViewFacade.retrieve(parseInt(request.params.id))
        .then(function(view) {
          return response.render("configuration/view", {view, ScheduleType, ViewSourceType, hasProjectPermission});
        }).catch(function(err) {
          return response.render("base/404");
        });
    }
  };
};