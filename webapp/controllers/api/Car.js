const {Connection} = require('../../core/utility/connection');
const DataManager = require('../../core/DataManager');
const ViewFacade = require('../../core/facade/View');
const Filter = require('../../core/facade/Filter');

const env = process.env.NODE_ENV.toLowerCase() || 'development';
const config = require('../../config/db')[env];

/**
 * Injecting NodeJS App configuration AS dependency. It retrieves a Views controllers API
 *
 * @param {Object}
 * @returns {Object}
 */

module.exports = function(app) {
    const uri =  `postgis://${config.username}:${config.password}@${config.host}:${config.port}/${config.database}`;

    return {
        getAllSimplified: async (request, response) => {
            const specificParameters = JSON.parse(request.query.specificParameters);
            const view = JSON.parse(specificParameters.view);

            const conn = new Connection(uri);
            await conn.connect();

            const table = {
                name: await Filter.getTableName(conn, view.id),
                alias: specificParameters.tableAlias,
                owner: ''
            };

            const filter =
              specificParameters.isDynamic ?
                await Filter.setFilter(conn, request.query, table, view) :
                {
                    sqlWhere: '',
                    secondaryTables: '',
                    sqlHaving: '',
                    order: '',
                    limit: specificParameters.limit ? ` LIMIT ${specificParameters.limit}` : '',
                    offset: specificParameters.offset ? ` OFFSET ${specificParameters.offset}` : ''
                };

            const sqlSelectCount = specificParameters.count ? `,COUNT(${specificParameters.tableAlias}.*) AS ${specificParameters.countAlias}` : '';
            const sqlSelectSum = specificParameters.sum ? `,SUM(${specificParameters.tableAlias}.${specificParameters.sumField}) AS ${specificParameters.sumAlias}` : '';
            const sqlSelect =
                ` SELECT property.numero_do1 AS registro_estadual,
                    property.numero_do2 AS registro_federal,
                    property.nome_da_p1 AS nome_propriedade,
                    property.municipio1 AS municipio,
                    property.area_ha_ AS area,
                    property.situacao_1 AS situacao,
                    ST_Y(ST_Transform (ST_Centroid(property.geom), 4326)) AS "lat",
                    ST_X(ST_Transform (ST_Centroid(property.geom), 4326)) AS "long"
                    ${sqlSelectSum}
                    ${sqlSelectCount} `;

            const sqlFrom = ` FROM public.${table.name} AS ${specificParameters.tableAlias}`;

            const sqlGroupBy =
                ` GROUP BY  property.numero_do1,
                        property.numero_do2,
                        property.nome_da_p1,
                        property.municipio1,
                        property.area_ha_,
                        property.situacao_1,
                        ST_Y(ST_Transform (ST_Centroid(property.geom), 4326)),
                        ST_X(ST_Transform (ST_Centroid(property.geom), 4326)) `;
            const sqlOrderBy = ` ORDER BY ${specificParameters.sortField} DESC `;



            const column = view.isPrimary ?  'de_car_validado_sema_numero_do1' :  'a_carfocos_20_de_car_validado_sema_numero_do1';


            filter.secondaryTables += specificParameters.isDynamic ?
              '  , public.de_car_validado_sema AS property' :
              '';

            filter.sqlWhere += specificParameters.isDynamic ?
              ` AND property.numero_do1 = ${specificParameters.tableAlias}.de_car_validado_sema_numero_do1 `: '';

            const sqlWhere =
              filter.sqlHaving ?
                ` ${filter.sqlWhere} 
                    AND ${specificParameters.tableAlias}.de_car_validado_sema_numero_do1 IN
                      ( SELECT tableWhere.${column} AS subtitle
                        FROM public.${table.name} AS tableWhere
                        GROUP BY tableWhere.de_car_validado_sema_numero_do1
                        ${filter.sqlHaving}) ` :
                filter.sqlWhere;

            let sql =
              ` ${sqlSelect}
                ${sqlFrom}
                ${filter.secondaryTables}
                ${sqlWhere}
                ${sqlGroupBy}
                ${sqlOrderBy}
                ${filter.limit}
                ${filter.offset}`;

            try {
                const result = await conn.execute(sql);
                const car = result.rows;

                const resultCount = await conn.execute(
                  `SELECT ROW_NUMBER() OVER(ORDER BY property.numero_do2 ASC) AS count 
                        ${sqlFrom} ${filter.secondaryTables} ${sqlWhere} ${sqlGroupBy} ORDER BY count DESC LIMIT 1`);
                car.push(resultCount.rows[0]['count']);

                await conn.disconnect();
                response.json(car)
            } catch (error) {
                console.log(error)
            }
        },

        getAll: async (request, response) => {

            const conn = new Connection(uri)
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
