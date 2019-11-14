var {Connection} = require('../../core/utility/connection');

/**
 * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
 *
 * @param {Object}
 * @returns {Object}
 */
module.exports = function(app) {
    const url = "postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt";
    const table = ' public.de_projus_bacias_sema ';

    return {
        getAll: async (request, response) => {
            const conn = new Connection(url);
            await conn.connect();

            const sql =
                ` SELECT gid, pjbh AS name, promotoria
                  FROM ${table}
                  ORDER BY name `;

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
