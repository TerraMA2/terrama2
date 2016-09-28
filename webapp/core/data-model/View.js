(function() {
  'use strict';

  // Dependency
  var AbstractClass = require("./AbstractData");

  /**
   * It defines a TerraMA² View representation. 
   * @class View
   */
  function View(params) {
    AbstractClass.call(this, {'class': 'View'});
    
    /**
     * View identifier
     * @type {number}
     */
    this.id = params.id;
    /**
     * View name
     * @type {string}
     */
    this.name = params.name;
    /**
     * View description
     * @type {string}
     */
    this.description = params.description;
    /**
     * View server URI. The server may be a GeoServer
     * @type {string}
     */
    this.serverURI = params.serverURI;
    /**
     * View layer URI. It is retrieved from TCP Service
     * @type {string}
     */
    this.layerURI = params.serverURI;
    /**
     * View style script
     * @type {string}
     */
    this.script = params.script;
  }

  View.prototype = Object.create(AbstractClass.prototype);
  View.prototype.constructor = View;

  View.prototype.rawObject = function() {
    var obj = this.toObject;
    delete obj.uri;
    obj.serverURI = this.serverURI;
    obj.layerURI = this.layerURI;

    return obj;
  }

  View.prototype.toObject = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      name: this.name,
      description: this.description,
      uri: this.serverURI,
      script: this.script
    });
  };

  module.exports = View;
} ());