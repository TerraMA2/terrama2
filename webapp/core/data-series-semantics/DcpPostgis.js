var Dcp = require('./Dcp');
var Enums = require('./../Enums');
var Form = Enums.Form;

var DcpPostgis = function() {
  Dcp.call(this, {name: "DCP-POSTGIS"});
};

DcpPostgis.prototype = Object.create(Dcp.prototype);
DcpPostgis.prototype.constructor = DcpPostgis;

DcpPostgis.identifier = function() {
  return "DCP-postgis";
};

DcpPostgis.schema = function() {
  // var dcpSchema = Dcp.schema.call(this);
  var dcpSchema = {
    type: "object",
    properties: {},
    required: {}
  };
  dcpSchema.properties.tableName = {
    type: Form.Field.TEXT,
    title: "Table Name"
  };
  //
  // dcpSchema.properties.geometryColumn = {
  //   type: Form.Field.TEXT,
  //   title: "Geometry Column"
  // };

  dcpSchema.required = ['tableName'];//, 'geometryColumn'];

  return dcpSchema;
};

DcpPostgis.form = function() {
  return [
    {
      key: 'tableName',
      htmlClass: "col-md-3"
    }//,
    // {
    //   key: 'geometryColumn',
    //   htmlClass: "col-md-2"
    // }
  ];
};

module.exports = DcpPostgis;