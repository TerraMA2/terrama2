var AbstractClass = require('./../AbstractSemantics');
var Enums = require('./../Enums');
var DataSeriesSemanticsError = require('./../Exceptions').DataSeriesSemanticsError;
var DataSeriesSemantics = Enums.DataSeriesSemantics;
var DataSeriesType = Enums.DataSeriesType;
var Form = Enums.Form;


/**
 * Base class for handling DCP semantics type.
 * @constructor
 * @param {Object} args - It defines an object for semantics representation.
 */
var Dcp = function(args) {
  AbstractClass.apply(this, [args]);
};

Dcp.prototype = Object.create(AbstractClass.prototype);
Dcp.prototype.constructor = Dcp;

Dcp.schema = function() {
  var parentValues = AbstractClass.prototype.schema.call(this);

  var properties = {
    latitude: {
      type: Form.Field.NUMBER,
        title: "Latitude"
    },
    longitude: {
      type: Form.Field.NUMBER,
        title: "Longitude"
    },
    projection: {
      type: Form.Field.TEXT,
        title: "SRID"
    }
  };

  return {
    type: Form.Type.OBJECT,
    properties: Object.assign(properties, parentValues),

    required: ['latitude', 'longitude', 'projection']
  };
};

Dcp.form = function() {
  var baseFields = AbstractClass.prototype.form.call(this);
  return [
    {
      key: "latitude",
      htmlClass: "col-md-2"
    },
    {
      key: "longitude",
      htmlClass: "col-md-2"
    },
    {
      key: "projection",
      htmlClass: "col-md-2"
    }
  ];
};

module.exports = Dcp;
