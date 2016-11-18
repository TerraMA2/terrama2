'use strict';

var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;
var Utils = require("./../Utils");

/**
 * It defines a GeoTIFF semantics representation
 * 
 * @param {Object} args - An arguments to build a grid model. It may be a orm model instance or just a object 
 * @constructor
 */
var GridGeoTiff = module.exports = function(args) {
  AbstractClass.apply(this, arguments);
};

GridGeoTiff.identifier = function() {
  return "GRID-static_geotiff";
};

GridGeoTiff.prototype = Object.create(AbstractClass.prototype);
GridGeoTiff.prototype.constructor = GridGeoTiff;

GridGeoTiff.prototype.schema = function() {
  return {
    properties: Utils.extend(Utils.getFolderSchema(), {
      mask: {
        type: Form.Field.TEXT,
        title: "Mask"
      }
    }),
    required: ['mask']
  };
};

GridGeoTiff.prototype.form = function() {
  return [
    Utils.getFolderForm(),
    {
      key: 'mask',
      htmlClass: "col-md-6 terrama2-schema-form"
    }
  ];
};
