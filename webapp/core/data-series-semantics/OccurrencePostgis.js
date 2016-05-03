var Occurrence = require('./Occurrence');
var Form = require('./../Enums').Form;

var OccurrencePostgis = module.exports = function() {
  Occurrence.apply(this, arguments);
};

OccurrencePostgis.identifier = function() {
  return "OCCURRENCE-postgis";
};

OccurrencePostgis.prototype = Object.create(Occurrence.prototype);
OccurrencePostgis.prototype.constructor = OccurrencePostgis;

OccurrencePostgis.schema = function() {
  return {
    properties: {
      tableName: {
        type: Form.Field.TEXT,
        title: "Table Name"
      }//,
      // geometry: {
      //   type: Form.Field.TEXT,
      //   title: "Geometry Field"
      // }
    },
    required: ['tableName']
    // ,timeStamp: {
    //   type: Form.Field.TEXT,
    //   title: "TimeStamp column"
    // }
  }
};

OccurrencePostgis.form = function() {
  return ['*'];
};