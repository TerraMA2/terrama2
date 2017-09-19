(function(){
  "use strict";

  var Interpolation = module.exports = {};

  var PromiseClass = require("./../Promise");

  Interpolation.save = function(interpolationObject, projectId){
    return new PromiseClass(function(resolve, reject){
      return resolve("salvo");
    });
  };

  Interpolation.list = function(restriction){
    return new PromiseClass(function(resolve, reject){
      return resolve(["int1", "int2"]);
    });
  };

  Interpolation.update = function(interpolationId, interpolationObject){
    return new PromiseClass(function(resolve, reject){
      return resolve("modificado");
    });
  };

  Interpolation.delete = function(){
    return new PromiseClass(function(resolve, reject){
      return resolve("deletado");
    });
  };
} ());