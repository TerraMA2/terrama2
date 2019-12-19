  const {Connection} = require('../../core/utility/connection');
  const DataManager = require('../../core/DataManager');
  const ViewFacade = require("../../core/facade/View");
  const ReportService = require("../../core/facade/ReportService")


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
    return {
      getStaticData: async (request, response) => {
        const {
          limit,
          offset,
          view,
          countTotal,
          sortColumn,
          sortOrder
        } = request.query;

        const conn = new Connection(URI);
        await conn.connect();

        const viewParam = JSON.parse(view);

        const viewAux = await ViewFacade.retrieve(viewParam.id);
        const dataSeries = await DataManager.getDataSeries({id: viewAux.data_series_id});
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id});

        const dataSet = dataSeries.dataSets[0];
        let tableName = dataSet.format.table_name;

        let sqlSelect = `SELECT *`;
        let sqlFrom = '';
        let sqlWhere = '';

        sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(geom), 4326)) AS "lat",
                      ST_X(ST_Transform (ST_Centroid(geom), 4326)) AS "long"
        `;


        sqlFrom += ` FROM public.${tableName}`;

        if (sortColumn && sortOrder) {
          sqlWhere += ` ORDER BY ${sortColumn} ${sortOrder === 1?'ASC':'DESC'}`
        }

        if (limit) {
          sqlWhere += ` LIMIT ${limit}`
        }

        if (offset) {
          sqlWhere += ` OFFSET ${offset}`
        }

        const sql = sqlSelect + sqlFrom + sqlWhere;

        let result;
        let resultCount;
        try {
          result = await conn.execute(sql);
          let dataJson = result.rows;


          let sqlCount;
          if (countTotal) {
            sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName}`;
            resultCount = await conn.execute(sqlCount);

            dataJson.push(resultCount.rows[0]['count']);
          }

          await conn.disconnect();
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
          view,
          countTotal,
          sortColumn,
          sortOrder
        } = request.query;

        const viewParam = JSON.parse(view);

        const viewAuxi = await ViewFacade.retrieve(viewParam.id);
        const dataSeries = await DataManager.getDataSeries({id: viewAuxi.data_series_id});
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id});
        const uri = dataProvider.uri;

        const conn = new Connection(URI);
        await conn.connect();

        const dataSet = dataSeries.dataSets[0];
        const tableName = dataSet.format.table_name;
        const geomColumn = dataSet.format.geometry_property;
        const timeStampColumn = dataSet.format.timestamp_property;

        let sqlSelect = `SELECT *`;
        let sqlFrom = '';
        let sqlWhere = '';

        if (geomColumn) {
          sqlSelect += `, ST_Y(ST_Transform (ST_Centroid(${geomColumn}), 4326)) AS "lat",
                        ST_X(ST_Transform (ST_Centroid(${geomColumn}), 4326)) AS "long"
          `
        }

        sqlFrom += ` FROM public.${tableName}`;

        if (date) {
          const dateFrom = date[0];
          const dateTo = date[1];

          sqlWhere += `
              WHERE ${timeStampColumn}::date >= '${dateFrom}' AND ${timeStampColumn}::date <= '${dateTo}'
          `
        }

        if (sortColumn && sortOrder) {
          sqlWhere += ` ORDER BY ${sortColumn} ${sortOrder === 1?'ASC':'DESC'}`
        }

        if (limit) {
          sqlWhere += ` LIMIT ${limit}`
        }

        if (offset) {
          sqlWhere += ` OFFSET ${offset}`
        }

        const sql = sqlSelect + sqlFrom + sqlWhere;

        let result;
        let resultCount;
        try {
          result = await conn.execute(sql);
          let dataJson = result.rows;

          let sqlCount;
          if (countTotal) {
            sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName}`;
            resultCount = await conn.execute(sqlCount);
            dataJson.push(resultCount.rows[0]['count'])
          }

          await conn.disconnect();
          response.json(dataJson)
        } catch (error) {
          console.log(error)
        }
      },
      getAnalysisData: async (request, response) => {
        const params = JSON.parse(request.query.specificParameters);
        params.date = request.query.date;
        params.filter = request.query.filter;

        const result = await ReportService.findAnaliseData(params);

        response.json(result);
      },
      getCarData: async (request, response) => {
        const {
          carRegister,
          date,
          viewId
        } = request.query;

        const view = await ViewFacade.retrieve(viewId);
        const dataSeries = await DataManager.getDataSeries({id: view.data_series_id});
        const dataProvider = await DataManager.getDataProvider({id: dataSeries.data_provider_id});
        // const uri = dataProvider.uri
        const tableName = dataSeries.dataSets[0].format.table_name;

        const conn = new Connection(URI);
        await conn.connect();
        let dateFrom = null;
        let dateTo = null;

        if (date) {
          dateFrom = date[0];
          dateTo = date[1];
        }

        let sql = `SELECT
            car.numero_do1 AS register,
            car.area_ha_ AS area,
            car.nome_da_p1 AS name,
            car.municipio1 AS city,
            car.cpfcnpj AS cpf,
            car.nomepropri AS owner,
            munic.comarca AS county,
            substring(ST_EXTENT(munic.geom)::TEXT, 5, length(ST_EXTENT(munic.geom)::TEXT) - 5) as citybbox,
            substring(ST_EXTENT(car.geom)::TEXT, 5, length(ST_EXTENT(car.geom)::TEXT) - 5) as bbox,
            ST_Y(ST_Transform (ST_Centroid(car.geom), 4326)) AS "lat",
            ST_X(ST_Transform (ST_Centroid(car.geom), 4326)) AS "long"
            FROM public.${tableName} AS car
            INNER JOIN de_municipios_sema munic ON
            car.numero_do1 = '${carRegister}'
            AND munic.municipio = car.municipio1
            GROUP BY car.numero_do1, car.area_ha_, car.nome_da_p1, car.municipio1, car.geom, munic.comarca, car.cpfcnpj, car.nomepropri`;

        const result = await conn.execute(sql);
        let propertyData = result.rows[0];

        const sqlBurningSpotlights = `
                    SELECT
                    count(*) as focuscount,
                    extract('YEAR' FROM focus.execution_date) as year
                    FROM public.a_carfocos_28 as focus
                    INNER JOIN public.${tableName} AS car on
                    focus.de_car_validado_sema_numero_do1 = car.numero_do1 AND
                    extract('YEAR' FROM focus.execution_date) > 2007 AND
                    car.numero_do1 = '${carRegister}'
                    group by year
                  `;

        const resultBurningSpotlights = await conn.execute(sqlBurningSpotlights);
        const burningSpotlights = resultBurningSpotlights.rows;

        const sqlBurnedAreas = `
                    SELECT
                    SUM(areaq.calculated_area_ha) as burnedAreas,
                    extract('YEAR' FROM areaq.execution_date) as year
                    FROM public.a_caraq_40 as areaq
                    INNER JOIN public.${tableName} AS car on
                    areaq.de_car_validado_sema_numero_do1 = car.numero_do1 AND
                    car.numero_do1 = '${carRegister}'
                    group by year
                  `;

        const resultBurnedAreas = await conn.execute(sqlBurnedAreas);
        const burnedAreas = resultBurnedAreas.rows;

        const sqlBurnedAreasYear = `SELECT
                              extract(year from date_trunc('year', areaq.execution_date)) AS date,
                              SUM(areaq.calculated_area_ha) as burnedAreas
                              FROM public.a_caraq_40 areaq
                              WHERE areaq.de_car_validado_sema_numero_do1 = '${carRegister}'
                              GROUP BY date
                              ORDER BY date;`;

        const resultBurnedAreasYear = await conn.execute(sqlBurnedAreasYear);
        const burnedAreasYear = resultBurnedAreasYear.rows;

        const sqlProdesYear = `SELECT
                              extract(year from date_trunc('year', cp.execution_date)) AS date,
                              SUM(cp.calculated_area_ha) as area
                              FROM public.a_carprodes_15 cp
                              WHERE cp.de_car_validado_sema_numero_do1 = '${carRegister}'
                              GROUP BY date
                              ORDER BY date;`;

        const sqlDeterYear = `SELECT
                              extract(year from date_trunc('year', cd.execution_date)) AS date,
                              SUM(cd.calculated_area_ha) as area
                              FROM public.a_cardeter_3 cd
                              WHERE cd.de_car_validado_sema_numero_do1 = '${carRegister}'
                              GROUP BY date
                              ORDER BY date;`;

        const sqlSpotlightsYear = `SELECT
                              extract(year from date_trunc('year', cf.execution_date)) AS date,
                              COUNT(cf.*) as spotlights
                              FROM public.a_carfocos_28 cf
                              WHERE cf.de_car_validado_sema_numero_do1 = '${carRegister}' AND
                              extract('YEAR' FROM cf.execution_date) > 2007
                              GROUP BY date
                              ORDER BY date;`;

        let dateSql = ` and execution_date::date >= '${dateFrom}' AND execution_date::date <= '${dateTo}'`;

        const sqlProdesArea = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_15 where de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;

        const sqlIndigenousLand = `SELECT SUM(calculated_area_ha) AS area FROM public.a_cardeter_ti_9 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConservationUnit = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_uc_23 where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlLegalReserve = `SELECT SUM(calculated_area_ha) AS area FROM public.a_cardeter_reserva_6 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlAPP = `SELECT SUM(calculated_area_ha) AS area FROM public.a_cardeter_app_4 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConsolidatedUse = `SELECT SUM(calculated_area_ha) AS area FROM public.a_cardeter_usocon_8 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlAnthropizedUse = `SELECT SUM(calculated_area_ha) AS area FROM public.a_cardeter_usoant_5 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlNativeVegetation = `SELECT SUM(calculated_area_ha) AS area FROM public.a_cardeter_veg_7 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;

        const resultIndigenousLand = await conn.execute(sqlIndigenousLand);
        const indigenousLand = resultIndigenousLand.rows;

        const resultConservationUnit = await conn.execute(sqlConservationUnit);
        const conservationUnit = resultConservationUnit.rows;

        const resultLegalReserve = await conn.execute(sqlLegalReserve);
        const legalReserve = resultLegalReserve.rows;

        const resultAPP = await conn.execute(sqlAPP);
        const app = resultAPP.rows;

        const resultConsolidatedUse = await conn.execute(sqlConsolidatedUse);
        const consolidatedArea = resultConsolidatedUse.rows;

        const resultAnthropizedUse = await conn.execute(sqlAnthropizedUse);
        const anthropizedUse = resultAnthropizedUse.rows;

        const resultNativeVegetation = await conn.execute(sqlNativeVegetation);
        const nativeVegetation = resultNativeVegetation.rows;


        const sqlAPPDETERCount = `SELECT SUM(calculated_area_ha) AS count FROM public.a_cardeter_app_4 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlLegalReserveDETERCount = `SELECT SUM(calculated_area_ha) AS count FROM public.a_cardeter_reserva_6 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConservationUnitDETERCount = `SELECT SUM(calculated_area_ha) AS count FROM public.a_cardeter_uc_10 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlIndigenousLandDETERCount = `SELECT SUM(calculated_area_ha) AS count FROM public.a_cardeter_ti_9 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConsolidatedUseDETERCount = `SELECT SUM(calculated_area_ha) AS count FROM public.a_cardeter_usocon_8 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        // const sqlExploraDETERCount = `SELECT SUM(calculated_area_ha) AS count FROM public.a_explora_cardeter_ where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlDesmateDETERCount = `SELECT SUM(calculated_area_ha) AS count FROM public.a_cardeter_desmate_13 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlEmbargoedAreaDETERCount = `SELECT SUM(calculated_area_ha) AS count FROM public.a_cardeter_emb_11 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlLandAreaDETERCount = `SELECT SUM(calculated_area_ha) AS count FROM public.a_cardeter_desemb_12 where a_cardeter_3_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;

        const resultAPPDETERCount = await conn.execute(sqlAPPDETERCount);
        const aPPDETERCount = resultAPPDETERCount.rows;

        const resultLegalReserveDETERCount = await conn.execute(sqlLegalReserveDETERCount);
        const legalReserveDETERCount = resultLegalReserveDETERCount.rows;

        const resultConservationUnitDETERCount = await conn.execute(sqlConservationUnitDETERCount);
        const conservationUnitDETERCount = resultConservationUnitDETERCount.rows;

        const resultIndigenousLandDETERCount = await conn.execute(sqlIndigenousLandDETERCount);
        const indigenousLandDETERCount = resultIndigenousLandDETERCount.rows;

        const resultConsolidatedUseDETERCount = await conn.execute(sqlConsolidatedUseDETERCount);
        const consolidatedUseDETERCount = resultConsolidatedUseDETERCount.rows;

        // const resultExploraDETERCount = await conn.execute(sqlExploraDETERCount);
        // const explorationDETERCount = resultExploraDETERCount.rows;

        const resultDesmateDETERCount = await conn.execute(sqlDesmateDETERCount);
        const deforestationDETERCount = resultDesmateDETERCount.rows;

        const resultEmbargoedAreaDETERCount = await conn.execute(sqlEmbargoedAreaDETERCount);
        const embargoedAreaDETERCount = resultEmbargoedAreaDETERCount.rows;

        const resultLandAreaDETERCount = await conn.execute(sqlLandAreaDETERCount);
        const landAreaDETERCount = resultLandAreaDETERCount.rows;

        const sqlAPPPRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_app_16 where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlLegalReservePRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_reserva_19 where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConservationUnitPRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_uc_23 where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlIndigenousLandPRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_ti_22 where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConsolidatedUsePRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_usocon_21 where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        // const sqlExploraPRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_explora_carprodes_ where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlDesmatePRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_desmate_26 where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlEmbargoedAreaPRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_emb_24 where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlLandAreaPRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_carprodes_desemb_25 where a_carprodes_15_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;

        const resultAPPPRODESSum = await conn.execute(sqlAPPPRODESSum);
        const aPPPRODESSum = resultAPPPRODESSum.rows;

        const resultLegalReservePRODESSum = await conn.execute(sqlLegalReservePRODESSum);
        const legalReservePRODESSum = resultLegalReservePRODESSum.rows;

        const resultConservationUnitPRODESSum = await conn.execute(sqlConservationUnitPRODESSum);
        const conservationUnitPRODESSum = resultConservationUnitPRODESSum.rows;

        const resultIndigenousLandPRODESSum = await conn.execute(sqlIndigenousLandPRODESSum);
        const indigenousLandPRODESSum = resultIndigenousLandPRODESSum.rows;

        const resultConsolidatedUsePRODESSum = await conn.execute(sqlConsolidatedUsePRODESSum);
        const consolidatedUsePRODESSum = resultConsolidatedUsePRODESSum.rows;

        // const resultExploraPRODESSum = await conn.execute(sqlExploraPRODESSum);
        // const explorationPRODESSum = resultExploraPRODESSum.rows;

        const resultDesmatePRODESSum = await conn.execute(sqlDesmatePRODESSum);
        const deforestationPRODESSum = resultDesmatePRODESSum.rows;

        const resultEmbargoedAreaPRODESSum = await conn.execute(sqlEmbargoedAreaPRODESSum);
        const embargoedAreaPRODESSum = resultEmbargoedAreaPRODESSum.rows;

        const resultLandAreaPRODESSum = await conn.execute(sqlLandAreaPRODESSum);
        const landAreaPRODESSum = resultLandAreaPRODESSum.rows;

        const sqlAPPFOCOSCount = `SELECT COUNT(*) AS count FROM public.a_carfocos_app_29 where a_carfocos_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlLegalReserveFOCOSCount = `SELECT COUNT(*) AS count FROM public.a_carfocos_reserva_31 where a_carfocos_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConservationUnitFOCOSCount = `SELECT COUNT(*) AS count FROM public.a_carfocos_uc_35 where a_carfocos_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlIndigenousLandFOCOSCount = `SELECT COUNT(*) AS count FROM public.a_carfocos_ti_34 where a_carfocos_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConsolidatedUseFOCOSCount = `SELECT COUNT(*) AS count FROM public.a_carfocos_usocon_33 where a_carfocos_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        // const sqlExploraFOCOSCount = `SELECT COUNT(*) AS count FROM public.apv_explora_carfocos_ where a_carfocos_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlDesmateFOCOSCount = `SELECT COUNT(*) AS count FROM public.a_carfocos_desmate_38 where a_carfocos_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlEmbFOCOSCount = `SELECT COUNT(*) AS count FROM public.a_carfocos_emb_36 where a_carfocos_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlLandAreaFOCOSCount = `SELECT COUNT(*) AS count FROM public.a_carfocos_desemb_37 where a_carfocos_28_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;

        const resultAPPFOCOSCount = await conn.execute(sqlAPPFOCOSCount);
        const aPPFOCOSCount = resultAPPFOCOSCount.rows;

        const resultLegalReserveFOCOSCount = await conn.execute(sqlLegalReserveFOCOSCount);
        const legalReserveFOCOSCount = resultLegalReserveFOCOSCount.rows;

        const resultConservationUnitFOCOSCount = await conn.execute(sqlConservationUnitFOCOSCount);
        const conservationUnitFOCOSCount = resultConservationUnitFOCOSCount.rows;

        const resultIndigenousLandFOCOSCount = await conn.execute(sqlIndigenousLandFOCOSCount);
        const indigenousLandFOCOSCount = resultIndigenousLandFOCOSCount.rows;

        const resultConsolidatedUseFOCOSCount = await conn.execute(sqlConsolidatedUseFOCOSCount);
        const consolidatedUseFOCOSCount = resultConsolidatedUseFOCOSCount.rows;

        // const resultExploraFOCOSCount = await conn.execute(sqlExploraFOCOSCount);
        // const explorationFOCOSCount = resultExploraFOCOSCount.rows;

        const resultDesmateFOCOSCount = await conn.execute(sqlDesmateFOCOSCount);
        const deforestationFOCOSCount = resultDesmateFOCOSCount.rows;

        const resultEmbFOCOSCount = await conn.execute(sqlEmbFOCOSCount);
        const embargoedAreaFOCOSCount = resultEmbFOCOSCount.rows;

        const resultLandAreaFOCOSCount = await conn.execute(sqlLandAreaFOCOSCount);
        const landAreaFOCOSCount = resultLandAreaFOCOSCount.rows;

        const sqlAPPBURNEDAREASum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_caraq_app_41 where a_caraq_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlLegalReserveBURNEDAREASum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_caraq_reserva_43 where a_caraq_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConservationUnitBURNEDAREASum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_caraq_uc_47 where a_caraq_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlIndigenousLandBURNEDAREASum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_caraq_ti_46 where a_caraq_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlConsolidatedUseBURNEDAREASum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_caraq_usocon_45 where a_caraq_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        // const sqlExploraPRODESSum = `SELECT SUM(calculated_area_ha) AS area FROM public.apv_explora_carprodes_ where a_caraq_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlDesmateBURNEDAREASum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_caraq_desmate_50 where a_caraq_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlEmbargoedAreaBURNEDAREASum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_caraq_emb_48 where a_caraq_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;
        const sqlLandAreaBURNEDAREASum = `SELECT SUM(calculated_area_ha) AS area FROM public.a_caraq_desemb_49 where a_caraq_40_de_car_validado_sema_numero_do1 = '${carRegister}' ${dateSql}`;

        const resultAPPBURNEDAREASum = await conn.execute(sqlAPPBURNEDAREASum);
        const aPPBURNEDAREASum = resultAPPBURNEDAREASum.rows;

        const resultLegalReserveBURNEDAREASum = await conn.execute(sqlLegalReserveBURNEDAREASum);
        const legalReserveBURNEDAREASum = resultLegalReserveBURNEDAREASum.rows;

        const resultConservationUnitBURNEDAREASum = await conn.execute(sqlConservationUnitBURNEDAREASum);
        const conservationUnitBURNEDAREASum = resultConservationUnitBURNEDAREASum.rows;

        const resultIndigenousLandBURNEDAREASum = await conn.execute(sqlIndigenousLandBURNEDAREASum);
        const indigenousLandBURNEDAREASum = resultIndigenousLandBURNEDAREASum.rows;

        const resultConsolidatedUseBURNEDAREASum = await conn.execute(sqlConsolidatedUseBURNEDAREASum);
        const consolidatedUseBURNEDAREASum = resultConsolidatedUseBURNEDAREASum.rows;

        // const resultExploraPRODESSum = await conn.execute(sqlExploraPRODESSum);
        // const explorationPRODESSum = resultExploraPRODESSum.rows;

        const resultDesmateBURNEDAREASum = await conn.execute(sqlDesmateBURNEDAREASum);
        const deforestationBURNEDAREASum = resultDesmateBURNEDAREASum.rows;

        const resultEmbargoedAreaBURNEDAREASum = await conn.execute(sqlEmbargoedAreaBURNEDAREASum);
        const embargoedAreaBURNEDAREASum = resultEmbargoedAreaBURNEDAREASum.rows;

        const resultLandAreaBURNEDAREASum = await conn.execute(sqlLandAreaBURNEDAREASum);
        const landAreaBURNEDAREASum = resultLandAreaBURNEDAREASum.rows;

        const resultProdesArea = await conn.execute(sqlProdesArea);
        const prodesArea = resultProdesArea.rows;

        const resultDeterYear = await conn.execute(sqlDeterYear);
        const deterYear = resultDeterYear.rows;

        const resultProdesYear = await conn.execute(sqlProdesYear);
        const prodesYear = resultProdesYear.rows;

        const resultSpotlightsYear = await conn.execute(sqlSpotlightsYear);
        const spotlightsYear = resultSpotlightsYear.rows;

        if (propertyData) {
          propertyData.burningSpotlights = burningSpotlights;
          propertyData.burnedAreas = burnedAreas;
          // propertyData.deter = deter[0]
          propertyData.prodesArea = prodesArea[0]['area'];
          propertyData.prodesYear = prodesYear;
          propertyData.deterYear = deterYear;
          propertyData.spotlightsYear = spotlightsYear;
          propertyData.burnedAreasYear = burnedAreasYear;
          propertyData.indigenousLand = indigenousLand[0];
          propertyData.conservationUnit = conservationUnit[0];
          propertyData.legalReserve = legalReserve[0];
          propertyData.app = app[0];
          propertyData.consolidatedArea = consolidatedArea[0];
          propertyData.anthropizedUse = anthropizedUse[0];
          propertyData.nativeVegetation = nativeVegetation[0];

          propertyData.app = {
            affectedArea: 'APP',
            recentDeforestation: aPPDETERCount[0]['count']|'',
            pastDeforestation: aPPPRODESSum[0]['area']|'',
            burnlights: aPPFOCOSCount[0]['count']|'',
            burnAreas: aPPBURNEDAREASum[0]['area']|''
          }

          propertyData.legalReserve = {
            affectedArea: 'ARL',
            recentDeforestation: legalReserveDETERCount[0]['count']|'',
            pastDeforestation: legalReservePRODESSum[0]['area']|'',
            burnlights: legalReserveFOCOSCount[0]['count']|'',
            burnAreas: legalReserveBURNEDAREASum[0]['area']|'',
          }

          propertyData.conservationUnit = {
            affectedArea: 'UC',
            recentDeforestation: conservationUnitDETERCount[0]['count']|'',
            pastDeforestation: conservationUnitPRODESSum[0]['area']|'',
            burnlights: conservationUnitFOCOSCount[0]['count']|'',
            burnAreas: conservationUnitBURNEDAREASum[0]['area']|''
          }

          propertyData.indigenousLand = {
            affectedArea: 'TI',
            recentDeforestation: indigenousLandDETERCount[0]['count']|'',
            pastDeforestation: indigenousLandPRODESSum[0]['area']|'',
            burnlights: indigenousLandFOCOSCount[0]['count']|'',
            burnAreas: indigenousLandBURNEDAREASum[0]['area']|'',
          }

          propertyData.consolidatedUse = {
            affectedArea: 'AUC',
            recentDeforestation: consolidatedUseDETERCount[0]['count']|'',
            pastDeforestation: consolidatedUsePRODESSum[0]['area']|'',
            burnlights: consolidatedUseFOCOSCount[0]['count']|'',
            burnAreas: consolidatedUseBURNEDAREASum[0]['area']|'',
          }

          // propertyData.exploration = {
          //   affectedArea: 'AUTEX',
          //   recentDeforestation: explorationDETERCount[0],
          //   pastDeforestation: explorationPRODESSum[0],
          //   burnlights: explorationFOCOSCount[0],
          //   burnAreas: ''
          // }

          propertyData.deforestation = {
            affectedArea: 'AD',
            recentDeforestation: deforestationDETERCount[0]['count']|'',
            pastDeforestation: deforestationPRODESSum[0]['area']|'',
            burnlights: deforestationFOCOSCount[0]['count']|'',
            burnAreas: deforestationBURNEDAREASum[0]['area']|'',
          }

          propertyData.embargoedArea = {
            affectedArea: 'Área embargada',
            recentDeforestation: embargoedAreaDETERCount[0]['count']|'',
            pastDeforestation: embargoedAreaPRODESSum[0]['area']|'',
            burnlights: embargoedAreaFOCOSCount[0]['count']|'',
            burnAreas: embargoedAreaBURNEDAREASum[0]['area']|'',
          }

          propertyData.landArea = {
            affectedArea: 'Área desembargada',
            recentDeforestation: landAreaDETERCount[0]['count']|'',
            pastDeforestation: landAreaPRODESSum[0]['area']|'',
            burnlights: landAreaFOCOSCount[0]['count']|'',
            burnAreas: landAreaBURNEDAREASum[0]['area']|'',
          }

        }

        await conn.disconnect();
        response.json(propertyData);
      }
    }
} ();
