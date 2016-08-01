"use strict";

var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var GridGeoTiff = module.exports = function(args) {
  AbstractClass.apply(this, arguments);
};

GridGeoTiff.identifier = function() {
  return "GRID-geotiff";
};

GridGeoTiff.prototype = Object.create(AbstractClass.prototype);
GridGeoTiff.prototype.constructor = GridGeoTiff;

GridGeoTiff.prototype.schema = function() {
  return {
    properties: {
      mask: {
        type: Form.Field.TEXT,
        title: "Mask"
      }
    },
    required: ['mask']
  };
};

GridGeoTiff.prototype.form = function() {
  return ['*'];
};
