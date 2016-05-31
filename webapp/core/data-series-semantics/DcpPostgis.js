var Dcp = require('./Dcp');
var Enums = require('./../Enums');
var Form = Enums.Form;

var DcpPostgis = function() {
  Dcp.call(this, {name: "DCP-postgis"});
};

DcpPostgis.prototype = Object.create(Dcp.prototype);
DcpPostgis.prototype.constructor = DcpPostgis;

DcpPostgis.identifier = function() {
  return "DCP-postgis";
};

DcpPostgis.schema = function() {
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

DcpPostgis.form = function() {
  return [
    {
      key: 'table_name',
      htmlClass: "col-md-3"
    }
  ];
};

DcpPostgis.demand = function () {
  return ["POSTGIS"];
};


module.exports = DcpPostgis;
