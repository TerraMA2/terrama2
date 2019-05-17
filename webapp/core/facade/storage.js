const { ValidationErrorItem } = require('sequelize');
const DataManager = require("./../DataManager");
const URIBuilder = require('./../UriBuilder');
const { ScheduleType, Uri } = require('./../Enums');
const TcpService = require('./tcp-manager/TcpService');

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
  /**
   * Retrieves list of storages based in query restriction
   *
   * @param {any} restriction Query restriction.
   */
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

  /**
   * Find storage by id
   *
   * @param {number} id Storage identifier
   */
  async get(id) {
    const matched = await this.list({ where: { id }, limit: 1 });

    return matched[0];
  }

  /**
   * Persists the storager into database
   *
   * @throws {ScheduleError} for any error occurred in operation
   *
   * @param {any} storage Storage to save
   */
  async save(storage){
    let transaction = await DataManager.orm.transaction();

    try {
      const options = { transaction };

      if (storage.schedule && (storage.schedule.scheduleType !== "3")) {
        const createdSchedule = await DataManager.addSchedule(storage.schedule, options);

        storage.schedule_id = createdSchedule.id;
      }

      storage.schedule_type = storage.schedule.scheduleType;

      // Tries to create model Storage
      const createdStorage = await DataManager.orm.models.Storages.create(storage, options);

      transaction.commit();
      
      await TcpService.send({'Storages': [createdStorage]});

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

      // We should define this variable since we cannot remove directly a schedule which belongs to storager
      // due CASCADE operation. We must keep which schedule to remove and after Storage update
      // perform operation.
      let scheduleToRemove = null;

      // If there already schedule
      if (oldStorage.schedule_id) {
        // When no schedule sent, set storager schedule to none and keep id to remove after update sequency
        if (storage.schedule.scheduleType == ScheduleType.MANUAL) {
          scheduleToRemove = oldStorage.schedule_id;
          storage.schedule_id = null;
        } else
          await DataManager.updateSchedule(oldStorage.schedule_id, storage.schedule, { transaction });
      } else {
        if (storage.schedule && storage.schedule.scheduleType != ScheduleType.MANUAL) {
          const createdSchedule = await DataManager.addSchedule(storage.schedule, options);
          storage.schedule_type = storage.schedule.scheduleType;

          storage.schedule_id = createdSchedule.id;
        }
      }
      // Update entire storage entity
      await DataManager.orm.models.Storages.update(storage, { where: { id: storageId }, ...options });

      // Remove schedule when updating to manual type
      if (scheduleToRemove !== null) {
        await DataManager.removeSchedule({ id: scheduleToRemove }, options);
      }

      // Persists the operation
      await transaction.commit();
      
      const updatedStorage = await this.get(storageId);
      
      await TcpService.send({'Storages': [updatedStorage]});

      return;
    } catch (err) {
      await transaction.rollback();

      throw new StorageError(err.errors);
    }
  }

  async delete(id) {
    let transaction = await DataManager.orm.transaction();

    try {
      const options = { transaction };

      await DataManager.orm.models.Storages.destroy({ where: { id }, ...options });

      // Persists the operation
      await transaction.commit();
      
      await TcpService.remove({'Storages': [id]});
    } catch (err) {
      await transaction.rollback();

      throw new StorageError(err.errors);
    }
  }
}
module.exports = storageFacade;
