var AbstractClass = require("./AbstractData");
var Utils = require('./../Utils');

function Intersection(params) {
  AbstractClass.call(this, {'class': 'Intersection'});

  this.id = params.id;
  this.collector_id = params.collector_id;
  this.attribute = params.attribute;
  this.alias = params.alias;
  this.dataseries_id = params.dataseries_id;
}

Intersection.prototype = Object.create(AbstractClass.prototype);
Intersection.prototype.constructor = Intersection;

Intersection.prototype.toObject = function() {
  return Object.assign(AbstractClass.prototype.toObject.call(this), {
    collector_id: this.collector_id,
    attribute: this.attribute,
    alias: this.alias,
    dataseries_id: this.dataseries_id
  });
};

module.exports = Intersection;
