const {Connection} = require('../../core/utility/connection');
const FilterService = require("../../core/facade/Filter");

module.exports = function(app) {

  return {
    getAnalysisTotals: async (request, response) => {
      const params = {
        specificParameters,
        date,
        filter
      } = request.query;

      const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt");
      await conn.connect();

      const sql = await FilterService.getSqlAnalysisTotals(conn, params);

      try {
        const result = await conn.execute(sql);

        await conn.disconnect();
        response.json(result.rows);
      } catch (error) {
        console.log(error);
      }
    },
    getDetailsAnalysisTotals: async (request, response) => {
      const params = {
        specificParameters,
        date,
        filter
      } = request.query;

      const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt");
      await conn.connect();

      const result = await FilterService.getAlertsGraphics(conn, params);
      try {
        await conn.disconnect();
        response.json(result);
      } catch (error) {
        console.log(error);
      }
    }
  }
} ();
