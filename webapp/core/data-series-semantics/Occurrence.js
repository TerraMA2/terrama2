var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var Occurrence = module.exports = function() {
  AbstractClass.apply(this, arguments);
};

Occurrence.prototype = Object.create(AbstractClass.prototype);
Occurrence.prototype.constructor = Occurrence;

Occurrence.schema = function() {
  return {}
};

Occurrence.form = function() {
  return [];
};