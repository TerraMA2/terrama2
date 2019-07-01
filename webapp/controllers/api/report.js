  var {Connection} = require('../../core/utility/connection');
  var DataManager = require('../../core/DataManager');

  /**
   * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
   *
   * @param {Object}
   * @returns {Object}
   */
  module.exports = function(app) {
    return {
      get: async (request, response) => {
        let {
          projectName, car
        } = request.query

        const dm = await DataManager
        response.json(car)

        // const conn = new Connection(dataProvider.uri)
        // await conn.connect()

        // let sql = ""
        // const result = await conn.execute(sql)
        // let rows = result.rows

        // await conn.disconnect()
        // response.json(rows)
      },
    }
} ();
