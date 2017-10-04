module.exports = function(app) {
  'use strict';

  var DataManager = require("./../../core/DataManager");
  var ScheduleType = require("./../../core/Enums").ScheduleType;

  return {
    new: function(request, response){
      var dataSeriesIdToInterpolate = request.params.input_ds;
      DataManager.getDataSeries({id: parseInt(dataSeriesIdToInterpolate)})
        .then(function(input_ds){
          return response.render("configuration/interpolator", {input_ds: input_ds.rawObject()});
        })
        .catch(function(err){
          return response.render("base/404");          
        });
    },
    edit: function(request, response){
      DataManager.getInterpolator({id: parseInt(request.params.id)})
        .then(function(interpolator) {
          return response.render("configuration/interpolator", {interpolator: interpolator.rawObject(), ScheduleType: ScheduleType });
        }).catch(function(err) {
          return response.render("base/404");
        });
    }
  }
}