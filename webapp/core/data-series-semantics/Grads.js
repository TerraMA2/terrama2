'use strict';

var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;
var Utils = require("./../Utils");

/**
 * It defines a GrADS semantics representation
 * 
 * @param {Object} args - An arguments to build a grads value. It may be a orm model instance or just a object 
 * @constructor
 */
var Grads = module.exports = function(args) {
  AbstractClass.apply(this, arguments);
};


Grads.identifier = function() {
  return "GRID-grads";
};

// Making javascript inherits, pointing the prototype to the base class
// and the prototype constructor to Real class.
Grads.prototype = Object.create(AbstractClass.prototype);
Grads.prototype.constructor = Grads;

Grads.prototype.schema = function() {
  return {
    properties: {
      ctl_filename: {
        type: Form.Field.TEXT,
        title: "CTL File"
      },
      srid: {
        type: Form.Field.NUMBER,
        title: "SRID"
      },
      bytes_before: {
        type: Form.Field.NUMBER,
        title: "Bytes Before",
        default: 0
      },
      bytes_after: {
        type: Form.Field.NUMBER,
        title: "Bytes After",
        default: 0
      },
      timezone: {
        type: Form.Field.TEXT,
        title: "Timezone"
      }
    },
    required: ['ctl_filename', 'srid', 'bytes_before', 'bytes_after', 'timezone']
  };
};

Grads.prototype.form = function() {
  return [
    {
      key: 'ctl_filename',
      htmlClass: "col-md-6 terrama2-schema-form"
    },
    {
      key: 'srid',
      htmlClass: "col-md-6 terrama2-schema-form"
    },
    {
      key: 'bytes_before',
      htmlClass: "col-md-6 terrama2-schema-form"
    },
    {
      key: 'bytes_after',
      htmlClass: "col-md-6 terrama2-schema-form"
    },
    {
      key: 'timezone',
      htmlClass: "col-md-6 terrama2-schema-form",
      type: 'select',
      titleMap: Utils.getTimezonesGUI()
    }
  ];
};
