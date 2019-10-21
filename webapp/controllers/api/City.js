  var {Connection} = require('../../core/utility/connection');

  /**
   * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
   *
   * @param {Object}
   * @returns {Object}
   */
  module.exports = function(app) {
    return {
      getCities: async (request, response) => {
        // const {
        // } = request.query
        // const view = await ViewFacade.retrieve(viewId)
        // const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
        // const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
        // const uri = dataProvider.uri
        // const conn = new Connection(uri)
        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        const sql = `SELECT * FROM de_municipios_sema`

        let result
        try {
          result = await conn.execute(sql)
          let cities = result.rows

          await conn.disconnect()
          response.json(cities)
        } catch (error) {
          console.log(error)
        }
      }
    }
} ();
