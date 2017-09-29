(function(){
  'use strict';

  //Facade
  var InterpolatorFacade = require("./../../core/facade/Interpolator");

  module.exports = function(app){
    return {
      get: function(request, response) {
        InterpolatorFacade.list({}).then(function(interpolator){
          return response.json(interpolator);
        });
      },
      post: function(request, response) {
        var interpolatorObject = request.body.interpolator;
        var scheduleObject = request.body.schedule;
        InterpolatorFacade.save(interpolatorObject, scheduleObject, app.locals.activeProject.id)
          .then(function(interpolator){
            return response.json({status: 200, interpolator: interpolator});
          });
      },
      put: function(request, response) {
        InterpolatorFacade.update({}).then(function(){
          return response.json(true);
        });
      }
    }
  }
} ());