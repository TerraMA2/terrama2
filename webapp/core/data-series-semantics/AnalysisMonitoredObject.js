var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var AnalysisMonitoredObject = module.exports = function() {
  AbstractClass.apply(this, arguments);
};

AnalysisMonitoredObject.identifier = function() {
  return "ANALYSIS_MONITORED_OBJECT-postgis";
};

AnalysisMonitoredObject.prototype = Object.create(AbstractClass.prototype);
AnalysisMonitoredObject.prototype.constructor = AnalysisMonitoredObject;

AnalysisMonitoredObject.schema = function() {
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

AnalysisMonitoredObject.form = function() {
  return ['*'];
};

AnalysisMonitoredObject.demand = function () {
  return ["POSTGIS"];
};
