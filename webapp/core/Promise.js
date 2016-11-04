'use strict';

/**
 * Defining fixed Promise used in TerraMA² Contexts
 * 
 * @type {bluebird.Promise}
 */
var Promise = require('bluebird').getNewLibraryCopy();

module.exports = Promise;
module.exports.Promise = Promise;
module.exports.default = Promise;