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
  return {
    type: Form.Type.OBJECT,
    properties: {
      mask: {
        type: Form.FormField.TEXT,
        title: "Mask"
      },
      timezone: {
        type: Form.FormField.TEXT,
        title: "Timezone"
      },
      path: {
        type: Form.FormField.TEXT,
        title: "Path"
      }
    },

    required: ['mask', 'timezone', 'path']
  }
};

DcpPostgis.form = function() {
  return [
    'mask',
    'timezone',
    'path'
  ];
};

module.exports = DcpPostgis;