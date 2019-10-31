var {Connection} = require('../../core/utility/connection');
var DataManager = require('../../core/DataManager');
var ViewFacade = require("../../core/facade/View");

/**
 * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
 *
 * @param {Object}
 * @returns {Object}
 */
module.exports = function(app) {
    return {
        getAllSimplified: async (request, response) => {
            const {
                viewId,
                limit,
                offset,
                count
            } = request.query
            const view = await ViewFacade.retrieve(viewId)
            const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
            const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
            const uri = dataProvider.uri
              // const conn = new Connection(uri)
            const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
            await conn.connect()

            const dataSet = dataSeries.dataSets[0]
            let tableName = dataSet.format.table_name

            let sql = `SELECT
                            numero_do1,
                            numero_do2,
                            nome_da_p1,
                            municipio1,
                            area_ha_,
                            situacao_1,
                            ST_Y(ST_Transform (ST_Centroid(geom), 4326)) AS "lat",
                            ST_X(ST_Transform (ST_Centroid(geom), 4326)) AS "long"
                        FROM public.${tableName}`

            if (limit) {
                sql +=` LIMIT ${limit}`
            }

            if (offset) {
                sql +=` OFFSET ${offset}`
            }

            try {
                const result = await conn.execute(sql)
                const car = result.rows

                if (count) {
                    sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName}`
                    resultCount = await conn.execute(sqlCount)
                    car.push(resultCount.rows[0]['count'])
                }

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
