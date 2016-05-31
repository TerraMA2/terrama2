var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var FileStaticDataOgr = module.exports = function() {
  AbstractClass.apply(this, arguments);
};

FileStaticDataOgr.identifier = function() {
  return "STATIC_DATA-ogr";
};

FileStaticDataOgr.prototype = Object.create(AbstractClass.prototype);
FileStaticDataOgr.prototype.constructor = FileStaticDataOgr;

FileStaticDataOgr.schema = function() {
  return {
    properties: {
      file_name: {
        type: Form.Field.TEXT,
        title: "File Name"
      }
    },
    required: ['file_name']
  }
};

FileStaticDataOgr.form = function() {
  return ['*'];
};

FileStaticDataOgr.demand = function() {
  return ["FILE"];
};
