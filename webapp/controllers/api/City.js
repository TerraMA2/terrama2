  var {Connection} = require('../../core/utility/connection');

  /**
   * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
   *
   * @param {Object}
   * @returns {Object}
   */
  module.exports = function(app) {
    const url = "postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt";
    const table = ' public.de_municipios_sema ';

    return {
      getAll: async (request, response) => {
        const conn = new Connection(url);
        await conn.connect();

        const sql =
            ` SELECT gid, id_munic, geocodigo, municipio AS name 
              FROM ${table} 
              WHERE municipio is not null
              ORDER BY municipio `;

        let result;

        try {
          result = await conn.execute(sql);
          let cities = result.rows;

          await conn.disconnect();
          response.json(cities);
        } catch (error) {
          console.log(error);
        }
      },

      getAllRegions: async (request, response) => {
        const conn = new Connection(url)
        await conn.connect();

        const sql =
            ` SELECT ROW_NUMBER() OVER (ORDER BY comarca ASC) AS id, comarca AS name
              FROM ${table}
              GROUP BY comarca
              ORDER BY comarca `;

        let result;
        try {
          result = await conn.execute(sql);
          let cities = result.rows;

          await conn.disconnect();
          response.json(cities);
        } catch (error) {
          console.log(error);
        }
      },

      getAllMesoregions: async (request, response) => {
        const conn = new Connection(url);
        await conn.connect();

        const sql =
            ` SELECT ROW_NUMBER() OVER (ORDER BY nm_meso ASC) AS id, nm_meso AS name
              FROM ${table}
              GROUP BY nm_meso
              ORDER BY nm_meso;`;

        let result;
        try {
          result = await conn.execute(sql);
          let cities = result.rows;

          await conn.disconnect();
          response.json(cities);
        } catch (error) {
          console.log(error);
        }
      },

      getAllMicroregions: async (request, response) => {
        const conn = new Connection(url);
        await conn.connect();

        const sql =
            ` SELECT ROW_NUMBER() OVER (ORDER BY nm_micro ASC) AS id, nm_micro AS name
              FROM ${table}
              GROUP BY nm_micro
              ORDER BY nm_micro; `;

        let result;
        try {
          result = await conn.execute(sql);
          let cities = result.rows;

          await conn.disconnect();
          response.json(cities)
        } catch (error) {
          console.log(error)
        }
      }
    }
} ();
