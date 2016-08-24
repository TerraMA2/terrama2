'use strict';

var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var Grads = module.exports = function(args) {
  AbstractClass.apply(this, arguments);
};

Grads.identifier = function() {
  return "GRID-grads";
};

Grads.prototype = Object.create(AbstractClass.prototype);
Grads.prototype.constructor = Grads;

Grads.prototype.schema = function() {
  return {
    properties: {
      mask: {
        type: Form.Field.TEXT,
        title: "Mask"
      },
      srid: {
        type: Form.Field.NUMBER,
        title: "SRID"
      }
    },
    required: ['mask', 'srid']
  };
};

Grads.prototype.form = function() {
  return [
    {
      key: 'mask',
      htmlClass: "col-md-6"
    },
    {
      key: 'srid',
      htmlClass: "col-md-6"
    }
  ];
};
