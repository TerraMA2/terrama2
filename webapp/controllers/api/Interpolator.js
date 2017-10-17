(function(){
  'use strict';

  //Facade
  var InterpolatorFacade = require("./../../core/facade/Interpolator");
  var Utils = require('./../../core/Utils');
  var TokenCode = require('./../../core/Enums').TokenCode;

  module.exports = function(app){
    return {
      get: function(request, response) {
        var project_id = request.query.project_id;
        InterpolatorFacade.retrieve(null, project_id).then(function(interpolator){
          return response.json(interpolator);
        }).catch(function(err) {
          Utils.handleRequestError(response, err, 400);
        });
      },
      post: function(request, response) {
        var interpolatorObject = request.body.interpolator;
        var scheduleObject = request.body.schedule;
        var shouldRun = request.body.interpolator.run;
        InterpolatorFacade.save(interpolatorObject, scheduleObject, request.session.activeProject.id)
          .then(function(interpolator){
            var extra = {};
            if (shouldRun){
              extra = {
                id: interpolator.dataSeriesOutput.id
              }
            }
            var token = Utils.generateToken(app, TokenCode.SAVE, interpolator.dataSeriesOutput.name, extra);
            return response.json({status: 200, result: interpolator, token: token});
          }).catch(function(err) {
            Utils.handleRequestError(response, err, 400);
          });
      },
      put: function(request, response) {
        var interpolatorId = parseInt(request.params.id);
        var interpolatorObject = request.body.interpolator;
        var scheduleObject = request.body.schedule;
        var shouldRun = request.body.interpolator.run;
        return InterpolatorFacade.update(interpolatorId, interpolatorObject, scheduleObject, request.session.activeProject.id)
          .then(function(interpolator){
            var extra = {};
            if (shouldRun){
              extra = {
                id: interpolator.dataSeriesOutput.id
              };
            }
            // generating token
            var token = Utils.generateToken(app, TokenCode.UPDATE, interpolator.dataSeriesOutput.name, extra);
            response.json({status: 200, result: interpolator, token: token});
          }).catch(function(err) {
            Utils.handleRequestError(response, err, 400);
          });
      }
    }
  }
} ());