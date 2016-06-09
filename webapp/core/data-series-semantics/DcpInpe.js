var Dcp = require('./Dcp');
var Enums = require('./../Enums');
var Form = Enums.Form;
var Utils = require('./../Utils');

/**
 * It handles DCP-INPE semantics type.
 * @constructor
 */
var DcpInpe = function(args) {
  Dcp.call(this, args);
};

DcpInpe.prototype = Object.create(Dcp.prototype);
DcpInpe.prototype.constructor = DcpInpe;

DcpInpe.identifier = function() {
  return "DCP-inpe";
};

DcpInpe.prototype.schema = function() {
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

DcpInpe.prototype.form = function() {
  return [
    {
      key: 'mask',
      htmlClass: "col-md-6"
    },
    {
      key: 'timezone',
      htmlClass: "col-md-6",
      type: 'select',
      titleMap: Utils.getTimezonesGUI()
    }
  ].concat(Dcp.form.call(this));
};

module.exports = DcpInpe;
