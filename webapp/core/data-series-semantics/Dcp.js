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
  if (!args.name)
    throw new DataSeriesSemanticsError("Dcp name is required.");

  var object = {};
  object[DataSeriesSemantics.NAME] = args.name;
  object[DataSeriesSemantics.FORMAT] = "DCP";
  object[DataSeriesSemantics.TYPE] = DataSeriesType.DCP;

  AbstractClass.apply(this, [object]);
};

Dcp.prototype = Object.create(AbstractClass.prototype);
Dcp.prototype.constructor = Dcp;

Dcp.schema = function() {
  var parentValues = AbstractClass.schema.call(this);

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
        title: "Projection"
    }
  };

  return {
    type: Form.Type.OBJECT,
    properties: Object.assign(properties, parentValues),

    required: ['latitude', 'longitude', 'projection']
  };
};

Dcp.form = function() {
  return [
    {
      key: "latitude",
      htmlClass: "col-md-3"
    },
    {
      key: "longitude",
      htmlClass: "col-md-3"
    },
    {
      key: "projection",
      htmlClass: "col-md-3"
    }
  ].concat(AbstractClass.form.call(this));
};

module.exports = Dcp;