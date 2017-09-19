(function(){
  'use strict';

  //Facade
  var InterpolationFacade = require("./../../core/facade/Interpolation");

  module.exports = function(app){
    return {
      get: function(request, response) {
        InterpolationFacade.list({}).then(function(interpolator){
          return response.json(interpolator);
        });
      },
      post: function(request, response) {

      },
      put: function(request, response) {

      },
      delete: function(request, response){

      }
    }
  }
} ());