var AbstractClass = require('./../AbstractSemantics');
var Enums = require('./../Enums');
var DataSeriesSemanticsError = require('./../Exceptions').DataSeriesSemanticsError;
var DataSeriesSemantics = Enums.DataSeriesSemantics;
var DataSeriesType = Enums.DataSeriesType;
var Form = Enums.Form;

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
  return {
    type: Form.Type.OBJECT,
    properties: {
      latitude: {
        type: Form.Field.NUMBER,
        title: "Latitude",
        htmlClass: "col-md-6"
      },
      longitude: {
        type: Form.Field.NUMBER,
        title: "Longitude",
        htmlClass: "col-md-6"
      }
    },

    required: ['latitude', 'longitude']
  }
};

Dcp.form = function() {
  return [
    {
      key: "latitude",
      htmlClass: "col-md-6"
    },
    {
      key: "longitude",
      htmlClass: "col-md-6"
    }
  ];
};

module.exports = Dcp;