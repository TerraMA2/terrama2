var DataSeriesSemanticsError = require('./../Exceptions').DataSeriesSemanticsError;
var Application = require("./../Application");

const getSemanticsTypes = () => {
  // list of all instances of semantics type
  var semanticsTypes = Application.get("semantics").map((semanticsItem) => {
    if (semanticsItem.gui) {
      return semanticsItem;
    }
  });

  return semanticsTypes;
}

var Factory = module.exports = {};

function getSemanticHelper(identifier) {
  var semanticsOutput = null;
  getSemanticsTypes().some((semantics) => {
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
  return getSemanticsTypes();
};