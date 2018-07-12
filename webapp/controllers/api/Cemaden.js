module.exports = (app) => {
  const cemaden = require('./../../core/data-series-semantics/Cemaden');

  const list = (request, response) => {
    const statesStr = request.query.states;

    let stateList = null;
    if (statesStr)
      stateList = statesStr.split(",");

    cemaden.listDCP(stateList)
      .then(data => response.status(200).json(data))
      .catch(error => response.status(400).json({error}));
  };

  return { list };
};