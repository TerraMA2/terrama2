var Dcp = require('./Dcp');
var Enums = require('./../Enums');
var Form = Enums.Form;

var DcpPostgis = function() {
  Dcp.call(this, {name: "DCP-POSTGIS"});
};

DcpPostgis.prototype = Object.create(Dcp.prototype);
DcpPostgis.prototype.constructor = DcpPostgis;

DcpPostgis.identifier = function() {
  return "DCP-POSTGIS";
};

DcpPostgis.schema = function() {
  var dcpSchema = Dcp.schema.call(this);
  dcpSchema.properties.tableName = {
    type: Form.Field.TEXT,
    title: "Table Name"
  };

  dcpSchema.required = dcpSchema.required.concat(['tableName']);

  return dcpSchema;
};

DcpPostgis.form = function() {
  return [
    {
      key: 'tableName',
      htmlClass: "col-md-5"
    }
  ].concat(Dcp.form.call(this));
};

module.exports = DcpPostgis;