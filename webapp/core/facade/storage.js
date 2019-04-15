const { ValidationErrorItem } = require('sequelize');
const DataManager = require("./../DataManager");

class StorageError extends Error {
  constructor(errors, code = 400) {
    super('Storage Error');

    this.errors = errors;
    this.code = code;
  }

  getErrors() {
    const { errors } = this;

    const errorList = [];

    for(const error of errors) {
      let formatedError = { };

      if (error instanceof ValidationErrorItem) {
        formatedError.field = error.path;
        formatedError.message = error.message;
      } else {
        formatedError.field = error.message;
      }

      errorList.push(formatedError);
    }

    return errorList;
  }
}

class storageFacade {
  async list(restriction = {}){
    const storages = await DataManager.orm.models.Storages.findAll(restriction);
    return storages;
  }

  async get(id) {
    const matched = await this.list({ where: { id }, limit: 1 });

    return matched[0];
  }

  async validate(storage) {
    try {
      const options = { validate: true }
      await DataManager.orm.models.Storages.create(storage, options);
    } catch (err) {
      throw new StorageError(err.errors);
    }
  }

  async save(storage, options){
    try {
      // Validate model
      await this.validate(storage);

      // Tries to create model Storage
      const createdStorage = await DataManager.orm.models.Storages.create(storage, options);

      return createdStorage;
    } catch (error) {
      throw new StorageError(error.errors);
    }
  }
}
module.exports = storageFacade;