const {Connection} = require('../../core/utility/connection');
const env = process.env.NODE_ENV.toLowerCase() || 'development';
const config = require('../../config/db')[env];


const URI = `postgis://${config.username}:${config.password}@${config.host}:${config.port}/${config.database}`;

/**
 * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
 *
 * @param {Object}
 * @returns {Object}
 */
module.exports = function(app) {
    const table = 'public.de_unidade_cons_sema';

    return {
        getAll: async (request, response) => {
            const conn = new Connection(URI);
            await conn.connect();

            const sql =
                `   SELECT  gid, nome AS name
                    FROM ${table} 
                    WHERE nome is not null
                    ORDER BY name `;

            let result;
            try {
                result = await conn.execute(sql);
                let cities = result.rows;

                response.json(cities);
            } catch (error) {
                console.log(error);
            } finally {
                await conn.disconnect();
            }
        }
    }
} ();
