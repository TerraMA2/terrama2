'use strict';

var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

/**
 * It defines a GrADS semantics representation
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
      }
    },
    required: ['ctl_filename', 'srid']
  };
};

Grads.prototype.form = function() {
  return [
    {
      key: 'ctl_filename',
      htmlClass: "col-md-6"
    },
    {
      key: 'srid',
      htmlClass: "col-md-6"
    }
  ];
};
