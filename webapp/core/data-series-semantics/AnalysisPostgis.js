'use strict';

var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var AnalysisPostgis = module.exports = function() {
  AbstractClass.apply(this, arguments);
};

AnalysisPostgis.identifier = function() {
  return "ANALYSIS-postgis";
};

AnalysisPostgis.prototype = Object.create(AbstractClass.prototype);
AnalysisPostgis.prototype.constructor = AnalysisPostgis;

AnalysisPostgis.schema = function() {
  return {
    properties: {
      table_name: {
        type: Form.Field.TEXT,
        title: "Table Name"
      }
    },
    required: ['table_name']
  };
};

AnalysisPostgis.form = function() {
  return ['*'];
};

AnalysisPostgis.demand = function () {
  return ["POSTGIS"];
};
