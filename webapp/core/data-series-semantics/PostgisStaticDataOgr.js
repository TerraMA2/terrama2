var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var PostgisStaticDataOgr = module.exports = function(args) {
  AbstractClass.apply(this, arguments);
};

PostgisStaticDataOgr.identifier = function() {
  return "STATIC_DATA-postgis";
};

PostgisStaticDataOgr.prototype = Object.create(AbstractClass.prototype);
PostgisStaticDataOgr.prototype.constructor = PostgisStaticDataOgr;

PostgisStaticDataOgr.prototype.schema = function() {
  return {
    properties: {
      table_name: {
        type: Form.Field.TEXT,
        title: "Table Name"
      }
    },
    required: ['table_name']
  }
};

PostgisStaticDataOgr.prototype.form = function() {
  return ['*'];
};
