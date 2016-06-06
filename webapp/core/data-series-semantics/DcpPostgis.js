var Dcp = require('./Dcp');
var Enums = require('./../Enums');
var Form = Enums.Form;

var DcpPostgis = function(args) {
  Dcp.call(this, args);
};

DcpPostgis.prototype = Object.create(Dcp.prototype);
DcpPostgis.prototype.constructor = DcpPostgis;

DcpPostgis.identifier = function() {
  return "DCP-postgis";
};

DcpPostgis.prototype.schema = function() {
  var dcpSchema = {
    type: "object",
    properties: {},
    required: {}
  };
  dcpSchema.properties.table_name = {
    type: Form.Field.TEXT,
    title: "Table Name"
  };

  dcpSchema.required = ['table_name'];

  return dcpSchema;
};

DcpPostgis.prototype.form = function() {
  return [
    {
      key: 'table_name',
      htmlClass: "col-md-3"
    }
  ];
};

module.exports = DcpPostgis;
