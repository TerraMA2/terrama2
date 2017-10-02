(function(){
  'use strict';

  //Facade
  var InterpolatorFacade = require("./../../core/facade/Interpolator");
  var Utils = require('./../../core/Utils');
  var TokenCode = require('./../../core/Enums').TokenCode;

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
        var shouldRun = request.body.run;
        InterpolatorFacade.save(interpolatorObject, scheduleObject, app.locals.activeProject.id)
          .then(function(interpolator){
            var extra = {};
            if (shouldRun){
              extra = {
                id: interpolator.id
              }
            }
            var token = Utils.generateToken(app, TokenCode.SAVE, "Interpolator", extra);
            return response.json({status: 200, result: interpolator, token: token});
          });
      },
      put: function(request, response) {
        var interpolatorId = parseInt(request.params.id);
        var interpolatorObject = request.body.interpolator;
        var scheduleObject = request.body.schedule;
        var shouldRun = request.body.run;
        return InterpolatorFacade.update(interpolatorId, interpolatorObject, scheduleObject, app.locals.activeProject.id)
          .then(function(interpolator){
            var extra = {};
            if (shouldRun){
              extra = {
                id: interpolatorId
              };
            }
            // generating token
            var token = Utils.generateToken(app, TokenCode.UPDATE, "Interpolator", extra);
            response.json({status: 200, result: interpolator, token: token});
          }).catch(function(err) {
            logger.error(Utils.format("%s %s", "Error while retrieving updated interpolator", err.toString()));
            Utils.handleRequestError(response, err, 400);
          });
      }
    }
  }
} ());