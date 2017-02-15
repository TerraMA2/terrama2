module.exports = function(app) {
  'use strict';

  var DataManager = require("./../../core/DataManager");
  var UriBuilder = require("./../../core/UriBuilder");
  var ScheduleType = require("./../../core/Enums").ScheduleType;
  var makeTokenParameters = require('../../core/Utils').makeTokenParameters;

  return {
    get: function(request, response) {
      var parameters = makeTokenParameters(request.query.token, app);
      response.render("configuration/views", parameters);
    },
    new: function(request, response) {
      return response.render("configuration/view", {ScheduleType: ScheduleType});
    },
    edit: function(request, response) {
      DataManager.getView({id: parseInt(request.params.id)})
        .then(function(view) {
          return response.render("configuration/view", {view: view.rawObject(), ScheduleType: ScheduleType});
        }).catch(function(err) {
          return response.render("base/404");
        });
    }
  };
};