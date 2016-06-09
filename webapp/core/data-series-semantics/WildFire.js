var Occurrence = require('./Occurrence');
var Enums = require('./../Enums');
var Form = Enums.Form;

var WildFire = function(args) {
  Occurrence.call(this, args);
};

WildFire.prototype = Object.create(Occurrence.prototype);
WildFire.prototype.constructor = WildFire;

WildFire.identifier = function() {
  return "OCCURRENCE-wfp";
};

WildFire.prototype.schema = function() {
  var occurrenceSchema = Occurrence.prototype.schema.call(this);

  var properties = {
    mask: {
      type: Form.Field.TEXT,
      title: "Mask"
    },
    srid: {
      type: Form.Field.NUMBER,
      title: "SRID"
    }
  };

  if (occurrenceSchema.hasOwnProperty('properties')) {
    occurrenceSchema.properties.srid = properties.srid;
  } else {
    Object.assign(occurrenceSchema, {properties: properties});
  }

  var requiredFields = ['mask', 'srid'];

  occurrenceSchema.required = occurrenceSchema.hasOwnProperty('required') ? occurrenceSchema.required.concat(requiredFields) : requiredFields;

  return occurrenceSchema;
};

WildFire.prototype.form = function() {
  return [
    {
      key: 'mask',
      htmlClass: 'col-md-6'
    },
    {
      key: 'srid',
      htmlClass: "col-md-6"
    }
  ].concat(Occurrence.prototype.form.call(this));
};

module.exports = WildFire;
