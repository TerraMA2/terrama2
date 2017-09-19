module.exports = function(app) {
  'use strict';

  var ScheduleType = require("./../../core/Enums").ScheduleType;

  return {
    new: function(request, response){
      response.render("configuration/interpolator");
    },
    edit: function(request, response){
      var hasProjectPermission = app.locals.activeProject.hasProjectPermission;
      DataManager.getInterpolator({id: parseInt(request.params.id)})
        .then(function(interpolator) {
          return response.render("configuration/interpolator", {interpolator: interpolator.rawObject(), ScheduleType: ScheduleType, hasProjectPermission: hasProjectPermission });
        }).catch(function(err) {
          return response.render("base/404");
        });
    }
  }
}