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
    properties: Utils.extend(Utils.getFolderSchema(), {
      ctl_filename: {
        type: Form.Field.TEXT,
        title: "CTL File"
      },
      srid: {
        type: Form.Field.NUMBER,
        title: "SRID"
      },
      timezone: {
        type: Form.Field.TEXT,
        title: "Timezone"
      },
      number_of_bands: {
        type: Form.Field.NUMBER,
        title: "Number of Bands",
        default: 1
      },
      value_multiplier: {
        type: Form.Field.NUMBER,
        title: "Multiplier",
        default: 1
      },
      binary_file_mask: {
        type: Form.Field.TEXT,
        title: "Binary File Mask"
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
      temporal: {
        type: Form.Field.CHECKBOX,
        title: "Temporal"
      },
      time_interval: {
        type: Form.Field.NUMBER,
        title: "Time Interval"
      },
      time_interval_unit: {
        type: Form.Field.TEXT,
        title: "Time Interval Unit"
      }
    }),
    required: ['ctl_filename', 'srid', 'timezone', 'number_of_bands', 'value_multiplier', 'bytes_before', 'bytes_after', 'time_interval', 'time_interval_unit']
  };
};

Grads.prototype.form = function() {
  return [
    Utils.getFolderForm(),
    {
      key: 'ctl_filename',
      htmlClass: "col-md-6 terrama2-schema-form grads-form-item"
    },
    {
      key: 'srid',
      htmlClass: "col-md-3 terrama2-schema-form grads-form-item"
    },
    {
      key: 'timezone',
      htmlClass: "col-md-3 terrama2-schema-form grads-form-item",
      type: 'select',
      titleMap: Utils.getTimezonesGUI()
    },
    {
      key: 'number_of_bands',
      htmlClass: "col-md-3 terrama2-schema-form grads-form-item"
    },
    {
      key: 'value_multiplier',
      htmlClass: "col-md-3 terrama2-schema-form grads-form-item"
    },
    {
      key: 'binary_file_mask',
      htmlClass: "col-md-6 terrama2-schema-form grads-form-item"
    },
    {
      key: 'bytes_before',
      htmlClass: "col-md-3 terrama2-schema-form grads-form-item"
    },
    {
      key: 'bytes_after',
      htmlClass: "col-md-3 terrama2-schema-form grads-form-item"
    },
    {
      key: 'temporal',
      htmlClass: "col-md-2 terrama2-schema-form grads-form-item"
    },
    {
      key: 'time_interval',
      htmlClass: "col-md-2 terrama2-schema-form grads-form-item",
      condition: "model.temporal"
    },
    {
      key: 'time_interval_unit',
      htmlClass: "col-md-2 terrama2-schema-form grads-form-item",
      type: 'select',
      titleMap: [{ name: 'Minute', value: 'minute' }, { name: 'Hour', value: 'hour' }, { name: 'Day', value: 'day' }],
      condition: "model.temporal"
    }
  ];
};
