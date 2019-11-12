  var {Connection} = require('../../core/utility/connection');

  /**
   * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
   *
   * @param {Object}
   * @returns {Object}
   */
  module.exports = function(app) {
    return {
      getAll: async (request, response) => {

        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        const sql = `SELECT gid, bioma as name FROM public.de_biomas_mt`

        let result
        try {
          result = await conn.execute(sql)
          let biome = result.rows

          await conn.disconnect()
          response.json(biome)
        } catch (error) {
          console.log(error)
        }
      }
    }
} ();
