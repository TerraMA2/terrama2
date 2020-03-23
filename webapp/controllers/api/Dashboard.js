const {Connection} = require('../../core/utility/connection');
const FilterService = require("../../core/facade/Filter");

const env = process.env.NODE_ENV.toLowerCase() || 'development';
const config = require('../../config/db')[env];


const URI = `postgis://${config.username}:${config.password}@${config.host}:${config.port}/${config.database}`;

module.exports = function(app) {

  return {
    getAnalysisTotals: async (request, response) => {
      const params = {
        specificParameters,
        date,
        filter
      } = request.query;

      const conn = new Connection(URI);
      await conn.connect();

      try {
        const sql = await FilterService.getSqlAnalysisTotals(conn, params);


        const result = await conn.execute(sql);
        result.rows[0].activearea = true;

        response.json(result.rows);
      } catch (error) {
        console.log(error);
      } finally {
        await conn.disconnect();
      }
    },
    getDetailsAnalysisTotals: async (request, response) => {
      const params = {
        specificParameters,
        date,
        filter
      } = request.query;

      const conn = new Connection(URI);
      await conn.connect();

      const result = await FilterService.getAlertsGraphics(conn, params);
      try {
        response.json(result);
      } catch (error) {
        console.log(error);
      } finally {
        await conn.disconnect();
      }
    }
  }
} ();
