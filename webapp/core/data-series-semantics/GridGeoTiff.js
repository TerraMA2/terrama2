var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var GridGeoTiff = module.exports = function() {
  AbstractClass.apply(this, arguments);
};

GridGeoTiff.identifier = function() {
  return "GRID-geotiff";
};

GridGeoTiff.prototype = Object.create(AbstractClass.prototype);
GridGeoTiff.prototype.constructor = GridGeoTiff;

GridGeoTiff.schema = function() {
  return {
    properties: {
      mask: {
        type: Form.Field.TEXT,
        title: "Mask"
      }
    },
    required: ['mask']
  }
};

GridGeoTiff.form = function() {
  return ['*'];
};

GridGeoTiff.demand = function () {
  return ["FILE", "FTP"];
};
