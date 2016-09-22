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
      },
      timezone: {
        type: Form.Field.TEXT,
        title: "Timezone"
      }
    },
    required: ['mask', 'timezone']
  };
};

GridGeoTiff.prototype.form = function() {
  return [
    {
      key: 'mask',
      htmlClass: "col-md-6 terrama2-schema-form"
    },
    {
      key: 'timezone',
      htmlClass: "col-md-6 terrama2-schema-form",
      type: "select",
      titleMap: Utils.getTimezonesGUI()
    }
  ];
};
