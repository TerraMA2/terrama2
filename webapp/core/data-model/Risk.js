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
     * Project Identifier
     * @type {number}
     */
    this.project_id = params.project_id;
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
      project_id: this.project_id,
      name: this.name,
      description: this.description,
      levels: this.levels
    });
  };

  Risk.prototype.toService = function() {
    var levelsCopy =[];
    //Removing unnecessary properties to send to service
    this.levels.forEach(function(level){
      delete level.id;
      delete level.risk_id;
      levelsCopy.push(level);
    });
    return Object.assign(BaseClass.prototype.toObject.call(this), {
      name: this.name,
      project_id: this.project_id,
      description: this.description,
      levels: levelsCopy
    });
  };

  Risk.prototype.rawObject = function() {
    var toObject = this.toObject();
    return toObject;
  };

  module.exports = Risk;

} ());