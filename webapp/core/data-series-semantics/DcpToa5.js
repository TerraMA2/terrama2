var Dcp = require('./Dcp');
var Enums = require('./../Enums');
var Form = Enums.Form;

/**
 * It handles DCP-INPE semantics type.
 * @constructor
 */
var DcpToa5 = function() {
  Dcp.call(this, {name: "DCP-toa5"});
};

DcpToa5.prototype = Object.create(Dcp.prototype);
DcpToa5.prototype.constructor = DcpToa5;

DcpToa5.demand = function () {
  return ["FILE", "FTP"];
};

DcpToa5.identifier = function() {
  return "DCP-toa5";
};

DcpToa5.schema = function() {
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

DcpToa5.form = function() {
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

DcpToa5.demand = function () {
  return ["FILE", "FTP"];
};

module.exports = DcpToa5;
