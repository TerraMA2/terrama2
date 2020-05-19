# TerraMA² Webmonitor user preferences

## Default language

**Options**

    pt - Portuguese
    en - English
    es - Espanish
 
**Example**

    "language": "pt"

## Default project

**Example**

    "project": "project name"
    
## Default latlong

Latitude and longitude to center the map.

**Example**

    "latLong": [-55, -15]

## Default Zoom

Map zoom level  (1 to 18).

**Example**

    "zoom": "3"

## Selected layers

Layers to start selected.

**Example**

    "selectedLayers": [
	    "layer1",
	    "layer2"
    ]

## Zoom to layer

Zoom to a specific layer. The layer must be on the selectedLayers list.

**Example**

    zoomToLayer: "layerName"

## Default template

**Options**

    osm - Open Street Maps
    sentinel - Sentinel
    gebco - GEBCO

**Example**

    template: "osm"

## Logo

Path of the logo to show on the sidebar header. Image must be place on the folder "/opt/terrama2/4.1.0/webmonitor/public/images/" through the volume "webmonitor_images".

**Example**

    "logo": "images/terrama2_logo.png"

## Logo link

Link to open on logo click.

**Example**

    "logoLink": "http://www.terrama2.dpi.inpe.br/"

## Logo footer

Logos to show on the sidebar footer. 

**Example**

    "logoFooter": [
    	{ 
    		link: "http://www.inpe.br/",
    		path: "images/inpe.png"
    	}
    ]

