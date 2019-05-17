"use strict";

var DataManager = require("./../../core/DataManager");
var ScheduleType = require("./../../core/Enums").ScheduleType;
var makeTokenParameters = require('../../core/Utils').makeTokenParameters;

module.exports = function(app) {
	return {
		get: function(request, response) {
			var parameters = makeTokenParameters(request.query.token, app);
			var hasProjectPermission = request.session.activeProject.hasProjectPermission;
			parameters.hasProjectPermission = hasProjectPermission;
			response.render("configuration/storages", parameters);
		},
		new: function(request, response) {
			return response.render("configuration/storages", {ScheduleType: ScheduleType});
		},
		edit: function(request, response) {
      var hasProjectPermission = request.session.activeProject.hasProjectPermission;
      DataManager.getStorage({id: parseInt(request.params.id)})
      .then(function(storage) {
          return response.render("configuration/storages", {storage: storage.rawObject(), ScheduleType: ScheduleType, hasProjectPermission: hasProjectPermission});
      }).catch(function(err) {
          return response.render("base/404");
      });
    }
  }
};
