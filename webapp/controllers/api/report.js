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
      getData: async (request, response) => {
        let {
          projectName,
          tableName,
          limit,
          offset,
          group,
          date,
          localization,
          area
        } = request.query

        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        let sql = `
            SELECT * FROM public.${tableName}
        `;

        let sqlCount = `SELECT COUNT(*) AS count FROM public.${tableName}`

        if (limit) {
          sql +=` LIMIT ${limit}`
        }

        if(offset) {
          sql +=` OFFSET ${offset}`
        }

        let result
        let resultCount
        try {
          result = await conn.execute(sql)
          resultCount = await conn.execute(sqlCount)
          let dataJson = result.rows
          dataJson.push(resultCount.rows[0]['count'])
          await conn.disconnect()
          response.json(dataJson)
        } catch (error) {
          console.log(error)
        }
      },
      getPropertiesData: async (request, response) => {
        const {
          projectName,
          car,
          group,
          date,
          localization,
          area
        } = request.query

        const tableName = "de_car_validado_sema"
        viewId = 29

        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        let carInfoJson = [];
        let sql = `
            SELECT
            CAR.numero_do2 AS REGISTRO_CAR,
            CAR.nome_da_p1 AS NOME_PROPRIEDADE,
            CAR.area_ha_ AS AREA,
            CAR.municipio1 AS CIDADE,
            ST_Y(ST_Transform (ST_Centroid(car.geom), 4326)) AS "lat",
            ST_X(ST_Transform (ST_Centroid(car.geom), 4326)) AS "long"
            FROM PUBLIC.${tableName} AS CAR
        `;
        if(car) {
          sql += ` WHERE CAR.numero_do2 = ${car}`
        }
        if(area) {
          sql += ` WHERE CAR.area_ha_ > ${area}`
        }
        const result = await conn.execute(sql)
        let propertyData = result.rows
        carInfoJson["property-data"] = propertyData
        await conn.disconnect()
        response.json(propertyData)
      },
      get: async (request, response) => {
        let {
          projectName, carRegister, viewId
        } = request.query

        const tableName = "de_car_validado_sema"
        viewId = 29

        // const view = await ViewFacade.retrieve(viewId)
        // const dataSeries = await DataManager.getDataSeries({id:view.data_series_id})
        // const dataProvider = await DataManager.getDataProvider({id:dataSeries.data_provider_id})
        // const uri = dataProvider.uri
        // const tableName = dataSeries.dataSets[0].format.table_name
        // const conn = new Connection(uri)

        const conn = new Connection("postgis://mpmt:secreto@terrama2.dpi.inpe.br:5432/mpmt")
        await conn.connect()

        let carInfoJson = {};

        // let sql = `
        //     SELECT
        //     aip.cod_imovel AS "car-register",
        //     aip.num_area AS area,
        //     aip.num_modulo AS "tax-modules",
        //     aip.geocodigo as geocode,
        //     m.nm_municip AS state
        //     FROM public.${tableName} AS aip
        //     INNER JOIN public.municipios AS m
        //     ON
        //     aip.geocodigo = cast(m.cd_geocmu AS int)
        //     WHERE aip.cod_imovel = '${carRegister}'
        // `;

        let sql = `
            SELECT
            car.numero_do2 AS register,
            car.nome_da_p1 AS name,
            car.area_ha_ AS area,
            car.municipio1 as city,
            ST_Y(ST_Transform (ST_Centroid(car.geom), 4326)) AS "lat",
            ST_X(ST_Transform (ST_Centroid(car.geom), 4326)) AS "long"
            FROM public.${tableName} AS car
        `;
        const result = await conn.execute(sql)
        let propertyData = result.rows

        // const geocode = propertyData[0].geocode

        // const state = `terrama2_8:view8`

        // const property = `terrama2_${viewId}:view${viewId}`

        // const propertyCarVision = `${property},terrama2_31:view31,terrama2_32:view32`

        // const stateVision = `${property},${state}`

        // const cityVision = `${property},${state}`

        // const deforestationVision = `${property},terrama2_36:view36`

        // const legalReservDetailedVision = `${property},terrama2_34:view34,terrama2_36:view36`

        // const appDetailedVision = `${property},terrama2_36:view36,terrama2_31:view31`

        // const consolidatedArea = `${property},terrama2_36:view36,terrama2_32:view32`

        // const deforestationDETER1 = `${property},terrama2_36:view36`
        // const deforestationDETER2 = `${property},terrama2_36:view36`

        // const deforestationHistory2017 = `${property},terrama2_38:view38`
        // const deforestationHistory2019 = `${property},terrama2_36:view36`

        // const burnedAreas2017 = `${property},terrama2_43:view43`

        // const vision = [
        //   {
        //     "title": "Visão do Estado",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${stateVision}&bbox=-61.6904258728027,-18.0950622558594,-50.1677627563477,-7.29556512832642&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "layers":stateVision.split(','),
        //     "extent":[-61.6904258728027,-18.0950622558594,-50.1677627563477,-7.29556512832642]
        //   },
        //   {
        //     "title": "Visão do Município",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&layers=${cityVision}&bbox=-61.6904258728027,-18.0950622558594,-50.1677627563477,-7.29556512832642&width=500&height=450&srs=EPSG:4674&CQL_FILTER=geocodigo%20=%20${geocode};cd_geocmu%20=%20${geocode}&format=image/png`,
        //     "layers":cityVision.split(','),
        //     "filter" :`geocodigo=${geocode};cd_geocmu=${geocode}`,
        //     "extent": [-61.6904258728027,-18.0950622558594,-50.1677627563477,-7.29556512832642]
        //   },
        //   {
        //     "title": "Visão do CAR no Imóvel",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${propertyCarVision}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "layers":propertyCarVision.split(','),
        //     "extent": [-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827]
        //   },
        //   {
        //     "title": "Histórico Degradação Ambiental",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "layers":property.split(','),
        //     "extent": [-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827]
        //   },
        //   {
        //     "title": "Desmatamento - 2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${deforestationVision}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "layers":deforestationVision.split(','),
        //     "extent": [-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827]
        //   },
        //   {
        //     "title": "Queimadas - 2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "layers":property.split(','),
        //     "extent": [-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827]
        //   }
        // ]

        // const visionLegend = [
        //   {
        //     "title":"Visão do Estado",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${property}`
        //   },
        //   {
        //     "title":"Visão do Município",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${state}`
        //   },
        //   {
        //     "title":"Visão do CAR no Imóvel",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${property}`
        //   },
        //   {
        //     "title":"Histórico Degradação Ambiental",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${property}`
        //   },
        //   {
        //     "title":"Desmatamento - 2019",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${property}`
        //   },
        //   {
        //     "title":"Queimadas - 2019",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${property}`
        //   }
        // ]

        // const detailedVision = [
        //   {
        //     "title":"Assentamento",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc":"Imóvel não intersecta área de assentamento",
        //     "deforestation":"",
        //     "burned": "",
        //     "disabled": true
        //   },
        //   {
        //     "title":"Terra indígena",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc":"Imóvel não intersecta área de terra indígena",
        //     "deforestation":"",
        //     "burned": "",
        //     "disabled": true
        //   },
        //   {
        //     "title":"Unidade de conservação",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc":"Imóvel não intersecta área de unidade de conservação",
        //     "deforestation":"",
        //     "burned": "",
        //     "disabled": true
        //   },
        //   {
        //     "title":"Reserva legal",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${legalReservDetailedVision}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc":"Total de degradação ambiental identificada em 2019",
        //     "deforestation":"108 ha (1,08 km²)",
        //     "burned": "57 ha (0,57 km²)",
        //     "disabled": false
        //   },
        //   {
        //     "title":"APP",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${appDetailedVision}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc":"Imóvel possui Área de Preservação Permanente sem ocorrência de Desmatamentos ou Queimadas.",
        //     "deforestation":"",
        //     "burned": "",
        //     "disabled": false
        //   },
        //   {
        //     "title":"Área consolidada",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${consolidatedArea}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc":"Total de degradação ambiental identificada em 2019",
        //     "deforestation":"14,4 ha (1,44 km²)",
        //     "burned": "",
        //     "disabled": false
        //   },
        // ]

        // const deforestation = [
        //   {
        //     "title":"Desmatamento DETER sobre imagem LANDSAT 28/04/2019",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${deforestationDETER1}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=500&srs=EPSG:4674&format=image/png`
        //   },
        //   {
        //     "title":"Desmatamento DETER sobre imagem LANDSAT 07/02/2019",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${deforestationDETER2}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=500&srs=EPSG:4674&format=image/png`
        //   }
        // ]

        // const deforestationHistory = [
        //   {
        //     "title":"PRODES 2015",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "0.58 km²"
        //   },
        //   {
        //     "title":"PRODES 2016",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "1.22 km²"
        //   },
        //   {
        //     "title":"PRODES 2017",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${deforestationHistory2017}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "0.59 km²"
        //   },
        //   {
        //     "title":"PRODES 2018",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "0.57 km²"
        //   },
        //   {
        //     "title":"PRODES 2019",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${deforestationHistory2019}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "1.17 km²"
        //   }
        // ]

        // const fireFocus = [
        //   {
        //     "title":"2015",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "183 focos"
        //   },
        //   {
        //     "title":"2016",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "1 foco"
        //   },
        //   {
        //     "title":"2017",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "184 focos"
        //   },
        //   {
        //     "title":"2018",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "4 focos"
        //   },
        //   {
        //     "title":"2019",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc": "1 foco"
        //   }
        // ]

        // const fireFocusChart = [
        //   {"date":"2015","value":183},
        //   {"date":"2016","value":1},
        //   {"date":"2017","value":184},
        //   {"date":"2018","value":4},
        //   {"date":"2019","value":1}
        // ]

        // const fireFocusLegend = [
        //   {
        //     "title":"2015",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${property}`
        //   },
        //   {
        //     "title":"2016",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${state}`
        //   },
        //   {
        //     "title":"2017",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${property}`
        //   },
        //   {
        //     "title":"2018",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${property}`
        //   },
        //   {
        //     "title":"2019",
        //     "url" : `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?REQUEST=GetLegendGraphic&TILED=true&VERSION=1.0.0&FORMAT=image/png&WIDTH=20&HEIGHT=20&LAYER=${property}`
        //   }
        // ]

        // const burnedAreas = [
        //   {
        //     "title":"Acumulado 2015",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc":"0,57 km²"
        //   },
        //   {
        //     "title":"Acumulado 2017",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${burnedAreas2017}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc":"27,3 km²",
        //   },
        //   {
        //     "title":"Acumulado 2019",
        //     "url":`http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //     "desc":"33,6 km²"
        //   }
        // ]

        // const burnedAreasBarChart = [
        //   [
        //     {"date":"2015","value":33.66},
        //     {"date":"2017","value":27.33},
        //     {"date":"2019","value":0.57}
        //   ]
        // ]

        // const burnedAreasPieChart = [
        //   [
        //     {"area": "Área queimada","value": 33.6},
        //     {"area": "Área imóvel","value": 69.64}
        //   ],
        //   [
        //     {"area": "Área queimada","value": 27.33},
        //     {"area": "Área imóvel","value": 69.64}
        //   ],
        //   [
        //     {"area": "Área queimada","value": 0.57},
        //     {"area": "Área imóvel","value": 69.64}
        //   ]
        // ]

        // const historyLandsat8 = [
        //   {
        //     "title":"06/01/2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //   },
        //   {
        //     "title":"22/01/2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //   },
        //   {
        //     "title":"07/02/2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //   },
        //   {
        //     "title":"23/02/2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //   },
        //   {
        //     "title":"11/03/2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //   },
        //   {
        //     "title":"12/04/2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //   },
        //   {
        //     "title":"28/04/2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //   },
        //   {
        //     "title":"14/05/2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //   },
        //   {
        //     "title":"30/05/2019",
        //     "url": `http://www.terrama2.dpi.inpe.br/mpmt/geoserver/wms?service=WMS&version=1.1.0&request=GetMap&TILED=true&layers=${property}&bbox=-56.0693332618758,-14.8137331428773,-55.9490165608853,-14.6905806618827&width=500&height=450&srs=EPSG:4674&format=image/png`,
        //   },
        // ]

        carInfoJson["property-data"] = propertyData

        // carInfoJson["vision"] = vision

        // carInfoJson["vision-legend"] = visionLegend

        // carInfoJson["detailed-vision"] = detailedVision

        // carInfoJson["deforestation"] = deforestation

        // carInfoJson["deforestation-history"] = deforestationHistory

        // carInfoJson["fire-focus"] = fireFocus

        // carInfoJson["fire-focus-chart"] = fireFocusChart

        // carInfoJson["fire-focus-legend"] = fireFocusLegend

        // carInfoJson["burned-areas"] = burnedAreas

        // carInfoJson["burned-areas-bar-chart"] = burnedAreasBarChart

        // carInfoJson["burned-areas-pie-chart"] = burnedAreasPieChart

        // carInfoJson["history-landsat-8"] = historyLandsat8

        await conn.disconnect()
        response.json(propertyData)
      },
    }
} ();
