var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var StaticDataOgr = module.exports = function() {
  AbstractClass.apply(this, arguments);
};

StaticDataOgr.identifier = function() {
  return "STATIC_DATA-ogr";
};

StaticDataOgr.prototype = Object.create(AbstractClass.prototype);
StaticDataOgr.prototype.constructor = StaticDataOgr;

StaticDataOgr.schema = function() {
  return {
    properties: {
      file: {
        type: Form.Field.TEXT,
        title: "File"
      }
    },
    required: ['file']
  }
};

StaticDataOgr.form = function() {
  return ['*'];
};

StaticDataOgr.demand = function() {
  return ["FILE", "FTP"];
};
