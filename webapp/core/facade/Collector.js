const DataManager = require('./../DataManager');

class CollectorFacade {
  /**
   * Update and persist collector on database. It also update collector input output data sets
   *
   * @throws {Promise<Error>} on operation error
   *
   * @param {number} collectorId Collector Identifier
   * @param {any} collector Collector object to update
   * @param {any} options ORM parameters such transaction object
   */
  async updateCollector(collectorId, collector, options) {
    if (!options)
      options = { };

    // Update collector into database and persist on database if no transaction set
    await DataManager.updateCollector(collectorId, collector, options);

    // Apply changes into collector input output
    await this.updateCollectorIntent(collectorId, collector, options);
  }

  /**
   * Update and handle collector input output data set rules
   *
   * Whenever a collector updates, it checks for new/deleted datasets (DCP only)
   * and handle CollectorInputOutput table in order to the C++ services
   * be able to collect and to access the affected data.
   *
   * @param {number} collectorId Collector identifier
   * @param {any} collector Collector object affected
   * @param {any} options ORM parameters such transaction object
   */
  async updateCollectorIntent(collectorId, collector, options = { }) {
    // Update collector extent from each data series
    const inputDataSeries = await DataManager.getDataSeries({ id: collector.data_series_input });
    const outputDataSeries = await DataManager.getDataSeries({ id: collector.data_series_output });

    // Get Model
    const { CollectorInputOutput } = DataManager.orm.models;

    const currentListOfCollectorInOut = await DataManager.listCollectorInputOutput({ collector_id: collectorId }, options);

    for(const dataSetIndex in inputDataSeries.dataSets) {
      const dataSet = inputDataSeries.dataSets[dataSetIndex];
      const found = currentListOfCollectorInOut.find(elm => elm.input_dataset === dataSet.id);

      if (!found) {
        await CollectorInputOutput.create({
          collector_id: collectorId,
          input_dataset: dataSet.id,
          output_dataset: outputDataSeries.dataSets[dataSetIndex].id
        }, options)
      }
    }

    return null;
  }
}

module.exports = CollectorFacade;