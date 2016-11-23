var DataSeriesSemanticsError = require('./../Exceptions').DataSeriesSemanticsError;
var Application = require("./../Application");

function availableTypes() {
  var output = [];

  return output.concat(plugins);
}

// list of all instances of semantics type
var semanticsTypes = Application.get("semantics").map(function(semanticsItem) {
  if (semanticsItem.gui) {
    return semanticsItem;
  }
});

var Factory = module.exports = {};

function getSemanticHelper(identifier) {
  var semanticsOutput = null;
  semanticsTypes.some(function(semantics) {
    if (semantics.code === identifier) {
      semanticsOutput = semantics;
      return true;
    }
  });

  if (semanticsOutput)
    return semanticsOutput;

  throw new DataSeriesSemanticsError("DataSeriesSemantics is invalid.");
}

Factory.build = function(args) {
  return getSemanticHelper(args.code);
};

Factory.listAll = function() {
  return semanticsTypes;
};
