var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var Occurrence = module.exports = function(args) {
  AbstractClass.apply(this, arguments);
};

Occurrence.prototype = Object.create(AbstractClass.prototype);
Occurrence.prototype.constructor = Occurrence;
