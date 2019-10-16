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
      getStaticData: async (request, response) => {
        const {
          projectName,
          limit,
          offset,
          count,
          viewId
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

        let sqlSelect = `SELECT *`
        let sqlFrom = ''
        let sqlWhere = ''

        sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(geom), 4326)) AS "lat",
                      ST_X(ST_Transform (ST_Centroid(geom), 4326)) AS "long"
        `

        sqlFrom += ` FROM public.${tableName}`

        if (limit) {
          sqlWhere +=` LIMIT ${limit}`
        }

        if(offset) {
          sqlWhere +=` OFFSET ${offset}`
        }

        const sql = sqlSelect + sqlFrom + sqlWhere

        let result
        let resultCount
        try {
          result = await conn.execute(sql)
          let dataJson = result.rows

          let sqlCount
          if (count) {
            sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName}`
            resultCount = await conn.execute(sqlCount)
            dataJson.push(resultCount.rows[0]['count'])
          }

          await conn.disconnect()
          response.json(dataJson)
        } catch (error) {
          console.log(error)
        }
      },
      getDynamicData: async (request, response) => {
        const {
          limit,
          offset,
          date,
          localization,
          area,
          count,
          viewId
        } = request.query
        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
        const uri = dataProvider.uri
        // const conn = new Connection(uri)
        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        const dataSet = dataSeries.dataSets[0]
        const tableName = dataSet.format.table_name
        const geomColumn = dataSet.format.geometry_property
        const timeStampColumn = dataSet.format.timestamp_property

        let sqlSelect = `SELECT *`
        let sqlFrom = ''
        let sqlWhere = ''

        if (geomColumn) {
          sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(${geomColumn}), 4326)) AS "lat",
                        ST_X(ST_Transform (ST_Centroid(${geomColumn}), 4326)) AS "long"
          `
        }

        sqlFrom += ` FROM public.${tableName}`

        if (date) {
          const { dateFrom, dateTo } = date

          sqlWhere += `
              WHERE ${timeStampColumn}::date >= '${dateFrom}' AND ${timeStampColumn}::date <= '${dateTo}'
          `
        }

        if (limit) {
          sqlWhere +=` LIMIT ${limit}`
        }

        if(offset) {
          sqlWhere +=` OFFSET ${offset}`
        }

        const sql = sqlSelect + sqlFrom + sqlWhere

        let result
        let resultCount
        try {
          result = await conn.execute(sql)
          let dataJson = result.rows

          let sqlCount
          if (count) {
            sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName}`
            resultCount = await conn.execute(sqlCount)
            dataJson.push(resultCount.rows[0]['count'])
          }

          await conn.disconnect()
          response.json(dataJson)
        } catch (error) {
          console.log(error)
        }
      },
      getAnalysisData: async (request, response) => {
        const {
          projectName,
          limit,
          offset,
          group,
          date,
          localization,
          area,
          count,
          viewId
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

        let sqlTableName = `SELECT DISTINCT table_name FROM ${tableName}`
        resultTableName = await conn.execute(sqlTableName)

        tableName = resultTableName.rows[0]['table_name']

        let sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName} `
        let sqlSelect = `SELECT *`
        let sqlFrom = ''
        let sqlWhere = ''

        sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(intersection_geom), 4326)) AS "lat",
                      ST_X(ST_Transform (ST_Centroid(intersection_geom), 4326)) AS "long"
        `

        sqlFrom += ` FROM public.${tableName}`

        if (date) {
          const dateFrom = date[0];
          const dateTo = date[1];

          sqlWhere += `
              WHERE execution_date::date >= '${dateFrom}' AND execution_date::date <= '${dateTo}'
          `
        }
        sqlCount += sqlWhere

        if (area) {
          sqlWhere += ` AND calculated_area_ha > ${area}`
        }

        if (limit) {
          sqlWhere +=` LIMIT ${limit}`
        }

        if(offset) {
          sqlWhere +=` OFFSET ${offset}`
        }

        const sql = sqlSelect + sqlFrom + sqlWhere

        let result
        let resultCount
        try {
          result = await conn.execute(sql)
          let dataJson = result.rows

          if (count) {
            resultCount = await conn.execute(sqlCount)
            dataJson.push(resultCount.rows[0]['count'])
          }

          await conn.disconnect()
          response.json(dataJson)
        } catch (error) {
          console.log(error)
        }
      },
      getCarData: async (request, response) => {
        const {
          carRegister,
          intersectId,
          viewId
        } = request.query

        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
        // const uri = dataProvider.uri
        const tableName = dataSeries.dataSets[0].format.table_name
        // const conn = new Connection(uri)
        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        let sqlDeter = `
          SELECT orbitpoint, date, sensor, satellite FROM public.dd_deter_inpe di where di.gid = ${intersectId}
        `
        let sql = `SELECT
            car.numero_do2 AS register,
            car.area_ha_ AS area,
            car.nome_da_p1 AS name,
            car.municipio1 AS city,
            substring(ST_EXTENT(munic.geom)::TEXT, 5, length(ST_EXTENT(munic.geom)::TEXT) - 5) as citybbox,
            substring(ST_EXTENT(car.geom)::TEXT, 5, length(ST_EXTENT(car.geom)::TEXT) - 5) as bbox,
            ST_Y(ST_Transform (ST_Centroid(car.geom), 4326)) AS "lat",
            ST_X(ST_Transform (ST_Centroid(car.geom), 4326)) AS "long"
            FROM public.${tableName} AS car
            INNER JOIN de_municipios_sema munic ON
            car.numero_do2 = '${carRegister}'
            AND munic.municipio = car.municipio1
            GROUP BY car.numero_do2, car.area_ha_, car.nome_da_p1, car.municipio1, car.geom`;

        const result = await conn.execute(sql)
        let propertyData = result.rows[0]

        const sqlBurningSpotlights = `
                    SELECT
                    count(*) as focuscount,
                    extract('YEAR' FROM focus.execution_date) as year
                    FROM public.apv_car_focos_48 focus
                    INNER JOIN public.${tableName} AS car on
                    focus.de_car_validado_sema_numero_do2 = car.numero_do2 AND
                    car.numero_do2 = '${carRegister}'
                    group by year
                  `

        const resultBurningSpotlights = await conn.execute(sqlBurningSpotlights)
        const burningSpotlights = resultBurningSpotlights.rows

        const sqlBurnedAreas = `
                    SELECT
                    count(*) as focuscount,
                    extract('YEAR' FROM focus.execution_date) as year
                    FROM public.apv_car_focos_48 focus
                    INNER JOIN public.${tableName} AS car on
                    focus.de_car_validado_sema_numero_do2 = car.numero_do2 AND
                    car.numero_do2 = '${carRegister}'
                    group by year
                  `

        const resultDeter = await conn.execute(sqlDeter)
        const deter = resultDeter.rows

        const resultBurnedAreas = await conn.execute(sqlBurnedAreas)
        const burnedAreas = resultBurnedAreas.rows

        if (propertyData) {
          propertyData.burningSpotlights = burningSpotlights
          propertyData.burnedAreas = burnedAreas
          propertyData.deter = deter[0]
        }

        await conn.disconnect()
        response.json(propertyData)
      },
    }
} ();
