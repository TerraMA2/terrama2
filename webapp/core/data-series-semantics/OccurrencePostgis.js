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
      table_name: {
        type: Form.Field.TEXT,
        title: "Table Name"
      }
    },
    required: ['table_name']
  }
};

OccurrencePostgis.form = function() {
  return ['*'];
};

OccurrencePostgis.demand = function () {
  return ["POSTGIS"];
};
