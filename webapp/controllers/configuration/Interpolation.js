module.exports = function(app) {
  'use strict';

  var ScheduleType = require("./../../core/Enums").ScheduleType;

  return {
    new: function(request, response){
      response.render("configuration/interpolation");
    },
    edit: function(request, response){
      var hasProjectPermission = app.locals.activeProject.hasProjectPermission;
      DataManager.getInterpolation({id: parseInt(request.params.id)})
        .then(function(interpolation) {
          return response.render("configuration/interpolation", {interpolation: interpolation.rawObject(), ScheduleType: ScheduleType, hasProjectPermission: hasProjectPermission });
        }).catch(function(err) {
          return response.render("base/404");
        });
    }
  }
}