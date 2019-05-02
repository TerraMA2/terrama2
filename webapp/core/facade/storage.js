const { ValidationErrorItem } = require('sequelize');
const DataManager = require("./../DataManager");
const URIBuilder = require('./../UriBuilder');
const { ScheduleType, Uri } = require('./../Enums');

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
    const storages = await DataManager.orm.models.Storages.findAll({
      ...restriction,
      include: [
        {
          model: DataManager.orm.models.Schedule
        }
      ]
    });
    return storages.map(storage => {
      const storageOut = storage.get();

      if (storageOut.uri)
        storageOut.uriObject = URIBuilder.buildObject(storageOut.uri, Uri);

      if (storage.Schedule) {
        storageOut.schedule = storage.Schedule.get();
        storageOut.schedule.scheduleType = storage.schedule_type.toString();
        storageOut.schedule.scheduleHandler = storageOut.schedule.frequency_unit || storageOut.schedule.schedule_unit;
      }

      return storageOut;
    });
  }

  async get(id) {
    const matched = await this.list({ where: { id }, limit: 1 });

    return matched[0];
  }

  async validate(storage) {
    try {
      const options = { validate: true }
      await DataManager.orm.models.Storages.create(storage, options);

      if (storage.schedule)
        await DataManager.orm.models.Schedule.create(storage.schedule, options);
    } catch (err) {
      throw new StorageError(err.errors);
    }
  }

  async save(storage){
    let transaction = await DataManager.orm.transaction();

    try {
      // Validate model
      // await this.validate(storage);

      const options = { transaction };

      if (storage.schedule && (storage.schedule.scheduleType !== "3")) {
        const createdSchedule = await DataManager.addSchedule(storage.schedule, options);

        storage.schedule_id = createdSchedule.id;
      }

      storage.schedule_type = storage.schedule.scheduleType;

      // Tries to create model Storage
      const createdStorage = await DataManager.orm.models.Storages.create(storage, options);

      transaction.commit();

      return createdStorage;
    } catch (error) {
      transaction.rollback();

      throw new StorageError(error.errors);
    }
  }

  async update(storageId, storage) {
    let transaction = await DataManager.orm.transaction();

    try {
      const options = { transaction };
      // Retrieves context storage
      const oldStorage = await this.get(storageId);

      // If there already schedule
      if (oldStorage.schedule_id) {
        // When no schedule sent, remove
        if (!storage.schedule)
          await DataManager.removeSchedule({ id: oldStorage.schedule_id }, options);
        else
          await DataManager.updateSchedule(oldStorage.schedule_id, storage.schedule, { transaction });
      } else {
        if (storage.schedule_type != ScheduleType.MANUAL) {
          const createdSchedule = await DataManager.addSchedule(storage.schedule, options);
          storage.schedule_type = storage.schedule.scheduleType;

          storage.schedule_id = createdSchedule.id;
        }

      }

      await DataManager.orm.models.Storages.update(storage, { where: { id: storageId }, ...options });

      await transaction.commit();

      return;
    } catch (err) {
      await transaction.rollback();

      throw new StorageError(err.errors);
    }
  }
}
module.exports = storageFacade;