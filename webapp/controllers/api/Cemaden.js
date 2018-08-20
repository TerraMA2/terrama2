module.exports = (/*app*/) => {
  const Cemaden = require('./../../core/data-series-semantics/Cemaden');
  const DataManager = require('./../../core/DataManager');

  /**
   * Helper to dispatch CemadenNotSupported
   *
   * @param {Express.Response} response
   * @param {Error} error
   */
  const raiseCemadenNotSupported = (response, error) => {
    return response.status(400).json({ error: "Format Cemaden not found. Please contact System Administrator for further details." });
  }

  const list = (request, response) => {
    try {
      const cemadenObj = new Cemaden();

      const statesStr = request.query.states;
      const dataProviderId = request.query.provider;
      const stationIds = parseInt(request.query.stations);

      // When a data provider not set, Raise an error
      if (!dataProviderId)
        return response.status(400).json({ error: "No data provider informed" });

      // Ensure that data provider exists
      DataManager.getDataProvider({ id: dataProviderId })
        .then(dataProvider => {
          // Defines state list. By default, use all
          let stateList = null;
          if (statesStr)
            stateList = statesStr.split(",");

          // Retrieves cemaden from dataprovider URI
          return cemadenObj.listDCP(stateList, dataProvider, stationIds)
            .then(data => response.status(200).json(data));
        })
        .catch(error => {
          response.status(400).json({ error: error.message })
        });
    } catch (error) {
      raiseCemadenNotSupported(response, error);
    }
  };

  /**
   * URL Handler to retrieve available Cemaden stations
   * @param {Express.Request} request HTTP Request interceptor
   * @param {Express.Response} response HTTP Response object
   */
  const listStations = (request, response) => {
    try {
      const cemadenObj = new Cemaden();

      const cemadenForm = cemadenObj.semantics.gui.form;
      const stationField = cemadenForm.find(fields => fields.key === "station");

      if (!stationField || !stationField.titleMap)
        throw new Error("No station field set");

      // Helper to format station
      const parseStation = (station) => {
        return { id: station.value, name: station.name };
      }

      response.status(200).json(stationField.titleMap.map(station => parseStation(station)));
    } catch (error) {
      raiseCemadenNotSupported(response, error);
    }
  };

  return { list, listStations };
};