(function() {
  'use strict';

  // Dependency
  var AbstractClass = require("./AbstractData");
  var URIBuilder = require("./../UriBuilder");

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
    this.serverUri = params.serverUri;
    /**
     * View layer URI. It is retrieved from TCP Service
     * @type {string}
     */
    this.layerUri = params.serverUri;
    /**
     * View style script
     * @type {string}
     */
    this.script = params.script;
  }

  View.prototype = Object.create(AbstractClass.prototype);
  View.prototype.constructor = View;

  View.prototype.rawObject = function() {
    var obj = this.toObject();
    delete obj.uri;
    obj.serverUri = this.serverUri;
    obj.layerUri = this.layerUri;

    return obj;
  }

  View.prototype.toObject = function() {
    return Object.assign(AbstractClass.prototype.toObject.call(this), {
      id: this.id,
      name: this.name,
      description: this.description,
      serverUri: this.serverUri,
      script: this.script
    });
  };

  module.exports = View;
} ());