/**
 * A generic Filter object. It follows Design Pattern Criteria */
var Filter = module.exports = function() {};

/**
 * @param {Array} array - An array of values to be filtered
 * @param {Object} extra - An extra object for containing helper values or helper functions
 * @return {Promise<Array>} It must return a Promise of Array for generic usages */
Filter.prototype.match = function(array, extra) {
  throw new Error("It must be implemented");
};