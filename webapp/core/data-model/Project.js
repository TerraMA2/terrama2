const BaseClass = require('./AbstractData');
const isObject = require('./../Utils').isObject;

/**
 * @class Project
 *
 * Defines a data abstraction for a TerraMA² Project
 */
class Project extends BaseClass {
  constructor(properties) {
    super({'class': 'Project'});

    if (!isObject(properties))
      throw new Error("No valid project.");

    this.id = properties.id;
    this.name = properties.name;
    this.description = properties.description;
    this.protected = properties.protected;
    this.user_id = properties.user_id;
    this.user_name = properties.user_name;
    this.version = properties.version;
    this.active = properties.active || true;
  }

  toObject() {
    const obj = super.toObject();
    return {
      ...obj,
      id: this.id,
      name: this.name,
      description: this.description,
      protected: this.protected,
      user_id: this.user_id,
      user_name: this.user_name,
      version: this.version,
      active: this.active
    };
  }
}

module.exports = Project;
