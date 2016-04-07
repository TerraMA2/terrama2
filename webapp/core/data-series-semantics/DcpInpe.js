var Dcp = require('./Dcp');
var Enums = require('./../Enums');
var Form = Enums.Form;

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
  
  dcpSchema.required.concat(['mask', 'timezone']);

  return dcpSchema;
};

DcpInpe.form = function() {
  return [
    'mask',
    'timezone'
  ].concat(Dcp.form.call(this));
};

module.exports = DcpInpe;