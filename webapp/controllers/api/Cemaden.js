module.exports = (/*app*/) => {
  const Cemaden = require('./../../core/data-series-semantics/Cemaden');
  const DataManager = require('./../../core/DataManager');

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
      return response.status(400).json({ error: "Format Cemaden not found. Please contact System Administrator for further details." });
    }
  };

  return { list };
};