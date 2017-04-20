(function() {
  'use strict';

  // Dependencies

  /**
   * TerraMA² BaseClass of Data Model
   * @type {AbstractData}
   */
  var BaseClass = require('./AbstractData');
  /**
   * TerraMA² Global Utility module
   * @type {Utils}
   */
  var Utils = require("./../Utils");

  /**
   * TerraMA² Risk Data Model representation
   * 
   * @class Risk
   */
  var Risk = function(params) {
    BaseClass.call(this, {'class': 'Risk'});
    /**
     * Risk Identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * Risk name
     * @type {string}
     */
    this.name = params.name;
    /**
     * Risk description
     * @type {string}
     */
    this.description = params.description;

    this.levels = [];


    if (params.levels || params.riskLevels){
      this.levels = params.levels || params.riskLevels;
    } else if (params.RiskLevels) {
      var levelsList = [];
      params.RiskLevels.forEach(function(level){
        levelsList.push(level.get());
      });
      this.levels = levelsList;
    }

  };

  /**
   * It sets risk levels data.
   * @param {Sequelize.Model[]|Object[]}
   */
  Risk.prototype.setRiskLevels = function(RiskLevels) {
    var output = [];
    RiskLevels.forEach(function(RiskLevel) {
      var obj = RiskLevel;
      if (Utils.isFunction(RiskLevel.get)) {
        obj = RiskLevel.get();
      }
      output.push(obj);
    });
    this.levels = output;
  };
  Risk.prototype = Object.create(BaseClass.prototype);
  Risk.prototype.constructor = Risk;

  Risk.prototype.toObject = function() {
    return Object.assign(BaseClass.prototype.toObject.call(this), {
      id: this.id,
      name: this.name,
      description: this.description,
      levels: this.levels
    });
  };

  Risk.prototype.rawObject = function() {
    var toObject = this.toObject();
    return toObject;
  };

  module.exports = Risk;

} ());