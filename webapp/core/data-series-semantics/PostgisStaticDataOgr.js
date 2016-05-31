var AbstractClass = require('./../AbstractSemantics');
var Form = require('./../Enums').Form;

var PostgisStaticDataOgr = module.exports = function() {
  AbstractClass.apply(this, arguments);
};

PostgisStaticDataOgr.identifier = function() {
  return "STATIC_DATA-postgis";
};

PostgisStaticDataOgr.prototype = Object.create(AbstractClass.prototype);
PostgisStaticDataOgr.prototype.constructor = PostgisStaticDataOgr;

PostgisStaticDataOgr.schema = function() {
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

PostgisStaticDataOgr.form = function() {
  return ['*'];
};

PostgisStaticDataOgr.demand = function() {
  return ["POSTGIS"];
};
