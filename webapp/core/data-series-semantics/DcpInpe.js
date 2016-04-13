var Dcp = require('./Dcp');
var Enums = require('./../Enums');
var Form = Enums.Form;

/**
 * It handles DCP-INPE semantics type.
 * @constructor
 */
var DcpInpe = function() {
  Dcp.call(this, {name: "DCP-INPE"});
};

DcpInpe.prototype = Object.create(Dcp.prototype);
DcpInpe.prototype.constructor = DcpInpe;

DcpInpe.identifier = function() {
  return "DCP-INPE";
};

DcpInpe.schema = function() {
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

DcpInpe.form = function() {
  return [
    {
      key: 'mask',
      htmlClass: "col-md-6"
    },
    {
      key: 'timezone',
      htmlClass: "col-md-6",
      type: 'select',
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

module.exports = DcpInpe;