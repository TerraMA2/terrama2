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
  dcpSchema.properties.mask = {
    type: Form.Field.TEXT,
    title: "Mask"
  };

  dcpSchema.properties.timezone = {
    type: Form.Field.TEXT,
    title: "Timezone"
  };

  dcpSchema.required = dcpSchema.required.concat(['mask', 'timezone']);

  return dcpSchema;
};

DcpPostgis.form = function() {
  return [
    {
      key: 'mask',
      htmlClass: "col-md-6"
    },
    {
      key: 'timezone',
      type: 'select',
      htmlClass: "col-md-6",
      titleMap: [
        {
          value: '00:00',
          name: '00:00'
        },
        {
          value: '12:00',
          name: '12:00'
        }
      ]
    }
  ].concat(Dcp.form.call(this));
};

module.exports = DcpPostgis;