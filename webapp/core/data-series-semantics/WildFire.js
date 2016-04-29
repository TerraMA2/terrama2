var Occurrence = require('./Occurrence');
var Enums = require('./../Enums');
var Form = Enums.Form;

var WildFire = function() {
  Occurrence.call(this, {name: "WILD-FIRES"});
};

WildFire.prototype = Object.create(Occurrence.prototype);
WildFire.prototype.constructor = WildFire;

WildFire.identifier = function() {
  return "WILD-FIRES";
};

WildFire.schema = function() {
  var occurrenceSchema = Occurrence.schema.call(this);

  var properties = {
    mask: {
      type: Form.Field.TEXT,
      title: "Mask"
    },
    srid: {
      type: Form.Field.TEXT,
      title: "SRID"
    },
    timezone: {
      type: Form.Field.TEXT,
      title: "Timezone"
    }
  };
  
  if (occurrenceSchema.hasOwnProperty('properties')) {
    occurrenceSchema.properties.srid = properties.srid;
    occurrenceSchema.properties.timezone = properties.timezone;
  } else {
    Object.assign(occurrenceSchema, {properties: properties});
  }
  
  var requiredFields = ['mask', 'srid', 'timezone'];
  
  occurrenceSchema.required = occurrenceSchema.hasOwnProperty('required') ? occurrenceSchema.required.concat(requiredFields) : requiredFields;

  return occurrenceSchema;
};

WildFire.form = function() {
  return [
    {
      key: 'mask',
      htmlClass: 'col-md-4'
    },
    {
      key: 'srid',
      htmlClass: "col-md-4"
    },
    {
      key: 'timezone',
      type: 'select',
      htmlClass: "col-md-4",
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
  ].concat(Occurrence.form.call(this));
};

module.exports = WildFire;