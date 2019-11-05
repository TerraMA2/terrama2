  const {Connection} = require('../../core/utility/connection');
  const DataManager = require('../../core/DataManager');
  const ViewFacade = require("../../core/facade/View");


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
          viewId,
          sortColumn,
          sortOrder
        } = request.query
        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id: view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id})
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

        if (sortColumn && sortOrder) {
          sqlWhere += ` ORDER BY ${sortColumn} ${sortOrder === 1?'ASC':'DESC'}`
        }

        if (limit) {
          sqlWhere += ` LIMIT ${limit}`
        }

        if (offset) {
          sqlWhere += ` OFFSET ${offset}`
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
          viewId,
          sortColumn,
          sortOrder
        } = request.query
        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id: view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id})
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
          const {dateFrom, dateTo} = date

          sqlWhere += `
              WHERE ${timeStampColumn}::date >= '${dateFrom}' AND ${timeStampColumn}::date <= '${dateTo}'
          `
        }

        sqlCount += sqlWhere

        if (sortColumn && sortOrder) {
          sqlWhere += ` ORDER BY ${sortColumn} ${sortOrder === 1?'ASC':'DESC'}`
        }

        if (limit) {
          sqlWhere += ` LIMIT ${limit}`
        }

        if (offset) {
          sqlWhere += ` OFFSET ${offset}`
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
          viewId,
          sortColumn,
          sortOrder
        } = request.query
        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id: view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id})
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

        if (sortColumn && sortOrder) {
          sqlWhere += ` ORDER BY ${sortColumn} ${sortOrder === '1'?'ASC':'DESC'}`
        }

        if (limit) {
          sqlWhere += ` LIMIT ${limit}`
        }

        if (offset) {
          sqlWhere += ` OFFSET ${offset}`
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
          date,
          viewId
        } = request.query

        const view = await ViewFacade.retrieve(viewId)
        const dataSeries = await DataManager.getDataSeries({id: view.data_series_id})
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id})
        // const uri = dataProvider.uri
        const tableName = dataSeries.dataSets[0].format.table_name
        // const conn = new Connection(uri)
        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()
        let dateFrom = null;
        let dateTo = null;
        if (date) {
          dateFrom = date[0];
          dateTo = date[1];
        }

        // let sqlDeter = `
        //   SELECT orbitpoint, date, sensor, satellite FROM public.dd_deter_inpe di where di.gid = ${intersectId}
        // `
        let sql = `SELECT
            car.numero_do1 AS register,
            car.area_ha_ AS area,
            car.nome_da_p1 AS name,
            car.municipio1 AS city,
            substring(ST_EXTENT(munic.geom)::TEXT, 5, length(ST_EXTENT(munic.geom)::TEXT) - 5) as citybbox,
            substring(ST_EXTENT(car.geom)::TEXT, 5, length(ST_EXTENT(car.geom)::TEXT) - 5) as bbox,
            ST_Y(ST_Transform (ST_Centroid(car.geom), 4326)) AS "lat",
            ST_X(ST_Transform (ST_Centroid(car.geom), 4326)) AS "long"
            FROM public.${tableName} AS car
            INNER JOIN de_municipios_sema munic ON
            car.numero_do1 = '${carRegister}'
            AND munic.municipio = car.municipio1
            GROUP BY car.numero_do1, car.area_ha_, car.nome_da_p1, car.municipio1, car.geom`;

        const result = await conn.execute(sql)
        let propertyData = result.rows[0]

        const sqlBurningSpotlights = `
                    SELECT
                    count(*) as focuscount,
                    extract('YEAR' FROM focus.execution_date) as year
                    FROM public.apv_car_focos_48 as focus
                    INNER JOIN public.${tableName} AS car on
                    focus.de_car_validado_sema_numero_do1 = car.numero_do1 AND
                    car.numero_do1 = '${carRegister}'
                    group by year
                  `

        const resultBurningSpotlights = await conn.execute(sqlBurningSpotlights)
        const burningSpotlights = resultBurningSpotlights.rows

        const sqlBurnedAreas = `
                    SELECT
                    count(*) as focuscount,
                    extract('YEAR' FROM focus.execution_date) as year
                    FROM public.apv_car_focos_48 as focus
                    INNER JOIN public.${tableName} AS car on
                    focus.de_car_validado_sema_numero_do1 = car.numero_do1 AND
                    car.numero_do1 = '${carRegister}'
                    group by year
                  `

        // const resultDeter = await conn.execute(sqlDeter)
        // const deter = resultDeter.rows

        const resultBurnedAreas = await conn.execute(sqlBurnedAreas)
        const burnedAreas = resultBurnedAreas.rows

        const sqlProdesYear = `SELECT
                              extract(year from date_trunc('year', cp.execution_date)) AS date,
                              SUM(cp.calculated_area_ha) as area
                              FROM public.apv_car_prodes_40 cp
                              WHERE cp.de_car_validado_sema_numero_do1 = '${carRegister}'
                              GROUP BY date
                              ORDER BY date;`

        const sqlSpotlightsYear = `SELECT
                              extract(year from date_trunc('year', cf.execution_date)) AS date,
                              COUNT(cf.*) as spotlights
                              FROM public.apv_car_focos_48 cf
                              WHERE cf.de_car_validado_sema_numero_do1 = '${carRegister}'
                              GROUP BY date
                              ORDER BY date;`

        let dateSql = ` and execution_date::date >= '${dateFrom}' AND execution_date::date <= '${dateTo}'`

        const sqlIndigenousLand = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_ti_cardeter_68 where apv_car_deter_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`
        const sqlConservationUnit = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_uc_carprodes_65 where apv_car_prodes_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`
        const sqlLegalReserve = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_reserva_cardeter_37 where apv_car_deter_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`
        const sqlAPP = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_app_cardeter_35 where apv_car_deter_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`
        const sqlConsolidatedUse = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_usocon_cardeter_39 where apv_car_deter_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`
        const sqlAnthropizedUse = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_usoant_cardeter_36 where apv_car_deter_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`
        const sqlNativeVegetation = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_veg_cardeter_38 where apv_car_deter_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`

        const resultIndigenousLand = await conn.execute(sqlIndigenousLand)
        const indigenousLand = resultIndigenousLand.rows

        const resultConservationUnit = await conn.execute(sqlConservationUnit)
        const conservationUnit = resultConservationUnit.rows

        const resultLegalReserve = await conn.execute(sqlLegalReserve)
        const legalReserve = resultLegalReserve.rows

        const resultAPP = await conn.execute(sqlAPP)
        const app = resultAPP.rows

        const resultConsolidatedUse = await conn.execute(sqlConsolidatedUse)
        const consolidatedArea = resultConsolidatedUse.rows

        const resultAnthropizedUse = await conn.execute(sqlAnthropizedUse)
        const anthropizedUse = resultAnthropizedUse.rows

        const resultNativeVegetation = await conn.execute(sqlNativeVegetation)
        const nativeVegetation = resultNativeVegetation.rows

        const resultProdesYear = await conn.execute(sqlProdesYear)
        const prodesYear = resultProdesYear.rows

        const resultSpotlightsYear = await conn.execute(sqlSpotlightsYear)
        const spotlightsYear = resultSpotlightsYear.rows

        if (propertyData) {
          propertyData.burningSpotlights = burningSpotlights
          propertyData.burnedAreas = burnedAreas
          // propertyData.deter = deter[0]
          propertyData.prodesYear = prodesYear
          propertyData.spotlightsYear = spotlightsYear
          propertyData.indigenousLand = indigenousLand[0]
          propertyData.conservationUnit = conservationUnit[0]
          propertyData.legalReserve = legalReserve[0]
          propertyData.app = app[0]
          propertyData.consolidatedArea = consolidatedArea[0]
          propertyData.anthropizedUse = anthropizedUse[0]
          propertyData.nativeVegetation = nativeVegetation[0]
        }

        await conn.disconnect()
        response.json(propertyData)
      },
      getAnalysisTotals: async (request, response) => {
        const params = {
          viewId,
          groupCod,
          date,
          projectName,
          group,
          localization,
          area,
          count
        } = request.query;

        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt");
        await conn.connect();

        let sql = '';

        if (params.viewId && params.viewId !== 'null'){

          const view = await ViewFacade.retrieve(params.viewId);

          const dataSeries = await DataManager.getDataSeries({id: view.data_series_id});
          const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id});
          const uri = dataProvider.uri;

          const dataSet = dataSeries.dataSets[0];
          let tableName = dataSet.format.table_name;

          let sqlTableName = ` SELECT DISTINCT table_name FROM ${tableName}`;
          resultTableName = await conn.execute(sqlTableName);

          tableName = resultTableName.rows[0]['table_name'];

          let sqlWhere = '';

          if (params.date && params.date !== "null") {
            const dateFrom = params.date[0];
            const dateTo = params.date[1];
            sqlWhere += ` WHERE execution_date BETWEEN '${dateFrom}' AND '${dateTo}' `
          }
          if (params.area && params.area !== "null") {
            sqlWhere += ` AND calculated_area_ha > ${params.area} `
          }

          sql = params.groupCod === 'FOCOS' ?
              `SELECT (
                        SELECT ROW_NUMBER() OVER(ORDER BY de_car_validado_sema_numero_do1 ASC) AS Row
                        FROM public.${tableName}
                        ${sqlWhere}
                        GROUP BY de_car_validado_sema_numero_do1
                        ORDER BY Row DESC
                        LIMIT 1
                ) AS num_car,
                (
                        SELECT coalesce(sum(1), 0.00) as num_focos
                        FROM public.${tableName}
                        ${sqlWhere}
                ) AS num_focos; ` :
              ` SELECT COALESCE(SUM(calculated_area_ha), 0.00) AS area_tot, COALESCE(COUNT(1), 00.00) AS num_car
                  FROM public.${tableName} ${sqlWhere}`;
        } else {
          sql = params.groupCod === 'FOCOS' ? ` SELECT 0.00 AS num_focos, 00.00 AS num_car ` : ` SELECT 0.00 AS area_tot, 00.00 AS num_car `;
        }
        try {
          const result = await conn.execute(sql);

          await conn.disconnect();
          response.json(result.rows);
        } catch (error) {
          console.log(error);
        }
      }
    }
} ();
