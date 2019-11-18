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
    const uri = "postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt";
    const conn = new Connection(uri)

    return {
        getAllSimplified: async (request, response) => {
            const {
                viewId,
                limit,
                offset,
                count,
                sortColumn,
                sortOrder,
                filter
            } = request.query

            const view = await ViewFacade.retrieve(viewId)
            const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
            const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
            const uri = dataProvider.uri
              // const conn = new Connection(uri)
            const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
            await conn.connect()

            let tableName = dataSeries.dataSets[0].format.table_name

            if (sortColumn !== 'car') {
                let sqlTableName = `SELECT DISTINCT table_name FROM ${tableName}`
                resultTableName = await conn.execute(sqlTableName)

                tableName = resultTableName.rows[0]['table_name']
            }

            let sql = '';

            let sqlSelect = '';
            let sqlSelectSum = '';
            let sqlSelectCount = '';
            let sqlFrom = '';
            let sqlJoin = '';
            let sqlGroupBy = '';
            let sqlOrderBy = '';

            if (sortColumn === 'car') {
                sqlOrderBy = `area DESC`
            } else {
                sqlSelectCount = `,COUNT(car_${sortColumn}.*) as ${sortColumn}_count`;
                sqlOrderBy = `${sortColumn}_area DESC`;
            }

            if (sortColumn === 'focos') {
                sqlOrderBy = `${sortColumn}_count DESC`
            } else if (sortColumn !== 'car'){
                sqlSelectSum = `,SUM(car_${sortColumn}.calculated_area_ha) as ${sortColumn}_area`
            }

            let tableAlias = sortColumn !== 'car'?`car_${sortColumn}`:'property';

            sqlSelect += `
                    SELECT property.numero_do1 AS registro_estadual,
                            property.numero_do2 AS registro_federal,
                            property.nome_da_p1 AS nome_propriedade,
                            property.municipio1 AS municipio,
                            property.area_ha_ AS area,
                            property.situacao_1 AS situacao,
                            ST_Y(ST_Transform (ST_Centroid(property.geom), 4326)) AS "lat",
                            ST_X(ST_Transform (ST_Centroid(property.geom), 4326)) AS "long"
                            ${sqlSelectSum}
                            ${sqlSelectCount}
            `

            sqlFrom += `
                FROM public.${tableName} AS ${tableAlias}
            `

            if (sortColumn !== 'car') {
                sqlJoin += `
                    LEFT JOIN public.de_car_validado_sema as property
                    ON property.numero_do1 = ${tableAlias}.de_car_validado_sema_numero_do1
                `
            }

            sqlGroupBy += ` GROUP BY registro_estadual, registro_federal, nome_propriedade, municipio, area, situacao, lat, long
                            order by ${sqlOrderBy}
                `

            sql += sqlSelect + sqlFrom + sqlJoin + sqlGroupBy

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
        },

        getByCpf: async (request, response) => {
            const {cpf} = request.query;

            await conn.connect();

            const sqlWhere = cpf ? ` WHERE cpf = '${cpf}' ` : ` `;

            const sql = `SELECT
                            gid,
                            id as id_car,
                            '835.751.920-22' AS cpf,
                            numero_do1 as mt_car_number,
                            numero_do2 as federal_car_number,
                            nome_da_p1 as property_name,
                            municipio1 as city_name,
                            area_ha_ as property_area,
                            situacao_1 as status,
                            geom
                        FROM public.de_car_validado_sema 
                        ${sqlWhere} `;

            try {
                const result = await conn.execute(sql)

                await conn.disconnect()
                response.json(result.rows[0])
            } catch (error) {
                console.log(error)
            }
        }
    }
} ();
