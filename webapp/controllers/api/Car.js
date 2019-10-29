var {Connection} = require('../../core/utility/connection');

/**
 * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
 *
 * @param {Object}
 * @returns {Object}
 */
module.exports = function(app) {
    return {
        getAllSimplified: async (request, response) => {

            const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
            await conn.connect()

            const sql = `SELECT
                            id as id_car,
                            numero_do1 as mt_car_number,
                            nome_da_p1 as property_name,
                            municipio1 as city_name,
                            area_ha_ as property_area,
                            situacao_1 as status
                         FROM public.de_car_validado_sema`

            let result
            try {
                result = await conn.execute(sql)
                let car = result.rows

                await conn.disconnect()
                response.json(car)
            } catch (error) {
                console.log(error)

            }
        },

        getAll: async (request, response) => {

            const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
            await conn.connect()

            const sql = `SELECT 
                            gid,
                            id as id_car,
                            numero_do1 as mt_car_number,
                            numero_do2 as federal_car_number,
                            nome_da_p1 as property_name,
                            municipio1 as city_name,
                            area_ha_ as property_area,
                            situacao_1 as status,
                            geom
                         FROM public.de_car_validado_sema`

            let result
            try {
                result = await conn.execute(sql)
                let car = result.rows

                await conn.disconnect()
                response.json(car)
            } catch (error) {
                console.log(error)
            }
        }
    }
} ();
