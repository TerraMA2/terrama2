var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var Occurrence = module.exports = function() {
  AbstractClass.apply(this, arguments);
};

Occurrence.prototype = Object.create(AbstractClass.prototype);
Occurrence.prototype.constructor = Occurrence;

Occurrence.schema = function() {
  return {
    type: Form.Type.OBJECT,
    properties: {
      latitude: {
        type: Form.Field.NUMBER,
        title: "Latitude"
      },
      longitude: {
        type: Form.Field.NUMBER,
        title: "Longitude"
      }
    },

    required: ['latitude', 'longitude']
  }
};

Occurrence.form = function() {
  return [
    'latitude',
    'longitude'
  ];
};