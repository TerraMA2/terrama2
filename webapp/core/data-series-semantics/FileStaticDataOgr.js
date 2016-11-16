var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;
var Utils = require("./../Utils");

var FileStaticDataOgr = module.exports = function(args) {
  AbstractClass.apply(this, arguments);
};

FileStaticDataOgr.identifier = function() {
  return "STATIC_DATA-ogr";
};

FileStaticDataOgr.prototype = Object.create(AbstractClass.prototype);
FileStaticDataOgr.prototype.constructor = FileStaticDataOgr;

FileStaticDataOgr.prototype.schema = function() {
  return {
    properties: Utils.extend(Utils.getFolderSchema(), {
      mask: {
        type: Form.Field.TEXT,
        title: "File Name"
      }
    }),
    required: ['mask']
  }
};

FileStaticDataOgr.prototype.form = function() {
  return ['*'];
};
