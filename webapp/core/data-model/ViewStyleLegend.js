(function() {
  'use strict';

  // Dependency
  var AbstractClass = require("./AbstractData");

  /**
   * It defines a TerraMA² ViewStyleLegend Legend representation. 
   * @class ViewStyleLegend
   */
  function ViewStyleLegend(params) {
    // Calling base abstraction
    AbstractClass.call(this, {'class': 'ViewStyleLegend'});
    /**
     * ViewStyleLegend identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * ViewStyleLegend Type identifier
     * @type {number}
     */
    this.typeId = params.type_id || params.typeId;
    /**
     * ViewStyleLegend column
     * @type {string}
     */
    this.column = params.column;
    /**
     * ViewStyleLegend bands
     * @type {string}
     */
    this.bands = params.bands;
    /**
     * It defines the ViewStyleLegend colors used
     * @type {any}
     */
    this.colors = params.colors || [];
  }

  ViewStyleLegend.prototype = Object.create(AbstractClass.prototype);
  ViewStyleLegend.prototype.constructor = ViewStyleLegend;

  ViewStyleLegend.prototype.addColor = function(color) {
    this.colors.push(color);
  };

  /**
   * It builds a database representation of ViewStyleLegend
   */
  ViewStyleLegend.prototype.rawObject = function() {
    return this.toObject();
  };

  /**
   * It builds a standardized TCP format
   * 
   * @returns {Object}
   */
  ViewStyleLegend.prototype.toObject = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      column: this.column,
      type_id: this.typeId,
      colors: this.colors,
      bands: this.bands
    });
  };

  module.exports = ViewStyleLegend;
} ());