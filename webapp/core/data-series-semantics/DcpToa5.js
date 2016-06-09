var Dcp = require('./Dcp');
var Enums = require('./../Enums');
var Form = Enums.Form;
var Utils = require('./../Utils');

/**
 * It handles DCP-INPE semantics type.
 * @constructor
 */
var DcpToa5 = function(args) {
  Dcp.call(this, args);
};

DcpToa5.prototype = Object.create(Dcp.prototype);
DcpToa5.prototype.constructor = DcpToa5;

DcpToa5.identifier = function() {
  return "DCP-toa5";
};

DcpToa5.prototype.schema = function() {
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

DcpToa5.prototype.form = function() {
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


module.exports = DcpToa5;
